%{

/***

Compiler for a subset of C

Specifications:

1. Only postfix operator
2. Only supported types - INT, FLOAT, VOID
3. No preprocessor directives, No switch-case
4. consecutive logical operators or consecutive relational operators not supported
5. Empty argument list not supported as per given grammar
6. COMMA cant be used a delimiter in expression statements, so for(i=0,j=0;;) is invalid

***/

#include<bits/stdc++.h>
#include "headers/2005001_SymbolTable.hpp"
#include "headers/2005001_SymbolInfo.hpp"

using namespace std;


#define TYPE_INT string("INT")
#define TYPE_FLOAT string("FLOAT")
#define TYPE_VOID string("VOID")
#define TYPE_ERROR string("ERROR")


int yyparse(void); 
int yylex(void);

extern FILE* yyin;
FILE *fp;

extern SymbolTable* ST;

extern int line_count;
int total_error_count = 0;

extern int yylineno;

string prefix = "2005001_";

ofstream logout(prefix+"log.txt");
ofstream treeout(prefix+"parsetree.txt");
ofstream errorout(prefix+"error.txt");
ofstream debugout(prefix+"debug.txt");

class Writer
{
    ofstream& out;
public:
    Writer(ofstream& outFile) : out(outFile)
    {

    }
    void write_grammar_rule(SymbolInfo* info)
    {
        out<<info->get_type()<<"\t: "<<info->get_name()<<" "<<endl;
    }
    void write_grammar_rule(string rule) // overridden for directly pasting as per testcase
    {
        out<<rule<<endl;
    }
	void write_grammar_rule_default_style(string left, string right)
    {
        out<<left<<" : "<<right<<" "<<endl;
    }
	void write_parse_tree(TreeNode* root)
	{
		ParseTree* tree= new ParseTree(root);
		out<<tree->print_tree();
	}
	void write_error(int line_number, string error_text)
	{
		total_error_count++;
		out<<"Line# "<<line_number<<": "<<error_text<<endl;
	}
	void debug(string debug_text)
	{
		out<<line_count<<" "<<debug_text<<endl;
	}
	void debug_subtree(SymbolInfo* info)
	{
		debug(info->get_parse_tree_node()->print_subtree(5));
		out<<endl;
	}
	void write_log_footer()
	{
		out<<"Total Lines: "<<line_count<<endl;
		out<<"Total Errors: "<<total_error_count<<endl;
	}
	void write_log_syntax_error()
	{
		out<<"Error at line no "<<line_count<<" : syntax error"<<endl;
	}
	void write_error_syntax_error(string at_where, string of_what, int line_number)
	{
		total_error_count++;
		out<<"Line# "<<line_number<<": Syntax error at "<<at_where<<" of "<<of_what<<endl;
	}
};

Writer log_writer(logout);
Writer tree_writer(treeout);
Writer error_writer(errorout);
Writer debug_writer(debugout);

void debug(string debug_text)
{
	debug_writer.debug(debug_text);
}
void debug_subtree(SymbolInfo* info)
{
	debug_writer.debug_subtree(info);
}

void report_syntax_error(string at_where, string of_what, int line_number=line_count)
{
	error_writer.write_error_syntax_error(at_where, of_what, line_number);
}


void yyerror(char *s)
{
	logout<<"syntax error\n";
}

// helper global variables

string quote = "'";
string space = " ";
string newline = "\n";

// helper functions

void test()
{
	log_writer.write_grammar_rule("hello");
}

string get_lowercase_string(string s)
{
	int len = s.length();
	string ret = string(len, ' ');
	for(int i=0; i<len; i++)
	{
		ret[i] = tolower(s[i]);
	}
	return ret;
}

void debug_child_identifiers(SymbolInfo* symbol, string header="")
{
	for(auto& child: symbol->get_child_identifiers())
	{
		debug_writer.debug(header + space + child->get_name() + space + to_string(child->get_arr_size()));
	}
	debug_writer.debug("\n");
}

void write_ARRAY_to_SymbolInfo(SymbolInfo* info)
{
	if(info->check_property(IS_ARRAY))
	{
		info->set_type("ARRAY");
	}
}

void copy_type_and_properties(SymbolInfo* destination, SymbolInfo* source)
{
	destination->set_evaluated_type(source->get_evaluated_type());
	destination->set_boolean_properties(source->get_boolean_properties());
	destination->set_constant_text(source->get_constant_text());
}

string get_type_after_operation(string type_1, string type_2)
{
	if(type_1 == TYPE_ERROR or type_2 == TYPE_ERROR)
	{
		return TYPE_ERROR;
	}
	else if(type_1 == TYPE_VOID or type_2 == TYPE_VOID)
	{
		return TYPE_ERROR;
	}
	else if(type_1 == TYPE_FLOAT or type_2 == TYPE_FLOAT)
	{
		return TYPE_FLOAT;
	}
	else 
	{
		return TYPE_INT;
	}

}

// Parsing helper global variables

int exit_scope_pending = 0;                  // works like the top pointer of a stack // counts how many scopes have been entered but not exited yet
SymbolInfo* pending_function_insert = NULL;

SymbolInfo* current_function_definition_parameter_list = NULL;

// Parsing helper functions

vector<string> process_parameters(SymbolInfo* p_list)
{
	vector<string>valid_parameter_types;

	SymbolTable* dummy_ST = new SymbolTable(11);  //  will be auto freed when this function is exited

	dummy_ST->enter_scope();   // this is done for checking validity of parameters

	for(auto child: p_list->get_child_identifiers())
	{
		int line_number = child->get_start_line_count();
		string name = child->get_name();
		string type = child->get_evaluated_type();

		// following C, storing all types in the function for now
		// However, it behaves differently in C++
		valid_parameter_types.push_back(type);

		if(type == "VOID")
		{
			error_writer.write_error(line_number, "function parameter cannot be void");
		}
		if(child->get_type() == "type_specifier")  // unnamed parameter
		{
			//valid_parameter_types.push_back(type);
		}
		else 
		{
			bool success = dummy_ST->insert_into_current_scope(name, type);
			if(success)
			{
				//valid_parameter_types.push_back(type);
			}
			else 
			{
				auto existing_parameter = dummy_ST->lookup(name);
				if(existing_parameter->get_type() == type)
				{
					error_writer.write_error(line_number, "Redefinition of parameter " + quote + name + quote);
				}
				else 
				{
					error_writer.write_error(line_number, "Conflicting types for " + quote + name + quote);
				}
			}
		}
	}

	dummy_ST->exit_scope(); 

	return valid_parameter_types;
}

bool check_function_mismatch(SymbolInfo* existing_function, string return_type, vector<string>parameter_types)
{
	int mismatch = 0;
	if(existing_function->get_evaluated_type() != return_type || existing_function->get_parameter_count() != parameter_types.size())
	{
		mismatch = 1;
	}
	else 
	{
		int len = parameter_types.size();
		auto existing_parameters = existing_function->get_parameter_types();
		for(int i=0; i<len; i++)
		{
			if(parameter_types[i] != existing_parameters[i])
			{
				mismatch = 1;
				break;
			}
		}
	}
	return mismatch;
}

bool handle_function_declaration(int line_number, string function_name, string return_type, vector<string>parameter_types)
{
	bool error_occured = 0;
	bool success = ST->insert_into_current_scope(function_name, string("FUNCTION,")+return_type);
	if(success)
	{
		debug_writer.debug("inserted");
		auto inserted_function = ST->lookup(function_name);
		inserted_function->set_property(IS_FUNCTION);
		inserted_function->set_property(IS_DECLARED_FUNCTION);
		for(auto type: parameter_types)
		{
			inserted_function->add_parameter_type(type);
		}
		inserted_function->set_evaluated_type(return_type);
	}
	else
	{
		auto existing_symbol = ST->lookup(function_name);
		if(existing_symbol->check_property(IS_FUNCTION))
		{
			// check if functions' attributes completely match
			int mismatch = check_function_mismatch(existing_symbol, return_type, parameter_types);
			if(mismatch)
			{
				error_occured = 1;
				error_writer.write_error(line_number, "Conflicting types for "+quote+function_name+quote);
			}
			else  // so exactly same function
			{
				// no error, because, declaring same function again is permitted
			}
		}
		else 
		{
			error_occured = 1;
			error_writer.write_error(line_number, quote+function_name+quote+" redeclared as different kind of symbol" );
		}
	}
	return !error_occured;
}

bool handle_function_definition(int line_number, string function_name, string return_type, vector<string>parameter_types)
{
	int error_occured = 0;
	//bool success = ST->insert_into_current_scope(function_name, string("FUNCTION,")+return_type);
	bool success = (ST->lookup(function_name) == NULL);  // As we are in the global scope, lookup is valid and sufficient
	if(success)
	{
		// debug_writer.debug("inserted");
		// auto inserted_function = ST->lookup(function_name);
		// inserted_function->set_property(IS_FUNCTION);
		// inserted_function->set_property(IS_DEFINED_FUNCTION);
		// for(auto type: parameter_types)
		// {
		// 	inserted_function->add_parameter_type(type);
		// }
		// inserted_function->set_evaluated_type(return_type);


		// Pending the insert for later


		pending_function_insert = new SymbolInfo(function_name, return_type);
		for(auto type: parameter_types)
		{
			pending_function_insert->add_parameter_type(type);
		}
		pending_function_insert->set_evaluated_type(return_type);


	}
	else
	{
		auto existing_symbol = ST->lookup(function_name);
		if(existing_symbol->check_property(IS_FUNCTION))
		{
			// check if functions' attributes completely match
			int mismatch = check_function_mismatch(existing_symbol, return_type, parameter_types);
			if(mismatch)
			{
				error_occured = 1;
				error_writer.write_error(line_number, "Conflicting types for "+quote+function_name+quote);
			}
			else  // so exactly same function
			{
				// now, no error it is only declared before, but error if it is defined before
				if(existing_symbol->check_property(IS_DEFINED_FUNCTION))
				{
					error_occured = 1;
					error_writer.write_error(line_number, "Redefinition of "+quote+function_name+quote);
				}
				else 
				{
					// no error
				}
			}
		}
		else 
		{
			error_occured = 1;
			error_writer.write_error(line_number, quote+function_name+quote+" redeclared as different kind of symbol" );
		}
	}

	return !error_occured;
}

void insert_pending_function()
{
	if(pending_function_insert != NULL)
	{
		string function_name = pending_function_insert->get_name();
		string return_type = pending_function_insert->get_evaluated_type();
		
		ST->insert_into_current_scope(function_name, string("FUNCTION,") + return_type);

		debug_writer.debug("inserted");

		auto inserted_function = ST->lookup(function_name);
		inserted_function->set_property(IS_FUNCTION);
		inserted_function->set_property(IS_DEFINED_FUNCTION);
		for(auto type: pending_function_insert->get_parameter_types())
		{
			inserted_function->add_parameter_type(type);
		}
		inserted_function->set_evaluated_type(return_type);
		delete pending_function_insert;
		pending_function_insert = NULL;
	}
}


%}

