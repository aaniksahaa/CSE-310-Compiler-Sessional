#include<bits/stdc++.h>
#include "headers/2005001_SymbolTable.hpp"
#include "headers/2005001_SymbolInfo.hpp"

using namespace std;

// extern variables and functions

extern int total_error_count;

extern SymbolTable* ST;     // symboltable that is being used so far    // scope 1 yet not deleted
extern SymbolInfo* root;
extern void copy_type_and_properties(SymbolInfo* destination, SymbolInfo* source);
extern void run_parser(int argc,char *argv[]);

// global variables

map<string,string> op_to_assembly {
    {">", "JG"},
    {"<", "JL"},
    {">=", "JGE"},
    {"<=", "JLE"},
    {"==", "JE"},
    {"!=", "JNE"}
};


// Please note carefully that,
// the local spacing through stack_offset, does not only rely on the current function
// it also largely replies upon the current scope...
// if not handles carefully, infinite loop cases may appear...


// carefully handle these global marker variables

int current_line_number = 0;
int pending_line_number = -1;

bool is_current_scope_global;
int current_function_local_space; // space occupied by local variables of current function

int previous_scopes_local_space; // space taken by previous scopes of current function
int current_scope_local_space;  // this needs to be kept under re-calculation, becauise of handling abrupt return

int current_function_return_label_id = -1;

vector<SymbolInfo*>pending_parameter_list;

// single global variable to keep track of generated labels
int global_label_count = 0;

// introducing a new symbol-table for tracking variables
// because the previous symbol tables have already been destroyed with scope exiting...
SymbolTable* ST2 = new SymbolTable(11);


// withholding following approach as it will introduce difficulties 

// identifier name to stack position mapping
// i need not run any checks on existence, because there will be no error in the given code

// map<string,int>stack_position_map;  // this holds signed offset

ofstream codeout("code.asm");
ofstream icg_debug_out("icg_debug.txt");

extern string quote;
extern string space;
extern string newline;

// pre-declarations

void gen_start(SymbolInfo* curr);
void gen_program(SymbolInfo* curr);
void gen_unit(SymbolInfo* curr);
void gen_func_definition(SymbolInfo* curr);
void gen_compound_statement(SymbolInfo* curr);
void gen_statements(SymbolInfo* curr);
void gen_statement(SymbolInfo* curr);
void gen_var_declaration(SymbolInfo* curr);
void gen_expression_statement(SymbolInfo* curr);
void gen_expression(SymbolInfo* curr);
void gen_logic_expression(SymbolInfo* curr);
void gen_rel_expression(SymbolInfo* curr);
void gen_simple_expression(SymbolInfo* curr);
void gen_term(SymbolInfo* curr);
void gen_unary_expression(SymbolInfo* curr);
void gen_factor(SymbolInfo* curr);
void gen_variable(SymbolInfo* curr);
void gen_argument_list(SymbolInfo* curr);
void gen_arguments(SymbolInfo* curr);

// helpers 

// check whether expression is the result of a top-level logicop or relop
bool check_logic_rel_expression(SymbolInfo* logic_expression){

    if(logic_expression->name == "rel_expression LOGICOP rel_expression"){
        return true;
    }
    auto rel_expression = logic_expression->child_symbols[0];
    if(rel_expression->name == "simple_expression RELOP simple_expression"){
        return true;
    }

    return false;
    
}

string get_signed_string(int n){
    string result = to_string(n);
    if(n >= 0){
        result = "+" + result;
    }
    return result;
}

