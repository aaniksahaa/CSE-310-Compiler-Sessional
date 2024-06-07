#include<bits/stdc++.h>
#include "headers/2005001_SymbolTable.hpp"
#include "headers/2005001_SymbolInfo.hpp"

using namespace std;

extern SymbolInfo* root;
extern void copy_type_and_properties(SymbolInfo* destination, SymbolInfo* source);
extern void run_parser(int argc,char *argv[]);

// global variables

// carefully handle these
bool is_current_scope_global;
int current_function_local_space; // space occupied by local variables of current function


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
void gen_argument_list(SymbolInfo* curr);
void gen_arguments(SymbolInfo* curr);

// helpers 

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
    for (const auto& line : lines) {
        result += "\t" + line + "\n";
    }

    return result;
}

string get_local_variable_address(string name){
    int position = ST2->lookup(name)->stack_starting_position;
    string result = string("[BP")+get_signed_string(position)+string("]");
    return result;
}

// writers 

void write_header(string s){
	codeout<<s;
}

void write_footer(string s){
	codeout<<s;
}

void write_label(string s){
	codeout<<s;
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

void idebug(string s){
    icg_debug_out<<s<<endl;
}

void gen_start(SymbolInfo* curr)
{
    is_current_scope_global = 1;
    gen_program(curr->child_symbols[0]);
}

void gen_program(SymbolInfo* curr)
{
    if(curr->name == "program unit"){
        gen_program(curr->child_symbols[0]);
        gen_unit(curr->child_symbols[1]);
    } else if(curr->name == "unit") {
        gen_unit(curr->child_symbols[0]);
    }
}

void gen_unit(SymbolInfo* curr)
{
    if(curr->name == "func_definition"){
        gen_func_definition(curr->child_symbols[0]);
    }
}

void gen_func_definition(SymbolInfo* curr)
{
    is_current_scope_global = 0;
    current_function_local_space = 0;
    int parameter_count = 0;


    // common upper part
    auto f = curr->child_symbols[1];
    auto children = curr->child_symbols;
    write_header(f->name + " PROC\n");
    if(f->name == "main"){
        write_code("MOV AX, @DATA\nMOV DS, AX\n");
    }
    write_code("PUSH BP\nMOV BP, SP\n");


    if(curr->name == "type_specifier ID LPAREN RPAREN compound_statement"){
        gen_compound_statement(children[4]);
        // add sp, ret
        
    } else if(curr->name == "type_specifier ID LPAREN parameter_list RPAREN compound_statement") {
        idebug("here");
        auto temp_v = children[3]->get_child_identifiers();
        parameter_count = temp_v.size();
        reverse(temp_v.begin(), temp_v.end());
        int current_offset = 4; // will proceed like 4,6,8... in reverse
        for(auto child: temp_v){
            //stack_position_map.insert(make_pair(child->name, current_offset));
            ST2->insert_into_current_scope(child->name, child->type);
            ST2->lookup(child->name)->stack_starting_position = current_offset;
            current_offset += 2;
        }
        gen_compound_statement(children[5]);
    }


    // common lower part
    write_code(string("ADD SP, ")+to_string(current_function_local_space)+newline);
    write_code("POP BP\n");
    if(f->name != "main"){
        string return_statement = string("RET ")+(parameter_count    ?   to_string(2*parameter_count)    :   string(""));
        write_code(return_statement);
    }

    if(f->name == "main"){
        write_code("MOV AX,4CH\nINT 21H\n");
    }
    write_footer(f->name + " ENDP\n");
    is_current_scope_global = 1;
    current_function_local_space = 0;
}

void gen_compound_statement(SymbolInfo* curr)
{
    ST2->enter_scope();
    if(curr->name == "LCURL statements RCURL"){
        gen_statements(curr->child_symbols[1]);
    }
    ST2->exit_scope();
}

void gen_statements(SymbolInfo* curr)
{
    if(curr->name == "statements statement"){
        gen_statements(curr->child_symbols[0]);
        gen_statement(curr->child_symbols[1]);
    }
    else if(curr->name == "statement"){
        gen_statement(curr->child_symbols[0]);
    }
}

void gen_statement(SymbolInfo* curr)
{
    if(curr->name == "var_declaration"){
        gen_var_declaration(curr->child_symbols[0]);
    } else if(curr->name == "expression_statement"){
        gen_expression_statement(curr->child_symbols[0]);
    }
    else if(curr->name == "PRINTLN LPAREN ID RPAREN SEMICOLON"){
        int position = ST2->lookup(curr->child_symbols[2]->name)->stack_starting_position;
        write_code(string("MOV AX, ")+get_local_variable_address(curr->child_symbols[2]->name)+newline);
        write_code("CALL print_output\nCALL new_line\n");
    } else if(curr->name == "RETURN expression SEMICOLON"){
        gen_expression(curr->child_symbols[1]);
        // for now, assuming that return will always be at last
        // TODO: add a label at end where this will happen always and 
        // redirect all returns to this label
    }
}

void gen_var_declaration(SymbolInfo* curr)
{
    if(is_current_scope_global){

    } else {    // local variables, pushing to stack
        if(curr->name == "type_specifier declaration_list SEMICOLON"){
            for(auto child: curr->child_symbols[1]->get_child_identifiers())
			{   
                child->set_property(IS_LOCAL_VARIABLE);
                if(child->check_property(IS_ARRAY)){
                    int space_required = 2*child->get_arr_size();
                    write_code(string("SUB SP, ") + to_string(space_required) + newline);
                    // for array arr it means the position at stack of arr[0]
                    child->stack_starting_position = -(current_function_local_space + 2);
                    current_function_local_space += space_required;
                } else{
                    current_function_local_space += 2;
                    write_code("SUB SP, 2\n");
                    child->stack_starting_position = -current_function_local_space;
                }
                //stack_position_map.insert(make_pair(child->name, -child->stack_starting_position));
                ST2->insert_into_current_scope(child->name, child->type);
                copy_type_and_properties(ST2->lookup(child->name), child);
            }
        }
    }
}

void gen_expression_statement(SymbolInfo* curr){
    if(curr->name == "expression SEMICOLON"){
        gen_expression(curr->child_symbols[0]);
        write_code("POP AX\n");
    }
}

void gen_expression(SymbolInfo* curr){
    if(curr->name == "logic_expression"){
        gen_logic_expression(curr->child_symbols[0]);
    } else if(curr->name == "variable ASSIGNOP logic_expression"){

        // first prepare the value of logic_expression in AX for assigning
        gen_logic_expression(curr->child_symbols[2]);

        auto variable = curr->child_symbols[0];

        if(variable->name == "ID"){     // non-array variable
            write_code(string("MOV ")+get_local_variable_address(variable->child_symbols[0]->name)+string(", AX"));
            write_code("PUSH AX\n");
        }
    }
}

void gen_logic_expression(SymbolInfo* curr){
    if(curr->name == "rel_expression"){
        gen_rel_expression(curr->child_symbols[0]);
    }
}

void gen_rel_expression(SymbolInfo* curr){
    if(curr->name == "simple_expression"){
        gen_simple_expression(curr->child_symbols[0]);
    }
}

void gen_simple_expression(SymbolInfo* curr){
    if(curr->name == "term"){
        gen_term(curr->child_symbols[0]);
    }
}

void gen_term(SymbolInfo* curr){
    if(curr->name == "unary_expression"){
        gen_unary_expression(curr->child_symbols[0]);
    }
}

void gen_unary_expression(SymbolInfo* curr){
    if(curr->name == "factor"){
        gen_factor(curr->child_symbols[0]);
    }
}

void gen_factor(SymbolInfo* curr){
    if(curr->name == "CONST_INT"){
        write_code(string("MOV AX, ")+curr->child_symbols[0]->name+newline);
    } else if(curr->name == "ID LPAREN argument_list RPAREN"){  // function call
        //idebug("at call rule");
        gen_argument_list(curr->child_symbols[2]);
        write_code(string("CALL ")+curr->child_symbols[0]->name+newline);
    } else if(curr->name == "variable") {
        auto variable = curr->child_symbols[0];

        if(variable->name == "ID"){     // non-array variable
            write_code(string("MOV AX, ")+get_local_variable_address(variable->child_symbols[0]->name));
        }
    }
}

void gen_argument_list(SymbolInfo* curr){
    if(curr->name == "arguments"){
        gen_arguments(curr->child_symbols[0]);
    }
}

void gen_arguments(SymbolInfo* curr){
    idebug("at arguments");
    if(curr->name == "arguments COMMA logic_expression"){
        idebug("at arguments 1");
        gen_arguments(curr->child_symbols[0]);
        gen_logic_expression(curr->child_symbols[2]);
        write_code("PUSH AX\n");    // so that it can be used by the callee as an argument
    } else if(curr->name == "logic_expression"){  // function call
        idebug("at arguments 2");
        gen_logic_expression(curr->child_symbols[0]);
        write_code("PUSH AX\n");    // so that it can be used by the callee as an argument
    } 
}

void run_icg(){
    write_header(".MODEL SMALL\n.STACK 1000H\n.Data\n");
	write_code("number DB \"00000$\"");
    write_header(".CODE\n");
	gen_start(root);
    write_print_library();
    write_footer("\nEND main\n");
}

int main(int argc,char *argv[])
{
    run_parser(argc, argv);
    run_icg();
}