%union {
	SymbolInfo* info;
}

%token <info> IF FLOAT FOR INT VOID ELSE WHILE RETURN PRINTLN // info optional
%token <info> ADDOP MULOP 
%token <info> INCOP DECOP // info optional
%token <info> RELOP 
%token <info> ASSIGNOP // info optional
%token <info> LOGICOP 
%token <info> BITOP 
%token <info> NOT LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON // info optional
%token <info> CONST_INT CONST_FLOAT ID 
%token <info> CONST_CHAR SINGLE_LINE_STRING MULTI_LINE_STRING

%type <info> argument_list arguments
%type <info> statements statement expression_statement variable expression logic_expression rel_expression simple_expression term unary_expression factor
%type <info> var_declaration type_specifier declaration_list
%type <info> compound_statement
%type <info> func_declaration func_definition parameter_list
%type <info> start program unit 

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

start : program
	{
		log_writer.write_grammar_rule("start : program ");
		$$ = new SymbolInfo("program","start");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
		tree_writer.write_parse_tree($$->get_parse_tree_node());
	}
	;

program : 	program unit 
	{	
		log_writer.write_grammar_rule("program : program unit ");
		$$ = new SymbolInfo("program unit","program");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);
	}
	| unit
	{
		log_writer.write_grammar_rule("program : unit ");
		$$ = new SymbolInfo("unit","program");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
	}
	;
	
unit : 	var_declaration
	{
		log_writer.write_grammar_rule("unit : var_declaration  ");
		$$ = new SymbolInfo("var_declaration","unit");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
	}
	| func_declaration
	{
		log_writer.write_grammar_rule("unit : func_declaration ");
		$$ = new SymbolInfo("func_declaration","unit");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
	}
	| func_definition
	{
		log_writer.write_grammar_rule("unit : func_definition  ");
		$$ = new SymbolInfo("func_definition","unit");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
	}
	;
     