string left_pad(string s)
{
	vector<string> lines;
    istringstream iss(s);
    string line;

    while (getline(iss, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    string result;

    if(pending_line_number != -1 && lines.size()>0){
        lines[0] += string("\t\t\t; Line ")+to_string(pending_line_number);
        pending_line_number = -1;
    }

    for (const auto& line : lines) {
        result += "\t" + line + "\n";
    }

    return result;
}

// generates a new label
int get_new_label(){
    global_label_count++;
    return global_label_count;
}

void copy_boolean_labels(SymbolInfo* dest, SymbolInfo* source){
    dest->true_label_id = source->true_label_id;
    dest->false_label_id = source->false_label_id;
}


// Writer Functions


void write_header(string s){
	codeout<<s;
}

void write_footer(string s){
	codeout<<s;
}

void write_label(int label_id){
	codeout<<string("L")+to_string(label_id)+string(":\n");
}

void write_code(string s){
	codeout<<left_pad(s);
}

void write_unchanged(string s){
    codeout<<s;
}

void write_print_library(){
    write_header("\n;-------------------------------\n;         print library         \n;-------------------------------\n\n");
    ifstream p_file("print_library.txt");

    // Read the contents of the file into a string
    string content;
    std::string line;

    while (getline(p_file, line)) {
        content += line + "\n";
    }

    // Close the file
    p_file.close();

    write_unchanged(content);

}


// Debugger Functions


void debug_line(int line_number){
    icg_debug_out<<"Line "<<line_number<<":"<<endl;
}
void idebug(string s){
    icg_debug_out<<s<<endl;
}
void idebug(SymbolInfo* curr){
    icg_debug_out<<"at Line "<<curr->get_start_line_count()<<": "<<curr->type<<endl;
}


// Preproceesor for debugging and Line Number printing


void preprocess(SymbolInfo*curr){
    idebug(curr);
    if(curr->get_start_line_count() > current_line_number){
        pending_line_number = curr->get_start_line_count();
        current_line_number = curr->get_start_line_count();
    }
}


// necessary in special cases, where get_variable_indicator cannot be called

string get_local_variable_address(string name){
    int position = ST2->lookup(name)->stack_starting_position;
    string result = string("[BP")+get_signed_string(position)+string("]");
    return result;
}


// note that this function also writes code...
// so don't use it in between writing other code
// indicator here means, the memory indicator, like [BP-2], w[BX] etc...

string get_variable_indicator(SymbolInfo* variable){

    preprocess(variable);

    string result = "";
    auto symbol = ST2->lookup(variable->child_symbols[0]->name);

    idebug(symbol->name + space + variable->name);

    if(symbol->check_property(IS_LOCAL_VARIABLE)){      // local variable
        
        idebug("at indicator for local var");

        if(variable->name == "ID"){ // non-array variable

            int position = symbol->stack_starting_position;
            result = string("[BP")+get_signed_string(position)+string("]");

        } 
        else if(variable->name == "ID LSQUARE expression RSQUARE"){ // array element
            
            auto expression = variable->child_symbols[2];

            idebug("at indicator for array");

            gen_expression(expression);
            write_code("POP BX");

            // OPTIMIZATION
            // left shifting 1 bit, so that it gets multiplied by 2
            write_code("SHL BX, 1");      // now BX contains the element offset wrt array
            write_code("MOV AX, "+to_string(symbol->stack_starting_position));
            write_code("ADD AX, BX\nMOV BX, AX\nMOV SI, BX");
            
            result = "[BP+SI]";

        }
    }
    else {   // global variable
        
        idebug("at indicator for global var");

        if(variable->name == "ID"){ // non-array variable
            
            result = symbol->name;

        } 
        else if(variable->name == "ID LSQUARE expression RSQUARE"){ // array element
            
            idebug("at global array");

            auto expression = variable->child_symbols[2];

            gen_expression(expression);
            write_code("POP BX");

            // OPTIMIZATION
            // left shifting 1 bit, so that it gets multiplied by 2
            write_code("SHL BX, 1");      // now BX contains the element offset wrt array
            
            result = symbol->name + string("[BX]");
        
        }
    }

    return result;

}


// This function converts any value to boolean value and pushes to the stack

void gen_boolean_evaluation(SymbolInfo* curr){

    write_code("POP AX");
    write_code("CMP AX, 0");
    write_code(string("JNE L") + to_string(curr->true_label_id));
    write_code(string("JMP L") + to_string(curr->false_label_id));

}


// Code Generators for Non-terminals

void gen_start(SymbolInfo* curr)
{
    preprocess(curr);

    is_current_scope_global = 1;

    auto program = curr->child_symbols[0];

    gen_program(program);

}

void gen_program(SymbolInfo* curr)
{
    preprocess(curr);
    
    if(curr->name == "program unit"){

        auto program = curr->child_symbols[0];
        auto unit = curr->child_symbols[1];

        gen_program(program);
        gen_unit(unit);

    } else if(curr->name == "unit") {

        auto unit = curr->child_symbols[0];

        gen_unit(unit);

    }
}

void gen_unit(SymbolInfo* curr)
{
    preprocess(curr);
    
    if(curr->name == "func_definition"){

        auto func_definition = curr->child_symbols[0];

        gen_func_definition(func_definition);

    }
}

void gen_func_definition(SymbolInfo* curr)
{
    preprocess(curr);
    
    is_current_scope_global = 0;
    current_function_local_space = 0;

    int parameter_count = 0;

    // common upper part
    auto function = curr->child_symbols[1];
    auto children = curr->child_symbols;

    write_header(function->name + " PROC\n");

    if(function->name == "main"){
        write_code("MOV AX, @DATA\nMOV DS, AX\n");
    }

    write_code("PUSH BP\nMOV BP, SP\n");

    int new_label_id = get_new_label();

    current_function_return_label_id = new_label_id;

    pending_parameter_list.clear();

    if(curr->name == "type_specifier ID LPAREN RPAREN compound_statement"){
        
        auto compound_statement = curr->child_symbols[4];

        gen_compound_statement(compound_statement);
        
    } 
    else if(curr->name == "type_specifier ID LPAREN parameter_list RPAREN compound_statement") {
        
        auto parameter_list = curr->child_symbols[3];
        auto compound_statement = curr->child_symbols[5];

        auto temp_list = parameter_list->get_child_identifiers();
        parameter_count = temp_list.size();

        reverse(temp_list.begin(), temp_list.end());

        int current_offset = 4; // will proceed like 4,6,8... in reverse

        for(auto child: temp_list){

            child->set_property(IS_LOCAL_VARIABLE);
            child->stack_starting_position = current_offset;
            current_offset += 2;

            pending_parameter_list.push_back(child);

        }

        gen_compound_statement(compound_statement);
    }

    // common lower part

    // write_code(string("ADD SP, ") + to_string(current_function_local_space));

    write_label(new_label_id);

    write_code("POP BP\n");

    if(function->name != "main"){
        string return_statement = string("RET ")+(parameter_count    ?   to_string(2*parameter_count)    :   string(""));
        write_code(return_statement);
    }

    if(function->name == "main"){
        write_code("MOV AX,4CH\nINT 21H\n");
    }

    write_footer(function->name + " ENDP\n");

    is_current_scope_global = 1;
    current_function_local_space = 0;
    current_function_return_label_id = -1;
}

void gen_compound_statement(SymbolInfo* curr)
{
    preprocess(curr);
    
    ST2->enter_scope();

    for(auto child: pending_parameter_list){

        ST2->insert_into_current_scope(child->name, child->type);

        auto symbol = ST2->lookup(child->name);

        // Note that, this also copies stack offset
        copy_type_and_properties(symbol, child);

        idebug(symbol->name + string(" -> ") + to_string(symbol->stack_starting_position));

    }

    pending_parameter_list.clear();

    current_scope_local_space = 0;

    int space_overhead = 0;

    if(curr->name == "LCURL statements RCURL"){

        auto statements = curr->child_symbols[1];

        gen_statements(statements);

        space_overhead += statements->scope_local_space;

    }

    write_code(string("ADD SP, ") + to_string(space_overhead));
    current_function_local_space -= space_overhead;

    ST2->exit_scope();

}

void gen_statements(SymbolInfo* curr)
{
    preprocess(curr);
    
    if(curr->name == "statements statement"){

        auto statements = curr->child_symbols[0];
        auto statement = curr->child_symbols[1];

        gen_statements(statements);

        int new_label_id = get_new_label();
        statement->next_label_id = new_label_id;

        gen_statement(curr->child_symbols[1]);

        write_label(new_label_id);

    }
    else if(curr->name == "statement"){

        auto statement = curr->child_symbols[0];

        int new_label_id = get_new_label();
        statement->next_label_id = new_label_id;

        gen_statement(statement);

        write_label(new_label_id);

    }
}

void gen_statement(SymbolInfo* curr)
{
    preprocess(curr);
    
    if(curr->name == "var_declaration"){
        gen_var_declaration(curr->child_symbols[0]);
    } 
    else if(curr->name == "expression_statement"){
        gen_expression_statement(curr->child_symbols[0]);
    } 
    else if(curr->name == "compound_statement"){
        gen_compound_statement(curr->child_symbols[0]);
    }
    else if(curr->name == "FOR LPAREN expression_statement expression_statement expression RPAREN statement"){

        auto expression_statement_1 = curr->child_symbols[2];
        auto expression_statement_2 = curr->child_symbols[3];

        auto expression = curr->child_symbols[4];

        auto statement = curr->child_symbols[6]; 

        int new_label_id_1 = get_new_label();
        int new_label_id_2 = get_new_label();
        int new_label_id_3 = get_new_label();
        int new_label_id_4 = get_new_label();

        gen_expression_statement(expression_statement_1);

        write_label(new_label_id_1);

        expression_statement_2->true_label_id = new_label_id_3;
        expression_statement_2->false_label_id = curr->next_label_id;

        // condition check
        gen_expression_statement(expression_statement_2);

        write_label(new_label_id_2);

        gen_expression(expression);
        write_code("POP AX");       // something will be pushed, as no boolean labels are being set

        write_code(string("JMP L") + to_string(new_label_id_1));

        write_label(new_label_id_3);

        statement->next_label_id = new_label_id_4;

        gen_statement(statement);

        write_label(new_label_id_4);

        write_code(string("JMP L") + to_string(new_label_id_2));

    }
    else if(curr->name == "IF LPAREN expression RPAREN statement"){

        auto expression = curr->child_symbols[2];
        auto statement = curr->child_symbols[4];

        int new_label_id = get_new_label();

        expression->true_label_id = new_label_id;
        expression->false_label_id = curr->next_label_id;

        gen_expression(expression);

        write_label(new_label_id);

        statement->next_label_id = curr->next_label_id;

        gen_statement(statement);

    }
    else if(curr->name == "IF LPAREN expression RPAREN statement ELSE statement"){

        auto expression = curr->child_symbols[2];

        auto statement_1 = curr->child_symbols[4];
        auto statement_2 = curr->child_symbols[6];

        int new_label_id_1 = get_new_label();
        int new_label_id_2 = get_new_label();

        expression->true_label_id = new_label_id_1;
        expression->false_label_id = new_label_id_2;

        gen_expression(expression);

        write_label(new_label_id_1);

        statement_1->next_label_id = curr->next_label_id;

        gen_statement(statement_1);

        write_code( string("JMP L") + to_string(curr->next_label_id) );

        write_label(new_label_id_2);

        statement_2->next_label_id = curr->next_label_id;

        gen_statement(statement_2);

    }
    else if(curr->name == "WHILE LPAREN expression RPAREN statement"){

        auto expression = curr->child_symbols[2];
        auto statement = curr->child_symbols[4];

        int new_label_id_1 = get_new_label();
        int new_label_id_2 = get_new_label();

        write_label(new_label_id_1);

        expression->true_label_id = new_label_id_2;
        expression->false_label_id = curr->next_label_id;

        gen_expression(expression);

        write_label(new_label_id_2);

        statement->next_label_id = new_label_id_1;

        gen_statement(statement);

        write_code( string("JMP L") + to_string(new_label_id_1) );

    }
    else if(curr->name == "PRINTLN LPAREN ID RPAREN SEMICOLON"){

        auto id = curr->child_symbols[2];

        auto symbol = ST2->lookup(id->name);

        if(symbol->check_property(IS_LOCAL_VARIABLE)){
            write_code(string("MOV AX, ")+get_local_variable_address(symbol->name));
        }
        else{
            write_code(string("MOV AX, ")+symbol->name);
        }

        // calling the print function
        write_code("CALL print_output\nCALL new_line\n");

    } 
    else if(curr->name == "RETURN expression SEMICOLON"){

        auto expression = curr->child_symbols[1];

        gen_expression(expression);

        write_code("POP AX"); 
        
        // returning...
        write_code(string("ADD SP, ") + to_string(current_function_local_space));
        write_code(string("JMP L") + to_string(current_function_return_label_id));

    }
}

void gen_var_declaration(SymbolInfo* curr)
{
    preprocess(curr);
    
    if(is_current_scope_global){
        
    } 
    else {    // local variables, pushing to stack

        if(curr->name == "type_specifier declaration_list SEMICOLON"){

            for(auto child: curr->child_symbols[1]->get_child_identifiers())
			{   
                child->set_property(IS_LOCAL_VARIABLE);
                if(child->check_property(IS_ARRAY)){

                    int space_required = 2*child->get_arr_size();

                    write_code(string("SUB SP, ") + to_string(space_required) + newline);
                    
                    // for array arr it means the position at stack of arr[0]
                    child->stack_starting_position = -(current_function_local_space + space_required);
                    
                    current_function_local_space += space_required;
                    current_scope_local_space += space_required;

                } 
                else{

                    current_function_local_space += 2;
                    current_scope_local_space += 2;
                    
                    write_code("SUB SP, 2\n");
                    
                    child->stack_starting_position = -current_function_local_space;
                
                }
                
                ST2->insert_into_current_scope(child->name, child->type);
                
                copy_type_and_properties(ST2->lookup(child->name), child);
                
                idebug(child->name + string(" -> ") + to_string(child->stack_starting_position));
            
            }
        }
    }
}

void gen_expression_statement(SymbolInfo* curr){

    preprocess(curr);
    
    if(curr->name == "expression SEMICOLON"){

        auto expression = curr->child_symbols[0];

        copy_boolean_labels(expression, curr);

        gen_expression(expression);

        if(curr->true_label_id < 0){    // this means something was pushed after gen_expression
            write_code("POP AX");
        }

    }

}

void gen_expression(SymbolInfo* curr){

    preprocess(curr);
    
    if(curr->name == "logic_expression"){

        auto logic_expression = curr->child_symbols[0];

        copy_boolean_labels(logic_expression, curr);

        gen_logic_expression(logic_expression);


    } 
    else if(curr->name == "variable ASSIGNOP logic_expression"){
        
        auto logic_expression = curr->child_symbols[2];

        // not copying boolean labels

        // first prepare the value of logic_expression at stack for assigning
        gen_logic_expression(logic_expression);

        auto variable = curr->child_symbols[0];
        string indicator = get_variable_indicator(variable);

        write_code("POP AX\n");         // fetch the expression value at AX
        write_code( string("MOV ") + indicator+string(", AX") );

        write_code("PUSH AX\n");

        if( curr->true_label_id > 0 ){
            gen_boolean_evaluation(curr);
        }

    }
}

void gen_logic_expression(SymbolInfo* curr){

    preprocess(curr);
    
    int action_marker = 0;

    // if this is actually a top-level logic or rel expression,
    // but the true and false labels are not yet defined,
    // create labels for pushing 0/1 into the stack
    if( curr->true_label_id < 0 && check_logic_rel_expression(curr) ){

        int new_true_label_id = get_new_label();
        int new_false_label_id = get_new_label();

        curr->true_label_id = new_true_label_id;
        curr->false_label_id = new_false_label_id;

        action_marker = 1;

    }

    if(curr->name == "rel_expression"){

        auto rel_expression = curr->child_symbols[0];

        copy_boolean_labels(rel_expression, curr);

        gen_rel_expression(rel_expression);

    }
    else if(curr->name == "rel_expression LOGICOP rel_expression"){

        int new_label_id = get_new_label();

        auto rel_expression_1 = curr->child_symbols[0];
        auto rel_expression_2 = curr->child_symbols[2];

        auto logicop = curr->child_symbols[1];

        if(logicop->name == "&&"){

            // short circuiting
            rel_expression_1->false_label_id = curr->false_label_id;
            rel_expression_1->true_label_id = new_label_id;

            copy_boolean_labels(rel_expression_2, curr);

        }
        else if(logicop->name == "||"){

            // short circuiting
            rel_expression_1->true_label_id = curr->true_label_id;
            rel_expression_1->false_label_id = new_label_id;

            copy_boolean_labels(rel_expression_2, curr);

        }

        gen_rel_expression(rel_expression_1);
        write_label(new_label_id);
        gen_rel_expression(rel_expression_2);

    }

    if( action_marker == 1 ){

        write_label(curr->true_label_id);
        write_code("MOV AX, 1");

        int push_label = get_new_label();
        write_code(string("JMP L")+to_string(push_label));

        write_label(curr->false_label_id);
        write_code("MOV AX, 0");

        write_label(push_label);
        write_code("PUSH AX");

    }
}

void gen_rel_expression(SymbolInfo* curr){

    preprocess(curr);
    
    if(curr->name == "simple_expression"){

        auto simple_expression = curr->child_symbols[0];
        
        copy_boolean_labels(simple_expression, curr);

        gen_simple_expression(simple_expression);

    } 
    else if(curr->name == "simple_expression RELOP simple_expression"){

        auto simple_expression_1 = curr->child_symbols[0];
        auto simple_expression_2 = curr->child_symbols[2];

        auto relop = curr->child_symbols[1];

        // don't copy boolean labels

        gen_simple_expression(simple_expression_1);
        gen_simple_expression(simple_expression_2);

        string asm_op = op_to_assembly[relop->name];

        write_code("POP AX\n");
        write_code("POP CX\n");
        write_code("CMP CX, AX\n");
        write_code( asm_op + string(" L") + to_string(curr->true_label_id));
        write_code(string("JMP L")+to_string(curr->false_label_id));

    } 
}

// expected -> fetches the determined value to AX
void gen_simple_expression(SymbolInfo* curr){

    preprocess(curr);
        
    if(curr->name == "term"){

        auto term = curr->child_symbols[0];

        copy_boolean_labels(term, curr);

        gen_term(term);

    } 
    else if(curr->name == "simple_expression ADDOP term"){

        auto simple_expression = curr->child_symbols[0];
        auto term = curr->child_symbols[2];

        auto addop = curr->child_symbols[1];

        // don't copy boolean labels

        gen_simple_expression(simple_expression);
        gen_term(term);

        write_code("POP AX\n");
        write_code("POP CX\n");

        if(addop->name == "+"){
            write_code("ADD CX, AX\n");
        } 
        else{
            write_code("SUB CX, AX\n");
        }

        write_code("PUSH CX\n");

        if( curr->true_label_id > 0 ){
            gen_boolean_evaluation(curr);
        }

    }
}

void gen_term(SymbolInfo* curr){

    preprocess(curr);
    
    if(curr->name == "unary_expression"){

        auto unary_expression = curr->child_symbols[0];

        copy_boolean_labels(unary_expression, curr);

        gen_unary_expression(unary_expression);

    } 
    else if(curr->name == "term MULOP unary_expression"){
        
        auto term = curr->child_symbols[0];
        auto unary_expression = curr->child_symbols[2];

        auto mulop = curr->child_symbols[1];

        // don't copy boolean labels

        gen_term(term);
        gen_unary_expression(unary_expression);

        write_code("POP CX");
        write_code("POP AX");

        // Converts word to double-word, does sign extension for DX:AX
        write_code("CWD");

        if(mulop->name == "*"){
            write_code("MUL CX");
        }
        else {       // divide or modulus                   
            write_code("IDIV CX"); 
        }

        if(mulop->name == "%"){
            write_code("PUSH DX");
        }else{
            write_code("PUSH AX");
        }

        if( curr->true_label_id > 0 ){
            gen_boolean_evaluation(curr);
        }

    }

}

void gen_unary_expression(SymbolInfo* curr){

    preprocess(curr);
    
    if(curr->name == "ADDOP unary_expression"){

        auto unary_expression = curr->child_symbols[1];
        auto addop = curr->child_symbols[0];

        // don't copy boolean labels

        gen_unary_expression(unary_expression);

        write_code("POP AX");

        if(addop->name == "-"){
            write_code("NEG AX");
        }

        write_code("PUSH AX");

        if( curr->true_label_id > 0 ){
            gen_boolean_evaluation(curr);
        }

    }
    else if(curr->name == "NOT unary_expression"){

        int action_marker = 0;

        auto unary_expression = curr->child_symbols[1];

        // if this is actually a top-level not expression,
        // but the true and false labels are not yet defined,
        // create labels for pushing 0/1 into the stack
        if( curr->true_label_id < 0 ){

            int new_true_label_id = get_new_label();
            int new_false_label_id = get_new_label();

            curr->true_label_id = new_true_label_id;
            curr->false_label_id = new_false_label_id;

            action_marker = 1;

        }

        unary_expression->false_label_id = curr->true_label_id;
        unary_expression->true_label_id = curr->false_label_id;

        gen_unary_expression(unary_expression);

        if( action_marker == 1 ){

            write_label(curr->true_label_id);
            write_code("MOV AX, 1");

            int push_label = get_new_label();
            write_code(string("JMP L")+to_string(push_label));

            write_label(curr->false_label_id);
            write_code("MOV AX, 0");

            write_label(push_label);
            write_code("PUSH AX");

        }

    }
    else if(curr->name == "factor"){

        auto factor = curr->child_symbols[0];

        copy_boolean_labels(factor, curr);

        gen_factor(factor);

    }

}

void gen_factor(SymbolInfo* curr){

    preprocess(curr);
    
    if(curr->name == "CONST_INT"){

        auto constant_integer = curr->child_symbols[0];

        write_code(string("MOV AX, ") + constant_integer->name);
        write_code("PUSH AX");

        if( curr->true_label_id > 0 ){
            gen_boolean_evaluation(curr);
        }

    } 
    else if(curr->name == "ID LPAREN argument_list RPAREN"){  // function call
        
        auto function = curr->child_symbols[0];
        auto argument_list = curr->child_symbols[2];

        gen_argument_list(argument_list);

        write_code(string("CALL ") + function->name );
        write_code("PUSH AX");

        if( curr->true_label_id > 0 ){
            gen_boolean_evaluation(curr);
        }

    } 
    else if(curr->name == "variable") {

        auto variable = curr->child_symbols[0];

        copy_boolean_labels(variable, curr);

        gen_variable(variable);

    } 
    else if(curr->name == "LPAREN expression RPAREN"){

        auto expression = curr->child_symbols[1];

        // only here, labels must be copied, because, labels will be used in the expression next
        copy_boolean_labels(expression, curr);

        gen_expression(expression);

    }
    else if(curr->name == "variable INCOP"){

        auto variable = curr->child_symbols[0];

        string indicator = get_variable_indicator(variable);

        write_code(string("MOV AX, ")+indicator);
        write_code("PUSH AX");         // as it is postfix operator, pushing beforehand
        write_code("INC AX");
        write_code(string("MOV ")+indicator+string(", AX"));

        if( curr->true_label_id > 0 ){
            gen_boolean_evaluation(curr);
        }
    }
    else if(curr->name == "variable DECOP"){

        auto variable = curr->child_symbols[0];

        string indicator = get_variable_indicator(variable);

        write_code(string("MOV AX, ")+indicator);
        write_code("PUSH AX");         // as it is postfix operator, pushing beforehand
        write_code("DEC AX");
        write_code(string("MOV ")+indicator+string(", AX"));

        if( curr->true_label_id > 0 ){
            gen_boolean_evaluation(curr);
        }
    }
}

void gen_variable(SymbolInfo* curr){

    preprocess(curr);
    
    // this function should push the value of the variable to stack
    string indicator = get_variable_indicator(curr);
    write_code(string("MOV AX, ")+indicator);
    write_code("PUSH AX\n");

    if( curr->true_label_id > 0 ){
        gen_boolean_evaluation(curr);
    }

    // if(curr->name == "ID"){     // non-array variable
    //     write_code(string("MOV AX, ")+get_local_variable_address(curr->child_symbols[0]->name));
    //     write_code("PUSH AX\n");
    // }
}

void gen_argument_list(SymbolInfo* curr){

    preprocess(curr);
    
    if(curr->name == "arguments"){

        auto arguments = curr->child_symbols[0];

        gen_arguments(arguments);

    }
    else if(curr->name == " "){



    }

}

void gen_arguments(SymbolInfo* curr){
    
    preprocess(curr);
    
    if(curr->name == "arguments COMMA logic_expression"){

        auto arguments = curr->child_symbols[0];
        auto logic_expression = curr->child_symbols[2];

        gen_arguments(arguments);
        gen_logic_expression(logic_expression);

        // already pushed in gen_logic_expression(), no need to push again
        // write_code("PUSH AX\n");    // so that it can be used by the callee as an argument
    } 
    else if(curr->name == "logic_expression"){  // function call

        auto logic_expression = curr->child_symbols[0];

        gen_logic_expression(logic_expression);

        // already pushed in gen_logic_expression(), no need to push again
        //write_code("PUSH AX\n");    // so that it can be used by the callee as an argument
    } 
}

void write_global_variables(){

    idebug("at write_global_variables");

    // fetch globals from the only non-destroyed scopetable from parser
    for(auto entry: ST->get_current_scopetable()->get_all_entries()){
        // only handling int as per specification
        // i DW 1 DUP (0000H)
        if(entry->type == "INT" || entry->type == "ARRAY"){
            write_code(entry->name + string(" DW ") + to_string(entry->get_arr_size()) + string(" DUP (0000H)"));
            // inserting to new ST so that it can later be found
            ST2->insert_into_current_scope(entry->name, entry->type);
        }
    }
}

void run_icg(){
    write_header(".MODEL SMALL\n.STACK 1000H\n.Data\n");
    // a global variable 'number' needed for print library 
	write_code("number DB \"00000$\"");
    write_global_variables();
    write_header(".CODE\n");
	gen_start(root);
    write_print_library();
    write_footer("\nEND main\n");
}

int main(int argc,char *argv[])
{
    run_parser(argc, argv);
    //run_icg();
    if(total_error_count > 0){
        cout<<"\n\nSorry! The input file contains error. Cannot generate Intermediate Code\n\n"<<endl;
    }
    else{
        run_icg();
        cout<<"\nIntermediate code written to code.asm file"<<endl;
    }
}