func_declaration : 	type_specifier ID LPAREN parameter_list RPAREN SEMICOLON
	{
		// so our assumption about parameter list must be discarded now
		// Note that this nullification does not induce any memory leakage
		current_function_definition_parameter_list = NULL; 

		log_writer.write_grammar_rule("func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON ");
		$$ = new SymbolInfo("type_specifier ID LPAREN parameter_list RPAREN SEMICOLON","func_declaration");
		$$->set_line_counts($1->get_start_line_count(), $6->get_end_line_count());
		$$->set_children_tree_nodes(6,$1,$2,$3,$4,$5,$6);

		debug_child_identifiers($4,string("func_declaration"));

		// At first, processing the parameters

		vector<string> parameter_types = process_parameters($4);

		// Now processing the function itself

		string function_name = $2->get_name();
		string return_type = $1->get_name();
		int line_number = $2->get_start_line_count();

		if($4->check_property(SYNTAX_ERROR_OCCURRED))
		{
			report_syntax_error("parameter list", "function declaration");
		}
		else 
		{
			handle_function_declaration(line_number, function_name, return_type, parameter_types);
		}
	}
	| type_specifier ID LPAREN RPAREN SEMICOLON
	{
		log_writer.write_grammar_rule("func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON ");
		$$ = new SymbolInfo("type_specifier ID LPAREN RPAREN SEMICOLON","func_declaration");
		$$->set_line_counts($1->get_start_line_count(), $5->get_end_line_count());
		$$->set_children_tree_nodes(5,$1,$2,$3,$4,$5);

		// At first, processing the parameters

		vector<string> parameter_types;  // empty 

		// Now processing the function itself

		string function_name = $2->get_name();
		string return_type = $1->get_name();
		int line_number = $2->get_start_line_count();

		handle_function_declaration(line_number, function_name, return_type, parameter_types);
	}
	;
		 
func_definition : 	type_specifier ID LPAREN parameter_list RPAREN 
	{
		debugout<<line_count<<" at func_definition\n";
		// At first, processing the parameters

		vector<string> parameter_types = process_parameters($4);

		// Now processing the function itself

		string function_name = $2->get_name();
		string return_type = $1->get_name();
		int line_number = $2->get_start_line_count();

		handle_function_definition(line_number, function_name, return_type, parameter_types);

		if($4->check_property(SYNTAX_ERROR_OCCURRED))
		{
			report_syntax_error("parameter list", "function definition");
		}

	}
	compound_statement
	{
		if( !($4->check_property(SYNTAX_ERROR_OCCURRED)) )
		{
			log_writer.write_grammar_rule("func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement ");
		}
		
		$$ = new SymbolInfo("type_specifier ID LPAREN parameter_list RPAREN compound_statement","func_definition");
		$$->set_line_counts($1->get_start_line_count(), $7->get_end_line_count());
		$$->set_children_tree_nodes(6,$1,$2,$3,$4,$5,$7);

		if($4->check_property(SYNTAX_ERROR_OCCURRED))
		{
			// don't insert the function
		}
		else
		{
			insert_pending_function();
		}
	}
	| type_specifier ID LPAREN RPAREN
	{
		// At first, processing the parameters

		vector<string> parameter_types;   // empty list

		// Now processing the function itself

		string function_name = $2->get_name();
		string return_type = $1->get_name();
		int line_number = $2->get_start_line_count();

		handle_function_definition(line_number, function_name, return_type, parameter_types);

	}
	compound_statement
	{
		log_writer.write_grammar_rule("func_definition : type_specifier ID LPAREN RPAREN compound_statement");
		$$ = new SymbolInfo("type_specifier ID LPAREN RPAREN compound_statement","func_definition");
		$$->set_line_counts($1->get_start_line_count(), $6->get_end_line_count());
		$$->set_children_tree_nodes(5,$1,$2,$3,$4,$6);

		insert_pending_function();
	}
	;				


parameter_list  : 	parameter_list COMMA type_specifier ID
	{

		debugout<<line_count<<" at yacc parameter list\n";

		log_writer.write_grammar_rule("parameter_list  : parameter_list COMMA type_specifier ID");
		$$ = new SymbolInfo("parameter_list COMMA type_specifier ID","parameter_list");
		$$->set_line_counts($1->get_start_line_count(), $4->get_end_line_count());
		$$->set_children_tree_nodes(4,$1,$2,$3,$4);

		$4->set_evaluated_type($3->get_name());

		$$->set_child_identifiers($1->get_child_identifiers());

		if($1->check_property(SYNTAX_ERROR_OCCURRED))
		{
			// if error occurred, add no more
			$$->set_syntax_error();
		}
		else 
		{
			$$->add_child_identifier($4);	
		}

		// assuming these are definition parameters, will be handled later
		current_function_definition_parameter_list = $$;

	}
	| parameter_list COMMA type_specifier
	{
		log_writer.write_grammar_rule("parameter_list  : parameter_list COMMA type_specifier ");
		$$ = new SymbolInfo("parameter_list COMMA type_specifier","parameter_list");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		$$->set_child_identifiers($1->get_child_identifiers());

		if($1->check_property(SYNTAX_ERROR_OCCURRED))
		{
			// if error occurred, add no more
			$$->set_syntax_error();
		}
		else 
		{
			$$->add_child_identifier($3);
		}

	}
	| type_specifier ID
	{

		debugout<<line_count<<" at yacc parameter list\n";

		log_writer.write_grammar_rule("parameter_list  : type_specifier ID");
		$$ = new SymbolInfo("type_specifier ID","parameter_list");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);

		$2->set_evaluated_type($1->get_name());

		$$->add_child_identifier($2);

		// assuming these are definition parameters, will be handled later
		current_function_definition_parameter_list = $$;
	}
	| type_specifier
	{
		log_writer.write_grammar_rule("parameter_list  : type_specifier ");
		$$ = new SymbolInfo("type_specifier","parameter_list");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		// here, the type will be 'type_specifier' and the name will be INT, FLOAT etc
		// this means, this parameter was declared unnamed
		// type can be retrieved from evaluated_type
		// this will be handled while building the SymbolInfo for function
		$$->add_child_identifier($1);  

	}
	;

 		
compound_statement : 	LCURL statements RCURL
	{
		log_writer.write_grammar_rule("compound_statement : LCURL statements RCURL  ");
		$$ = new SymbolInfo("LCURL statements RCURL","compound_statement");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		ST->print_all_scope_tables();

		//debugout<<ST->stringify_all_scope_tables()<<endl;

		if(exit_scope_pending)
		{
			debug("exited scope");
			ST->exit_scope();
			exit_scope_pending--;	// popping off the stack  // one pending cleared
			debugout<<ST->stringify_all_scope_tables();
		}
		// The pending function needs to be inserted only if this compound statement corresponds to a function
		// So, it is not done here, it is done while matching function_definition rule

		// insert_pending_function();  // if any
	}
	| LCURL RCURL
	{
		log_writer.write_grammar_rule("compound_statement : LCURL RCURL  ");
		$$ = new SymbolInfo("LCURL RCURL","compound_statement");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);

		ST->print_all_scope_tables();

		//debugout<<ST->stringify_all_scope_tables()<<endl;

		if(exit_scope_pending)
		{
			debug("exited scope");
			ST->exit_scope();
			exit_scope_pending--;	// popping off the stack  // one pending cleared
			debugout<<ST->stringify_all_scope_tables();
		}
		// The pending function needs to be inserted only if this compound statement corresponds to a function
		// So, it is not done here, it is done while matching function_definition rule

		// insert_pending_function();  // if any
	}
	;
 		    
var_declaration : 	type_specifier declaration_list SEMICOLON
	{
		//debug_child_identifiers($2, "var_declaration");

		if($2->check_property(SYNTAX_ERROR_OCCURRED))
		{
			report_syntax_error("declaration list","variable declaration");
		}
		else 
		{
			log_writer.write_grammar_rule("var_declaration : type_specifier declaration_list SEMICOLON  ");
		}

		$$ = new SymbolInfo("type_specifier declaration_list SEMICOLON","var_declaration");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		string type = $1->get_name();

		if(type == "VOID")
		{
			for(auto child: $2->get_child_identifiers())
			{
				child->set_evaluated_type(type);
				int line_number = child->get_start_line_count();
				error_writer.write_error(line_number, string("Variable or field ") + quote + child->get_name() + quote + string(" declared void"));
			}
		}
		else
		{
			for(auto child: $2->get_child_identifiers())
			{
				child->set_evaluated_type(type);

				int line_number = child->get_start_line_count();
				string name = child->get_name();
				bool success = ST->insert_into_current_scope(name, type);
				if(success)
				{
					auto inserted_symbol = ST->lookup(name);
					inserted_symbol->set_boolean_properties(child->get_boolean_properties());
					inserted_symbol->set_evaluated_type(type);
					inserted_symbol->set_arr_size(child->get_arr_size());
					write_ARRAY_to_SymbolInfo(inserted_symbol);
				}
				else
				{
					auto existing_symbol = ST->lookup(name);
					
					if(existing_symbol->check_property(IS_FUNCTION))
					{
						error_writer.write_error(line_number, quote + name + quote + string(" redeclared as different kind of symbol"));
					}
					else
					{
						auto& e = existing_symbol;
						auto& c = child;
						// debug("Comparing...");
						// debug(e->get_name() + space + e->get_evaluated_type());
						// debug(c->get_name() + space + c->get_evaluated_type());
						if(e->get_evaluated_type()==c->get_evaluated_type() && e->check_property(IS_ARRAY)==c->check_property(IS_ARRAY) && e->arr_size==c->arr_size)
						{
							string arr = (existing_symbol->check_property(IS_ARRAY) ? "[]" : "");
							error_writer.write_error(line_number, string("Redeclaration of ") + quote + get_lowercase_string(type) + space + name + arr + quote);
						}
						else
						{
							error_writer.write_error(line_number, string("Conflicting types for") + quote + name + quote);
						}
					}
					
				}
			}
		}
	}
	|
	type_specifier ID LTHIRD RTHIRD ASSIGNOP LCURL argument_list RCURL SEMICOLON
	{
		log_writer.write_grammar_rule("var_declaration : type_specifier ID LTHIRD RTHIRD ASSIGNOP LCURL argument_list RCURL SEMICOLON");
		$$ = new SymbolInfo("type_specifier ID LTHIRD RTHIRD ASSIGNOP LCURL argument_list RCURL SEMICOLON","var_declaration");
		$$->set_line_counts($1->get_start_line_count(), $9->get_end_line_count());
		$$->set_children_tree_nodes(9,$1,$2,$3,$4,$5,$6,$7,$8,$9);
		string type = $1->get_name();
		if(type == "VOID")
		{
			error_writer.write_error(line_count, "Array cannot be declared void");
		}
		string name = $2->get_name();
		bool success = ST->insert_into_current_scope(name, type);
		if(success)
		{
			auto inserted_symbol = ST->lookup(name);
			inserted_symbol->set_evaluated_type(type);
			inserted_symbol->set_property(IS_ARRAY);
			write_ARRAY_to_SymbolInfo(inserted_symbol);
		}
		else
		{
			error_writer.write_error(line_count, "Redeclaration");
		}
	}
	;
 		 
type_specifier	: 	INT
	{
		log_writer.write_grammar_rule("type_specifier	: INT ");
		$$ = new SymbolInfo("INT","type_specifier");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
		$$->set_evaluated_type("INT");      // this is needed for nameless parameters
	}
	| FLOAT
	{
		log_writer.write_grammar_rule("type_specifier	: FLOAT ");
		$$ = new SymbolInfo("FLOAT","type_specifier");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
		$$->set_evaluated_type("FLOAT");	// this is needed for nameless parameters
	}
	| VOID
	{
		log_writer.write_grammar_rule("type_specifier	: VOID");
		$$ = new SymbolInfo("VOID","type_specifier");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
		$$->set_evaluated_type("VOID");		// this is needed for nameless parameters
	}
	;
 		
declaration_list : 	declaration_list COMMA ID
	{
		if( !($1->check_property(SYNTAX_ERROR_OCCURRED)) )
		{
			log_writer.write_grammar_rule("declaration_list : declaration_list COMMA ID  ");
		}
		
		$$ = new SymbolInfo("declaration_list COMMA ID","declaration_list");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		$$->set_child_identifiers($1->get_child_identifiers());

		if( $1->check_property(SYNTAX_ERROR_OCCURRED) )
		{
			// if error occurred, don't add any more...
			$$->set_syntax_error();
		}
		else 
		{
			$$->add_child_identifier($3);
		}
	}
	| declaration_list COMMA ID LTHIRD CONST_INT RTHIRD
	{
		log_writer.write_grammar_rule("declaration_list : declaration_list COMMA ID LSQUARE CONST_INT RSQUARE ");
		$$ = new SymbolInfo("declaration_list COMMA ID LSQUARE CONST_INT RSQUARE","declaration_list");
		$$->set_line_counts($1->get_start_line_count(), $6->get_end_line_count());
		$$->set_children_tree_nodes(6,$1,$2,$3,$4,$5,$6);

		$3->set_property(IS_ARRAY);
		$3->set_arr_size(stoi($5->get_name()));

		$$->set_child_identifiers($1->get_child_identifiers());

		if( ($1->check_property(SYNTAX_ERROR_OCCURRED)) )
		{
			// if error occurred, don't add any more...
			$$->set_syntax_error();
		}
		else 
		{
			$$->add_child_identifier($3);
		}
	}
	| ID
	{
		log_writer.write_grammar_rule("declaration_list : ID ");
		$$ = new SymbolInfo("ID","declaration_list");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		$$->add_child_identifier($1);
	}
	| ID LTHIRD CONST_INT RTHIRD
	{
		log_writer.write_grammar_rule("declaration_list : ID LSQUARE CONST_INT RSQUARE ");
		$$ = new SymbolInfo("ID LSQUARE CONST_INT RSQUARE","declaration_list");
		$$->set_line_counts($1->get_start_line_count(), $4->get_end_line_count());
		$$->set_children_tree_nodes(4,$1,$2,$3,$4);

		$1->set_property(IS_ARRAY);
		$1->set_arr_size(stoi($3->get_name()));

		$$->add_child_identifier($1);
	}
	;
 		  
statements : 	statement
	{
		log_writer.write_grammar_rule("statements : statement  ");
		$$ = new SymbolInfo("statement","statements");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
	}
	| statements statement
	{
		log_writer.write_grammar_rule("statements : statements statement  ");
		$$ = new SymbolInfo("statements statement","statements");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);
	}
	;
	   
statement : var_declaration
	{
		log_writer.write_grammar_rule("statement : var_declaration ");
		$$ = new SymbolInfo("var_declaration","statement");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
	}
	| expression_statement
	{
		log_writer.write_grammar_rule("statement : expression_statement  ");
		$$ = new SymbolInfo("expression_statement","statement");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
	}
	| compound_statement
	{
		log_writer.write_grammar_rule("statement : compound_statement ");
		$$ = new SymbolInfo("compound_statement","statement");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
	}
	| FOR LPAREN expression_statement expression_statement expression RPAREN statement
	{
		// for loop

		log_writer.write_grammar_rule_default_style("statement","FOR LPAREN expression_statement expression_statement expression RPAREN statement");
		$$ = new SymbolInfo("FOR LPAREN expression_statement expression_statement expression RPAREN statement","statement");
		$$->set_line_counts($1->get_start_line_count(), $7->get_end_line_count());
		$$->set_children_tree_nodes(7,$1,$2,$3,$4,$5,$6,$7);
	}
	| IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
	{
		// if without else 

		log_writer.write_grammar_rule_default_style("statement","IF LPAREN expression RPAREN statement");
		$$ = new SymbolInfo("IF LPAREN expression RPAREN statement","statement");
		$$->set_line_counts($1->get_start_line_count(), $5->get_end_line_count());
		$$->set_children_tree_nodes(5,$1,$2,$3,$4,$5);
	}	
	| IF LPAREN expression RPAREN statement ELSE statement
	{
		// if with else 

		log_writer.write_grammar_rule_default_style("statement","IF LPAREN expression RPAREN statement ELSE statement");
		$$ = new SymbolInfo("IF LPAREN expression RPAREN statement ELSE statement","statement");
		$$->set_line_counts($1->get_start_line_count(), $7->get_end_line_count());
		$$->set_children_tree_nodes(7,$1,$2,$3,$4,$5,$6,$7);
	}
	| WHILE LPAREN expression RPAREN statement
	{
		// while loop 

		log_writer.write_grammar_rule_default_style("statement","WHILE LPAREN expression RPAREN statement");
		$$ = new SymbolInfo("WHILE LPAREN expression RPAREN statement","statement");
		$$->set_line_counts($1->get_start_line_count(), $5->get_end_line_count());
		$$->set_children_tree_nodes(5,$1,$2,$3,$4,$5);
	}
	| PRINTLN LPAREN ID RPAREN SEMICOLON
	{
		log_writer.write_grammar_rule_default_style("statement","WHILE LPAREN expression RPAREN statement");
		$$ = new SymbolInfo("PRINTLN LPAREN ID RPAREN SEMICOLON","statement");
		$$->set_line_counts($1->get_start_line_count(), $5->get_end_line_count());
		$$->set_children_tree_nodes(5,$1,$2,$3,$4,$5);
	}
	| RETURN expression SEMICOLON
	{
		log_writer.write_grammar_rule("statement : RETURN expression SEMICOLON");
		$$ = new SymbolInfo("RETURN expression SEMICOLON","statement");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);
	}
	;
	  
expression_statement : 	SEMICOLON	
	{
		log_writer.write_grammar_rule("expression_statement : SEMICOLON ");
		$$ = new SymbolInfo("SEMICOLON","expression_statement");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
	}		
	| expression SEMICOLON 
	{
		if( $1->check_property(SYNTAX_ERROR_OCCURRED) )
		{
			report_syntax_error("expression","expression statement",$1->get_start_line_count());
		}
		else 
		{
			log_writer.write_grammar_rule("expression_statement : expression SEMICOLON 		 ");
		}
		$$ = new SymbolInfo("expression SEMICOLON","expression_statement");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);
	}
	;
	  
variable : 	ID 	
	{
		log_writer.write_grammar_rule("variable : ID 	 ");
		$$ = new SymbolInfo("ID","variable");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		string name = $1->get_name();
		int line_number = $1->get_start_line_count();
		bool error_occured = 0;

		auto inserted_symbol = ST->lookup(name);
		if(inserted_symbol == NULL)
		{
			error_writer.write_error(line_number, string("Undeclared variable ") + quote + name + quote);
			error_occured = 1;
		}
		else 
		{
			if(inserted_symbol->check_property(IS_FUNCTION))
			{
				error_writer.write_error(line_number, string("Function ") + quote + name + quote + string(" cannot be used as variable"));
				error_occured = 1;
			}
			else
			{
				// not checking array, as C allows array to int, with warning
			}
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else 
		{
			copy_type_and_properties($$, inserted_symbol);
		}

	}	
	| ID LTHIRD expression RTHIRD 
	{
		log_writer.write_grammar_rule("variable : ID LSQUARE expression RSQUARE  	 ");
		$$ = new SymbolInfo("ID LSQUARE expression RSQUARE","variable");
		$$->set_line_counts($1->get_start_line_count(), $4->get_end_line_count());
		$$->set_children_tree_nodes(4,$1,$2,$3,$4);

		string name = $1->get_name();
		int line_number = $1->get_start_line_count();
		bool error_occured = 0;

		auto inserted_symbol = ST->lookup(name);
		if(inserted_symbol == NULL)
		{
			error_writer.write_error(line_number, string("Undeclared variable ") + quote + name + quote);
			error_occured = 1;
		}
		else 
		{
			if(inserted_symbol->check_property(IS_FUNCTION))
			{
				error_writer.write_error(line_number, string("Function ") + quote + name + quote + string(" cannot be used as variable"));
				error_occured = 1;
			}
			else if( ! inserted_symbol->check_property(IS_ARRAY) )
			{
				error_writer.write_error(line_number, quote + name + quote + string(" is not an array"));
				error_occured = 1;
			}
			else if($3 -> get_evaluated_type() != TYPE_INT)
			{
				error_writer.write_error(line_number, "Array subscript is not an integer");
				error_occured = 1;
			}
			else
			{
				// valid
			}
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else 
		{
			copy_type_and_properties($$, inserted_symbol);
			$$->unset_property(IS_ARRAY); // because due to the subscript, it is no longer an array
		}
	}
	;
	 
expression : 	logic_expression
	{
		log_writer.write_grammar_rule("expression 	: logic_expression	 ");
		$$ = new SymbolInfo("logic_expression","expression");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		copy_type_and_properties($$,$1);

		if($1->check_property(SYNTAX_ERROR_OCCURRED))
		{
			$$->set_syntax_error();
		}
	}
	| variable ASSIGNOP logic_expression 	
	{
		log_writer.write_grammar_rule("expression 	: variable ASSIGNOP logic_expression 		 ");
		$$ = new SymbolInfo("variable ASSIGNOP logic_expression","expression");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		bool error_occured = 0;

		string type_1 = $1->get_evaluated_type();
		string type_2 = $3->get_evaluated_type();

		if(type_1 == TYPE_ERROR or type_2 == TYPE_ERROR)
		{
			error_occured = 1;
		}
		else if(type_1 == TYPE_VOID or type_2 == TYPE_VOID)
		{
			error_writer.write_error(line_count, "Void cannot be used in expression ");
			error_occured = 1;
		}
		else if($1->check_property(IS_ARRAY) && !($3->check_property(IS_ARRAY)))
		{
			// array = int // this type of expression
			error_writer.write_error(line_count, "Assignment to expression with array type");
			error_occured = 1;
		}
		else if(!($1->check_property(IS_ARRAY)) && $3->check_property(IS_ARRAY))
		{
			// int = array // this type of expression
			// it is allowed in C
		}
		else if(type_1 == TYPE_INT && type_2 == TYPE_FLOAT)
		{
			// int = float // this type of expression
			error_writer.write_error(line_count, "Warning: possible loss of data in assignment of FLOAT to INT");
			error_occured = 1;
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else
		{
			$$->set_evaluated_type(get_type_after_operation(type_1, type_2));
		}

		if($3->check_property(SYNTAX_ERROR_OCCURRED))
		{
			$$->set_syntax_error();
		}
	}
	| variable ASSIGNOP error	
	{
		//log_writer.write_grammar_rule("expression 	: variable ASSIGNOP logic_expression 		 ");
		$$ = new SymbolInfo("error","expression");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);

		log_writer.write_log_syntax_error();

		$$->set_syntax_error();
	}
	;
			
logic_expression : 	rel_expression 	
	{
		log_writer.write_grammar_rule("logic_expression : rel_expression 	 ");
		$$ = new SymbolInfo("rel_expression","logic_expression");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		copy_type_and_properties($$,$1);

		if($1->check_property(SYNTAX_ERROR_OCCURRED))
		{
			$$->set_syntax_error();
		}
	}
	| rel_expression LOGICOP rel_expression 	
	{
		log_writer.write_grammar_rule("logic_expression : rel_expression LOGICOP rel_expression 	 	 ");
		$$ = new SymbolInfo("rel_expression LOGICOP rel_expression","logic_expression");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		bool error_occured = 0;

		string type_1 = $1->get_evaluated_type();
		string type_2 = $3->get_evaluated_type();

		if(type_1 == TYPE_ERROR or type_2 == TYPE_ERROR)
		{
			error_occured = 1;
		}
		else if(type_1 == TYPE_VOID or type_2 == TYPE_VOID)
		{
			error_writer.write_error(line_count, "Void cannot be used in expression ");
			error_occured = 1;
		}
		else 
		{
			// anything else is okay
			// because, any two types can be operated with LOGICOP
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else
		{
			// the result of RELOP and LOGICOP operation should be an integer
			$$->set_evaluated_type(TYPE_INT);
		}
	}
	;
			
rel_expression	: 	simple_expression 
	{
		log_writer.write_grammar_rule("rel_expression	: simple_expression ");
		$$ = new SymbolInfo("simple_expression","rel_expression");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		copy_type_and_properties($$,$1);

		if($1->check_property(SYNTAX_ERROR_OCCURRED))
		{
			$$->set_syntax_error();
		}
	}
	| simple_expression RELOP simple_expression	
	{
		log_writer.write_grammar_rule("rel_expression	: simple_expression RELOP simple_expression	  ");
		$$ = new SymbolInfo("simple_expression RELOP simple_expression","rel_expression");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		bool error_occured = 0;

		string type_1 = $1->get_evaluated_type();
		string type_2 = $3->get_evaluated_type();

		if(type_1 == TYPE_ERROR or type_2 == TYPE_ERROR)
		{
			error_occured = 1;
		}
		else if(type_1 == TYPE_VOID or type_2 == TYPE_VOID)
		{
			error_writer.write_error(line_count, "Void cannot be used in expression ");
			error_occured = 1;
		}
		else if( $1->check_property(IS_ARRAY) and !($3->check_property(IS_ARRAY)) )
		{
			error_writer.write_error(line_count, string("Warning: comparison between pointer and ") + get_lowercase_string(type_2));
		}
		else if( !($1->check_property(IS_ARRAY)) and $1->check_property(IS_ARRAY) )
		{
			error_writer.write_error(line_count, string("Warning: comparison between pointer and ") + get_lowercase_string(type_1));
		}
		else 
		{
			// anything else is okay
			// because, any type can be compared with any other type also pointer and pointer
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else
		{
			// the result of RELOP and LOGICOP operation should be an integer
			$$->set_evaluated_type(TYPE_INT);
		}
	}
	;
				
simple_expression : term 
	{
		log_writer.write_grammar_rule("simple_expression : term ");
		$$ = new SymbolInfo("term","simple_expression");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		copy_type_and_properties($$,$1);
	}
	| simple_expression ADDOP term 
	{
		log_writer.write_grammar_rule("simple_expression : simple_expression ADDOP term  ");
		$$ = new SymbolInfo("simple_expression ADDOP term","simple_expression");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		bool error_occured = 0;

		string type_1 = $1->get_evaluated_type();
		string type_2 = $3->get_evaluated_type();

		if(type_1 == TYPE_ERROR or type_2 == TYPE_ERROR)
		{
			error_occured = 1;
		}
		else if(type_1 == TYPE_VOID or type_2 == TYPE_VOID)
		{
			error_writer.write_error(line_count, "Void cannot be used in expression ");
			error_occured = 1;
		}
		else if( $1->check_property(IS_ARRAY) or $3->check_property(IS_ARRAY) )
		{
			// only valid for same types in C
			if(type_1 != type_2)
			{
				error_writer.write_error(line_count, string("Invalid operands to binary operator ") + $2->get_name());
				error_occured = 1;
			}
		}
		else 
		{
			// anything else is okay
			// because, any type can be added with any other type
			// adding 
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else
		{
			// the result of RELOP and LOGICOP operation should be an integer
			$$->set_evaluated_type(get_type_after_operation(type_1,type_2));
		}
	}
	;
					
term :	unary_expression
	{
		log_writer.write_grammar_rule("term :	unary_expression ");
		$$ = new SymbolInfo("unary_expression","term");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		copy_type_and_properties($$,$1);
	}
	|  term MULOP unary_expression
	{
		log_writer.write_grammar_rule("term :	term MULOP unary_expression ");
		$$ = new SymbolInfo("term MULOP unary_expression","term");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		bool error_occured = 0;

		string type_1 = $1->get_evaluated_type();
		string type_2 = $3->get_evaluated_type();

		if(type_1 == TYPE_ERROR or type_2 == TYPE_ERROR)
		{
			error_occured = 1;
		}
		else if(type_1 == TYPE_VOID or type_2 == TYPE_VOID)
		{
			error_writer.write_error(line_count, "Void cannot be used in expression ");
			error_occured = 1;
		}
		else if( $1->check_property(IS_ARRAY) or $3->check_property(IS_ARRAY) )
		{
			// multiplying with array is not permitted
			error_writer.write_error(line_count, string("Invalid operands to binary operator ") + $2->get_name());
			error_occured = 1;
		}
		else if( $2->get_name() == string("%") )
		{
			if( type_1 == TYPE_FLOAT or type_2 == TYPE_FLOAT )
			{
				error_writer.write_error(line_count, "Operands of modulus must be integers ");
				error_occured = 1;
			}
			else if( $3->get_constant_text()!=EMPTY_CONSTANT_TEXT and stoi($3->get_constant_text())==0)
			{
				error_writer.write_error(line_count, "Warning: division by zero");
				error_occured = 1;
			}
		}
		else if( $2->get_name() == string("/") )
		{
			if( $3->get_constant_text()!=EMPTY_CONSTANT_TEXT and stoi($3->get_constant_text())==0)
			{
				error_writer.write_error(line_count, "Warning: division by zero");
				error_occured = 1;
			}
		}
		else 
		{
			// anything else is okay
			// because, any type can be compared with any other type
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else
		{
			// the result of RELOP and LOGICOP operation should be an integer
			$$->set_evaluated_type(get_type_after_operation(type_1, type_2));
		}
	}
	;

unary_expression : 	ADDOP unary_expression  
	{
		log_writer.write_grammar_rule_default_style("unary_expression","ADDOP unary_expression");
		$$ = new SymbolInfo("ADDOP unary_expression","unary_expression");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);

		bool error_occured = 0;
		string type_1 = $2->get_evaluated_type();

		if(type_1 == TYPE_ERROR)
		{
			error_occured = 1;
		}
		else if($2 -> check_property(IS_ARRAY))
		{
			error_writer.write_error(line_count, string("Wrong type of argument to unary ") + $1->get_name());
			error_occured = 1;
		}
		else if(type_1 == TYPE_VOID)
		{
			error_writer.write_error(line_count, "Void cannot be used in expression ");
			error_occured = 1;
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else
		{
			copy_type_and_properties($$,$2);
		}
	}
	| NOT unary_expression 
	{
		log_writer.write_grammar_rule_default_style("unary_expression","NOT unary_expression");
		$$ = new SymbolInfo("NOT unary_expression","unary_expression");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);

		bool error_occured = 0;
		string type_1 = $2->get_evaluated_type();

		if(type_1 == TYPE_ERROR)
		{
			error_occured = 1;
		}
		if($2 -> check_property(IS_ARRAY))
		{
			error_writer.write_error(line_count, string("Wrong type of argument to unary ") + $1->get_name());
			error_occured = 1;
		}
		else if(type_1 == TYPE_VOID)
		{
			error_writer.write_error(line_count, "Void cannot be used in expression ");
			error_occured = 1;
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else
		{
			$$->set_evaluated_type(TYPE_INT); // because NOT ... is always int
		}
	}
	| factor 
	{
		log_writer.write_grammar_rule("unary_expression : factor ");
		$$ = new SymbolInfo("factor","unary_expression");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		copy_type_and_properties($$,$1);
	}
	;
	
factor	: 	variable 
	{
		log_writer.write_grammar_rule("factor	: variable ");
		$$ = new SymbolInfo("variable","factor");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		copy_type_and_properties($$,$1);
	}
	| ID LPAREN argument_list RPAREN
	{
		log_writer.write_grammar_rule("factor	: ID LPAREN argument_list RPAREN  ");
		$$ = new SymbolInfo("ID LPAREN argument_list RPAREN","factor");
		$$->set_line_counts($1->get_start_line_count(), $4->get_end_line_count());
		$$->set_children_tree_nodes(4,$1,$2,$3,$4);

		debug("here");
		
		// this means function call

		string name = $1->get_name();
		int line_number = $1->get_start_line_count();
		bool error_occured = 0;

		auto inserted_function = ST->lookup(name);
		debug("here2");
		if(inserted_function == NULL)
		{
			debug("here3");
			error_writer.write_error(line_number, string("Undeclared function ") + quote + name + quote);
			error_occured = 1;
		}
		else 
		{
			debug("here4");
			if( ! inserted_function->check_property(IS_FUNCTION) )
			{
				error_writer.write_error(line_number, quote + name + quote + string(" is not a function")  );
				error_occured = 1;
			}
			else if( ! inserted_function->check_property(IS_DEFINED_FUNCTION) )
			{
				error_writer.write_error(line_number, string("Undefined reference to") + quote + name + quote );
				error_occured = 1;
			}
			else if($3->get_argument_count() > inserted_function->get_parameter_count())
			{
				error_writer.write_error(line_number, string("Too many arguments to function ") + quote + name + quote);
				error_occured = 1;
			}
			else if($3->get_argument_count() < inserted_function->get_parameter_count())
			{
				error_writer.write_error(line_number, string("Too few arguments to function ") + quote + name + quote);
				error_occured = 1;
			}
			else  // need to check all arguments one by one
			{
				int n = $3->get_argument_count();

				auto args = $3->get_argument_types();
				auto params = inserted_function->get_parameter_types();

				for(int i=0; i<n; i++)
				{
					if(args[i]==TYPE_ERROR or params[i]==TYPE_ERROR)
					{
						error_occured = 1; // don't throw error again
					}
					else if(args[i] != params[i])
					{
						error_writer.write_error(line_number, string("Type mismatch for argument ") + to_string(i+1) + string(" of ") + quote + name + quote);
						error_occured = 1;
					}
				}
			}
			if(inserted_function->get_evaluated_type() == TYPE_VOID) // to throw void error in subsequent lines
			{
				error_occured = 0;  // Note that, VOID is written later... and it helps detecting void usage in expression
			}
		}

		if(error_occured) 
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else 
		{
			$$->set_evaluated_type(inserted_function->get_evaluated_type());  // return type of function
		}

	}
	| LPAREN expression RPAREN
	{
		log_writer.write_grammar_rule("factor	: LPAREN expression RPAREN   ");
		$$ = new SymbolInfo("LPAREN expression RPAREN","factor");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		copy_type_and_properties($$,$2);
	}
	| CONST_INT 
	{
		log_writer.write_grammar_rule("factor	: CONST_INT   ");
		$$ = new SymbolInfo("CONST_INT","factor");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		$$->set_evaluated_type(TYPE_INT);
		$$->set_constant_text($1->get_name());
	}
	| CONST_FLOAT
	{
		log_writer.write_grammar_rule("factor	: CONST_FLOAT   ");
		$$ = new SymbolInfo("CONST_FLOAT","factor");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		$$->set_evaluated_type(TYPE_FLOAT);
		$$->set_constant_text($1->get_name());
	}
	| variable INCOP 
	{
		log_writer.write_grammar_rule_default_style("factor","variable INCOP");
		$$ = new SymbolInfo("variable INCOP","factor");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);

		bool error_occured = 0;
		string type_1 = $1->get_evaluated_type();

		if(type_1 == TYPE_ERROR)
		{
			error_occured = 1;
		}
		else if($1 -> check_property(IS_ARRAY))
		{
			// array cannot be incremented
			error_writer.write_error(line_count, string("Wrong type of argument to increment operator"));
			error_occured = 1;
		}
		else if(type_1 == TYPE_VOID)
		{
			error_writer.write_error(line_count, "Void cannot be used in expression ");
			error_occured = 1;
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else
		{
			copy_type_and_properties($$,$1);
		}
	}
	| variable DECOP
	{
		log_writer.write_grammar_rule_default_style("factor","variable DECOP");
		$$ = new SymbolInfo("variable DECOP","factor");
		$$->set_line_counts($1->get_start_line_count(), $2->get_end_line_count());
		$$->set_children_tree_nodes(2,$1,$2);

		bool error_occured = 0;
		string type_1 = $1->get_evaluated_type();

		if(type_1 == TYPE_ERROR)
		{
			error_occured = 1;
		}
		else if($1 -> check_property(IS_ARRAY))
		{
			// array cannot be decremented
			error_writer.write_error(line_count, string("Wrong type of argument to decrement operator"));
			error_occured = 1;
		}
		else if(type_1 == TYPE_VOID)
		{
			error_writer.write_error(line_count, "Void cannot be used in expression ");
			error_occured = 1;
		}

		if(error_occured)
		{
			$$->set_evaluated_type(TYPE_ERROR);
		}
		else
		{
			copy_type_and_properties($$,$1);
		}
	}
	;
	
argument_list : arguments
	{
		log_writer.write_grammar_rule("argument_list : arguments  ");
		$$ = new SymbolInfo("arguments","argument_list");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);
		
		$$->set_argument_types($1->get_argument_types());
	}
	;
	
arguments : arguments COMMA logic_expression
	{
		log_writer.write_grammar_rule("arguments : arguments COMMA logic_expression ");
		$$ = new SymbolInfo("arguments COMMA logic_expression","arguments");
		$$->set_line_counts($1->get_start_line_count(), $3->get_end_line_count());
		$$->set_children_tree_nodes(3,$1,$2,$3);

		$$->set_argument_types($1->get_argument_types());
		$$->add_argument_type($3->get_evaluated_type());
	}
	| logic_expression
	{
		log_writer.write_grammar_rule("arguments : logic_expression");
		$$ = new SymbolInfo("logic_expression","arguments");
		$$->set_line_counts($1->get_start_line_count(), $1->get_end_line_count());
		$$->set_children_tree_nodes(1,$1);

		$$->add_argument_type($1->get_evaluated_type());
	}
	;
 

%%

int main(int argc,char *argv[])
{

	if((fp=fopen(argv[1],"r"))==NULL)
	{
		printf("Cannot Open Input File.\n");
		exit(1);
	}

	yyin=fp;
	yyparse();

	log_writer.write_log_footer();

	logout.close();
	errorout.close();
	
	return 0;
}

/*

Shell Script:



#!/bin/bash

# Set debugging variable
DEBUG=0

# Debug file prefix
DEBUG_FILE_PREFIX="2005001_"

# Declare variables for file names
YACC_FILE="2005001.y"
LEX_FILE="2005001.l"
INPUT_FILE="input.c"

# Declare variables for intermediate files
YACC_OUTPUT="y.tab"
LEX_OUTPUT="lex.yy"
DEBUG_FILE="${DEBUG_FILE_PREFIX}debug.txt"
EXECUTABLE_NAME="a"

# Run yacc to generate parser files
yacc -d -y "$YACC_FILE"
echo 'Generated the parser C file as well as the header file'

# Compile the parser object file
g++ -w -c -o y.o "${YACC_OUTPUT}.c"
echo 'Generated the parser object file'

# Run flex to generate scanner files
flex "$LEX_FILE"
echo 'Generated the scanner C file'

# Compile the scanner object file
g++ -w -c -o l.o "${LEX_OUTPUT}.c"
# if the above command doesn't work, try g++ -fpermissive -w -c -o l.o lex.yy.c
echo 'Generated the scanner object file'

# Link the parser and scanner object files to create the executable
g++ y.o l.o -lfl -o "$EXECUTABLE_NAME"
echo 'All ready, running'

# Run the executable with the specified input file
./"$EXECUTABLE_NAME" "$INPUT_FILE"

# Check if debugging is enabled before deleting the debug file
if [ "$DEBUG" -eq 1 ]; then
    echo 'Debugging enabled, keeping the debug file'
else
    rm -f "${DEBUG_FILE}"
    echo 'Debug file deleted as debug mode is off'
fi

# Clean up intermediate files (excluding debug file) and executable
rm -f "${LEX_OUTPUT}.c" "${YACC_OUTPUT}.c" "${YACC_OUTPUT}.h" l.o y.o "$EXECUTABLE_NAME"
echo 'Cleaned up intermediate files and executable'



*/