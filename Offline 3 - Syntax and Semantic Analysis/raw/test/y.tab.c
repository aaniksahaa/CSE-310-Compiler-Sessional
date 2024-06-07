/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 2
#define YYMINOR 0
#define YYPATCH 20220114

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#undef YYBTYACC
#define YYBTYACC 0
#define YYDEBUGSTR YYPREFIX "debug"
#define YYPREFIX "yy"

#define YYPURE 0

#line 1 "b.y"

#include<bits/stdc++.h>
#include "headers/2005001_SymbolTable.hpp"
#include "headers/2005001_SymbolInfo.hpp"

using namespace std;

int yyparse(void); 
int yylex(void);

extern FILE* yyin;
FILE *fp;

extern SymbolTable* ST;

extern int line_count;
int total_error_count = 0;

ofstream logout("log.txt");
ofstream treeout("parsetree.txt");
ofstream errorout("error.txt");
ofstream debugout("debug.txt");

class Writer
{
    ofstream& out;
public:
    Writer(ofstream& outFile) : out(outFile)
    {

    }
    void write_grammar_rule(SymbolInfo* info)
    {
        out<<info->get_type()<<"\t: "<<info->get_name()<<" \n";
    }
    void write_grammar_rule(string rule) /* overridden for directly pasting as per testcase*/
    {
        out<<rule<<"\n";
    }
	void write_grammar_rule_default_style(string left, string right)
    {
        out<<left<<" : "<<right<<" \n";
    }
	void write_parse_tree(TreeNode* root)
	{
		ParseTree* tree= new ParseTree(root);
		out<<tree->print_tree();
	}
	void write_error(int line_number, string error_text)
	{
		total_error_count++;
		out<<"Line# "<<line_number<<": "<<error_text<<"\n";
	}
	void debug(string debug_text)
	{
		out<<debug_text<<"\n";
	}
	void write_log_footer()
	{
		out<<"Total Lines: "<<line_count<<"\n";
		out<<"Total Errors: "<<total_error_count<<"\n";
	}
};

Writer log_writer(logout);
Writer tree_writer(treeout);
Writer error_writer(errorout);
Writer debug_writer(debugout);


void yyerror(char *s)
{
	logout<<"syntax error\n";
}

/* helper global variables*/

string quote = "'";
string space = " ";
string newline = "\n";

/* helper functions*/

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

/* Parsing helper global variables*/

bool exit_scope_pending = 0;
SymbolInfo* pending_function_insert = NULL;

/* Parsing helper functions*/

vector<string> process_parameters(SymbolInfo* p_list)
{
	vector<string>valid_parameter_types;

	SymbolTable* dummy_ST = new SymbolTable(11);

	dummy_ST->enter_scope();   /* this is done for checking validity of parameters*/

	for(auto child: p_list->get_child_identifiers())
	{
		int line_number = child->get_start_line_count();
		string name = child->get_name();
		string type = child->get_evaluated_type();

		/* following C, storing all types in the function for now*/
		/* However, it behaves differently in C++*/
		valid_parameter_types.push_back(type);

		if(type == "VOID")
		{
			error_writer.write_error(line_number, "function parameter cannot be void");
		}
		if(child->get_type() == "type_specifier")  /* unnamed parameter*/
		{
			/*valid_parameter_types.push_back(type);*/
		}
		else 
		{
			bool success = dummy_ST->insert_into_current_scope(name, type);
			if(success)
			{
				/*valid_parameter_types.push_back(type);*/
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
			/* check if functions' attributes completely match*/
			int mismatch = check_function_mismatch(existing_symbol, return_type, parameter_types);
			if(mismatch)
			{
				error_occured = 1;
				error_writer.write_error(line_number, "Conflicting types for "+quote+function_name+quote);
			}
			else  /* so exactly same function*/
			{
				/* no error, because, declaring same function again is permitted*/
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
	/*bool success = ST->insert_into_current_scope(function_name, string("FUNCTION,")+return_type);*/
	bool success = (ST->lookup(function_name) == NULL);  /* As we are in the global scope, lookup is valid and sufficient*/
	if(success)
	{
		/* debug_writer.debug("inserted");*/
		/* auto inserted_function = ST->lookup(function_name);*/
		/* inserted_function->set_property(IS_FUNCTION);*/
		/* inserted_function->set_property(IS_DEFINED_FUNCTION);*/
		/* for(auto type: parameter_types)*/
		/* {*/
		/* 	inserted_function->add_parameter_type(type);*/
		/* }*/
		/* inserted_function->set_evaluated_type(return_type);*/


		/* Pending the insert for later*/


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
			/* check if functions' attributes completely match*/
			int mismatch = check_function_mismatch(existing_symbol, return_type, parameter_types);
			if(mismatch)
			{
				error_occured = 1;
				error_writer.write_error(line_number, "Conflicting types for "+quote+function_name+quote);
			}
			else  /* so exactly same function*/
			{
				/* now, no error it is only declared before, but error if it is defined before*/
				if(existing_symbol->check_property(IS_DEFINED_FUNCTION))
				{
					error_occured = 1;
					error_writer.write_error(line_number, "Redefinition of "+quote+function_name+quote);
				}
				else 
				{
					/* no error*/
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


#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 311 "b.y"
typedef union YYSTYPE {
	SymbolInfo* info;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 344 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

#if !(defined(yylex) || defined(YYSTATE))
int YYLEX_DECL();
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define IF 257
#define FLOAT 258
#define FOR 259
#define INT 260
#define VOID 261
#define ELSE 262
#define WHILE 263
#define RETURN 264
#define PRINTLN 265
#define ADDOP 266
#define MULOP 267
#define INCOP 268
#define DECOP 269
#define RELOP 270
#define ASSIGNOP 271
#define LOGICOP 272
#define BITOP 273
#define NOT 274
#define LPAREN 275
#define RPAREN 276
#define LCURL 277
#define RCURL 278
#define LTHIRD 279
#define RTHIRD 280
#define COMMA 281
#define SEMICOLON 282
#define CONST_INT 283
#define CONST_FLOAT 284
#define ID 285
#define CONST_CHAR 286
#define SINGLE_LINE_STRING 287
#define MULTI_LINE_STRING 288
#define LOWER_THAN_ELSE 289
#define YYERRCODE 256
typedef int YYINT;
static const YYINT yylhs[] = {                           -1,
    0,   21,   21,   22,   22,   22,   18,   18,   23,   19,
   24,   19,   20,   20,   20,   20,   17,   17,   14,   15,
   15,   15,   16,   16,   16,   16,    3,    3,    4,    4,
    4,    4,    4,    4,    4,    4,    4,    5,    5,    6,
    6,    7,    7,    8,    8,    9,    9,   10,   10,   11,
   11,   12,   12,   12,   13,   13,   13,   13,   13,   13,
   13,    1,    2,    2,
};
static const YYINT yylen[] = {                            2,
    1,    2,    1,    1,    1,    1,    6,    5,    0,    7,
    0,    6,    4,    3,    2,    1,    3,    2,    3,    1,
    1,    1,    3,    6,    1,    4,    1,    2,    1,    1,
    1,    7,    5,    7,    5,    5,    3,    1,    2,    1,
    4,    1,    3,    1,    3,    1,    3,    1,    3,    1,
    3,    2,    2,    1,    1,    4,    3,    1,    1,    2,
    2,    1,    3,    1,
};
static const YYINT yydefred[] = {                         0,
   21,   20,   22,    0,    4,    0,    5,    6,    0,    3,
    0,    0,    2,    0,    0,    0,   19,    0,    0,    0,
    0,    0,    8,    0,   15,    0,    0,   26,    0,    0,
   12,    7,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   18,   38,   58,   59,    0,    0,   27,
   30,    0,    0,   42,    0,    0,    0,   50,   54,   29,
    0,   31,   10,   13,   24,    0,    0,    0,    0,    0,
    0,   52,   53,    0,    0,    0,   17,   28,   60,   61,
    0,   39,    0,    0,    0,    0,    0,    0,    0,    0,
   37,    0,   57,    0,    0,   64,    0,   43,   45,    0,
    0,   51,    0,    0,    0,    0,   56,    0,   41,    0,
    0,   35,   36,   63,    0,    0,   34,   32,
};
#if defined(YYDESTRUCT_CALL) || defined(YYSTYPE_TOSTRING)
static const YYINT yystos[] = {                           0,
  258,  260,  261,  291,  305,  306,  309,  310,  312,  313,
  285,  307,  313,  275,  279,  281,  282,  276,  306,  311,
  283,  285,  282,  315,  285,  276,  281,  280,  279,  277,
  308,  282,  314,  306,  283,  257,  259,  263,  264,  265,
  266,  274,  275,  278,  282,  283,  284,  285,  294,  295,
  296,  297,  298,  299,  300,  301,  302,  303,  304,  305,
  306,  308,  308,  285,  280,  275,  275,  275,  298,  275,
  297,  303,  303,  298,  275,  279,  278,  295,  268,  269,
  271,  282,  272,  266,  270,  267,  285,  298,  296,  298,
  282,  285,  276,  292,  293,  299,  298,  299,  300,  302,
  301,  303,  276,  296,  276,  276,  276,  281,  280,  295,
  298,  295,  282,  299,  262,  276,  295,  295,
};
#endif /* YYDESTRUCT_CALL || YYSTYPE_TOSTRING */
static const YYINT yydgoto[] = {                          4,
   94,   95,   49,   50,   51,   52,   53,   54,   55,   56,
   57,   58,   59,   60,   61,   12,   62,    7,    8,   20,
    9,   10,   33,   24,
};
static const YYINT yysindex[] = {                      -144,
    0,    0,    0,    0,    0, -234,    0,    0, -144,    0,
 -257, -227,    0, -211, -242, -212,    0, -202, -197, -266,
 -186, -180,    0, -168,    0, -167, -144,    0, -165, -182,
    0,    0, -168, -166, -160, -152, -149, -148, -213, -147,
 -213, -213, -213,    0,    0,    0,    0, -244, -153,    0,
    0, -205, -139,    0, -134, -233, -123,    0,    0,    0,
 -140,    0,    0,    0,    0, -213, -258, -213, -136, -138,
 -209,    0,    0, -128, -213, -213,    0,    0,    0,    0,
 -213,    0, -213, -213, -213, -213, -130, -121, -258, -120,
    0, -119,    0, -109, -129,    0, -108,    0,    0, -123,
  -93,    0,  -95, -213,  -95, -107,    0, -213,    0,  -88,
 -100,    0,    0,    0,  -95,  -95,    0,    0,
};
static const YYINT yyrindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,  177,    0,
 -195,    0,    0,    0,    0,    0,    0,  -99, -253,    0,
    0, -184,    0,    0,    0,  -96,    0,    0,    0,    0,
    0,    0,    0, -224,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,  -75,    0,    0,
    0,  -51,    0,    0, -191,  -34,  -48,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -68,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -195,    0,    0,    0,
    0,    0,    0,    0,  -92,    0,    0,    0,    0,  -31,
  -20,    0,    0,    0,    0,    0,    0,    0,    0, -124,
    0,    0,    0,    0,    0,    0,    0,    0,
};
#if YYBTYACC
static const YYINT yycindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
#endif
static const YYINT yygindex[] = {                         0,
    0,    0,    0,  -47,  -60,  -41,  -30,  -69,  100,  101,
  116,  -38,    0,   11,    5,    0,   -3,    0,    0,    0,
    0,  176,    0,    0,
};
#define YYTABLESIZE 262
static const YYINT yytable[] = {                         71,
   71,   78,   72,   73,    6,   96,   89,   41,   69,   26,
    5,   98,   74,    6,   27,   42,   43,   14,   19,    5,
   31,   15,   16,   45,   46,   47,   48,   16,  104,   63,
   75,   34,   84,   71,   76,   88,   85,   90,  114,   71,
   21,   71,   71,   71,   71,   97,    1,  102,    2,    3,
   11,   14,   41,   16,   17,  110,   14,  112,   79,   80,
   42,   43,   79,   80,   18,   81,   71,  117,  118,   46,
   47,   48,   22,  111,   36,    1,   37,    2,    3,   23,
   38,   39,   40,   41,   44,   25,   25,   25,   44,   44,
   44,   42,   43,   28,   30,   44,   23,   23,   29,   45,
   46,   47,   48,   36,    1,   37,    2,    3,   30,   38,
   39,   40,   41,    1,   32,    2,    3,   35,   64,   65,
   42,   43,   66,   30,   77,   67,   68,   70,   45,   46,
   47,   48,   33,   33,   33,   33,   33,   83,   33,   33,
   33,   33,   82,   86,   87,   91,   92,   93,   15,   33,
   33,  108,   33,   33,  103,  105,  106,   33,   33,   33,
   33,   36,    1,   37,    2,    3,  107,   38,   39,   40,
   41,  109,   84,  115,  113,  116,    1,   11,   42,   43,
    9,   30,   99,   62,   13,  101,   45,   46,   47,   48,
   40,   40,   40,   40,   40,   40,   40,   55,   55,  100,
   40,   55,    0,   55,   40,   40,   40,   55,    0,    0,
    0,   55,   55,   55,   55,   55,    0,   48,   55,    0,
   55,   48,    0,   48,   55,    0,    0,   48,   55,    0,
   55,   48,   48,   48,   49,    0,    0,   46,   49,    0,
   49,   46,    0,    0,   49,   46,   46,   46,   49,   49,
   49,   47,    0,    0,    0,   47,    0,    0,    0,   47,
   47,   47,
};
static const YYINT yycheck[] = {                         41,
   42,   49,   41,   42,    0,   75,   67,  266,   39,  276,
    0,   81,   43,    9,  281,  274,  275,  275,   14,    9,
   24,  279,  276,  282,  283,  284,  285,  281,   89,   33,
  275,   27,  266,   75,  279,   66,  270,   68,  108,   81,
  283,   83,   84,   85,   86,   76,  258,   86,  260,  261,
  285,  276,  266,  281,  282,  103,  281,  105,  268,  269,
  274,  275,  268,  269,  276,  271,  108,  115,  116,  283,
  284,  285,  285,  104,  257,  258,  259,  260,  261,  282,
  263,  264,  265,  266,  276,  281,  282,  285,  280,  281,
  282,  274,  275,  280,  277,  278,  281,  282,  279,  282,
  283,  284,  285,  257,  258,  259,  260,  261,  277,  263,
  264,  265,  266,  258,  282,  260,  261,  283,  285,  280,
  274,  275,  275,  277,  278,  275,  275,  275,  282,  283,
  284,  285,  257,  258,  259,  260,  261,  272,  263,  264,
  265,  266,  282,  267,  285,  282,  285,  276,  279,  274,
  275,  281,  277,  278,  276,  276,  276,  282,  283,  284,
  285,  257,  258,  259,  260,  261,  276,  263,  264,  265,
  266,  280,  266,  262,  282,  276,    0,  277,  274,  275,
  277,  277,   83,  276,    9,   85,  282,  283,  284,  285,
  266,  267,  268,  269,  270,  271,  272,  266,  267,   84,
  276,  270,   -1,  272,  280,  281,  282,  276,   -1,   -1,
   -1,  280,  281,  282,  266,  267,   -1,  266,  270,   -1,
  272,  270,   -1,  272,  276,   -1,   -1,  276,  280,   -1,
  282,  280,  281,  282,  266,   -1,   -1,  272,  270,   -1,
  272,  276,   -1,   -1,  276,  280,  281,  282,  280,  281,
  282,  272,   -1,   -1,   -1,  276,   -1,   -1,   -1,  280,
  281,  282,
};
#if YYBTYACC
static const YYINT yyctable[] = {                        -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,
};
#endif
#define YYFINAL 4
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 289
#define YYUNDFTOKEN 316
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"$end",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"error","IF","FLOAT","FOR","INT","VOID",
"ELSE","WHILE","RETURN","PRINTLN","ADDOP","MULOP","INCOP","DECOP","RELOP",
"ASSIGNOP","LOGICOP","BITOP","NOT","LPAREN","RPAREN","LCURL","RCURL","LTHIRD",
"RTHIRD","COMMA","SEMICOLON","CONST_INT","CONST_FLOAT","ID","CONST_CHAR",
"SINGLE_LINE_STRING","MULTI_LINE_STRING","LOWER_THAN_ELSE","$accept","start",
"argument_list","arguments","statements","statement","expression_statement",
"variable","expression","logic_expression","rel_expression","simple_expression",
"term","unary_expression","factor","var_declaration","type_specifier",
"declaration_list","compound_statement","func_declaration","func_definition",
"parameter_list","program","unit","$$1","$$2","illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : start",
"start : program",
"program : program unit",
"program : unit",
"unit : var_declaration",
"unit : func_declaration",
"unit : func_definition",
"func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON",
"func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON",
"$$1 :",
"func_definition : type_specifier ID LPAREN parameter_list RPAREN $$1 compound_statement",
"$$2 :",
"func_definition : type_specifier ID LPAREN RPAREN $$2 compound_statement",
"parameter_list : parameter_list COMMA type_specifier ID",
"parameter_list : parameter_list COMMA type_specifier",
"parameter_list : type_specifier ID",
"parameter_list : type_specifier",
"compound_statement : LCURL statements RCURL",
"compound_statement : LCURL RCURL",
"var_declaration : type_specifier declaration_list SEMICOLON",
"type_specifier : INT",
"type_specifier : FLOAT",
"type_specifier : VOID",
"declaration_list : declaration_list COMMA ID",
"declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD",
"declaration_list : ID",
"declaration_list : ID LTHIRD CONST_INT RTHIRD",
"statements : statement",
"statements : statements statement",
"statement : var_declaration",
"statement : expression_statement",
"statement : compound_statement",
"statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement",
"statement : IF LPAREN expression RPAREN statement",
"statement : IF LPAREN expression RPAREN statement ELSE statement",
"statement : WHILE LPAREN expression RPAREN statement",
"statement : PRINTLN LPAREN ID RPAREN SEMICOLON",
"statement : RETURN expression SEMICOLON",
"expression_statement : SEMICOLON",
"expression_statement : expression SEMICOLON",
"variable : ID",
"variable : ID LTHIRD expression RTHIRD",
"expression : logic_expression",
"expression : variable ASSIGNOP logic_expression",
"logic_expression : rel_expression",
"logic_expression : rel_expression LOGICOP rel_expression",
"rel_expression : simple_expression",
"rel_expression : simple_expression RELOP simple_expression",
"simple_expression : term",
"simple_expression : simple_expression ADDOP term",
"term : unary_expression",
"term : term MULOP unary_expression",
"unary_expression : ADDOP unary_expression",
"unary_expression : NOT unary_expression",
"unary_expression : factor",
"factor : variable",
"factor : ID LPAREN argument_list RPAREN",
"factor : LPAREN expression RPAREN",
"factor : CONST_INT",
"factor : CONST_FLOAT",
"factor : variable INCOP",
"factor : variable DECOP",
"argument_list : arguments",
"arguments : arguments COMMA logic_expression",
"arguments : logic_expression",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
YYLTYPE  yyloc; /* position returned by actions */
YYLTYPE  yylloc; /* position from the lexer */
#endif

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
#ifndef YYLLOC_DEFAULT
#define YYLLOC_DEFAULT(loc, rhs, n) \
do \
{ \
    if (n == 0) \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 0).last_line; \
        (loc).first_column = YYRHSLOC(rhs, 0).last_column; \
        (loc).last_line    = YYRHSLOC(rhs, 0).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, 0).last_column; \
    } \
    else \
    { \
        (loc).first_line   = YYRHSLOC(rhs, 1).first_line; \
        (loc).first_column = YYRHSLOC(rhs, 1).first_column; \
        (loc).last_line    = YYRHSLOC(rhs, n).last_line; \
        (loc).last_column  = YYRHSLOC(rhs, n).last_column; \
    } \
} while (0)
#endif /* YYLLOC_DEFAULT */
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#if YYBTYACC

#ifndef YYLVQUEUEGROWTH
#define YYLVQUEUEGROWTH 32
#endif
#endif /* YYBTYACC */

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#ifndef YYINITSTACKSIZE
#define YYINITSTACKSIZE 200
#endif

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  *p_base;
    YYLTYPE  *p_mark;
#endif
} YYSTACKDATA;
#if YYBTYACC

struct YYParseState_s
{
    struct YYParseState_s *save;    /* Previously saved parser state */
    YYSTACKDATA            yystack; /* saved parser stack */
    int                    state;   /* saved parser state */
    int                    errflag; /* saved error recovery status */
    int                    lexeme;  /* saved index of the conflict lexeme in the lexical queue */
    YYINT                  ctry;    /* saved index in yyctable[] for this conflict */
};
typedef struct YYParseState_s YYParseState;
#endif /* YYBTYACC */
/* variables for the parser stack */
static YYSTACKDATA yystack;
#if YYBTYACC

/* Current parser state */
static YYParseState *yyps = 0;

/* yypath != NULL: do the full parse, starting at *yypath parser state. */
static YYParseState *yypath = 0;

/* Base of the lexical value queue */
static YYSTYPE *yylvals = 0;

/* Current position at lexical value queue */
static YYSTYPE *yylvp = 0;

/* End position of lexical value queue */
static YYSTYPE *yylve = 0;

/* The last allocated position at the lexical value queue */
static YYSTYPE *yylvlim = 0;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
/* Base of the lexical position queue */
static YYLTYPE *yylpsns = 0;

/* Current position at lexical position queue */
static YYLTYPE *yylpp = 0;

/* End position of lexical position queue */
static YYLTYPE *yylpe = 0;

/* The last allocated position at the lexical position queue */
static YYLTYPE *yylplim = 0;
#endif

/* Current position at lexical token queue */
static YYINT  *yylexp = 0;

static YYINT  *yylexemes = 0;
#endif /* YYBTYACC */
#line 1028 "b.y"


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

#line 855 "y.tab.c"

/* For use in generated program */
#define yydepth (int)(yystack.s_mark - yystack.s_base)
#if YYBTYACC
#define yytrial (yyps->save)
#endif /* YYBTYACC */

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE *newps;
#endif

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    newps = (YYLTYPE *)realloc(data->p_base, newsize * sizeof(*newps));
    if (newps == 0)
        return YYENOMEM;

    data->p_base = newps;
    data->p_mark = newps + i;
#endif

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;

#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%sdebug: stack size increased to %d\n", YYPREFIX, newsize);
#endif
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    free(data->p_base);
#endif
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif /* YYPURE || defined(YY_NO_LEAKS) */
#if YYBTYACC

static YYParseState *
yyNewState(unsigned size)
{
    YYParseState *p = (YYParseState *) malloc(sizeof(YYParseState));
    if (p == NULL) return NULL;

    p->yystack.stacksize = size;
    if (size == 0)
    {
        p->yystack.s_base = NULL;
        p->yystack.l_base = NULL;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        p->yystack.p_base = NULL;
#endif
        return p;
    }
    p->yystack.s_base    = (YYINT *) malloc(size * sizeof(YYINT));
    if (p->yystack.s_base == NULL) return NULL;
    p->yystack.l_base    = (YYSTYPE *) malloc(size * sizeof(YYSTYPE));
    if (p->yystack.l_base == NULL) return NULL;
    memset(p->yystack.l_base, 0, size * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    p->yystack.p_base    = (YYLTYPE *) malloc(size * sizeof(YYLTYPE));
    if (p->yystack.p_base == NULL) return NULL;
    memset(p->yystack.p_base, 0, size * sizeof(YYLTYPE));
#endif

    return p;
}

static void
yyFreeState(YYParseState *p)
{
    yyfreestack(&p->yystack);
    free(p);
}
#endif /* YYBTYACC */

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab
#if YYBTYACC
#define YYVALID        do { if (yyps->save)            goto yyvalid; } while(0)
#define YYVALID_NESTED do { if (yyps->save && \
                                yyps->save->save == 0) goto yyvalid; } while(0)
#endif /* YYBTYACC */

int
YYPARSE_DECL()
{
    int yym, yyn, yystate, yyresult;
#if YYBTYACC
    int yynewerrflag;
    YYParseState *yyerrctx = NULL;
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    YYLTYPE  yyerror_loc_range[3]; /* position of error start/end (0 unused) */
#endif
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
    if (yydebug)
        fprintf(stderr, "%sdebug[<# of symbols on state stack>]\n", YYPREFIX);
#endif
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    memset(yyerror_loc_range, 0, sizeof(yyerror_loc_range));
#endif

#if YYBTYACC
    yyps = yyNewState(0); if (yyps == 0) goto yyenomem;
    yyps->save = 0;
#endif /* YYBTYACC */
    yym = 0;
    /* yyn is set below */
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base;
#endif
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
#if YYBTYACC
        do {
        if (yylvp < yylve)
        {
            /* we're currently re-reading tokens */
            yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc = *yylpp++;
#endif
            yychar = *yylexp++;
            break;
        }
        if (yyps->save)
        {
            /* in trial mode; save scanner results for future parse attempts */
            if (yylvp == yylvlim)
            {   /* Enlarge lexical value queue */
                size_t p = (size_t) (yylvp - yylvals);
                size_t s = (size_t) (yylvlim - yylvals);

                s += YYLVQUEUEGROWTH;
                if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL) goto yyenomem;
                if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL) goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL) goto yyenomem;
#endif
                yylvp   = yylve = yylvals + p;
                yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp   = yylpe = yylpsns + p;
                yylplim = yylpsns + s;
#endif
                yylexp  = yylexemes + p;
            }
            *yylexp = (YYINT) YYLEX;
            *yylvp++ = yylval;
            yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *yylpp++ = yylloc;
            yylpe++;
#endif
            yychar = *yylexp++;
            break;
        }
        /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
        yychar = YYLEX;
#if YYBTYACC
        } while (0);
#endif /* YYBTYACC */
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, " <%s>", YYSTYPE_TOSTRING(yychar, yylval));
#endif
            fputc('\n', stderr);
        }
#endif
    }
#if YYBTYACC

    /* Do we have a conflict? */
    if (((yyn = yycindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
        yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        YYINT ctry;

        if (yypath)
        {
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: CONFLICT in state %d: following successful trial parse\n",
                                YYDEBUGSTR, yydepth, yystate);
#endif
            /* Switch to the next conflict context */
            save = yypath;
            yypath = save->save;
            save->save = NULL;
            ctry = save->ctry;
            if (save->state != yystate) YYABORT;
            yyFreeState(save);

        }
        else
        {

            /* Unresolved conflict - start/continue trial parse */
            YYParseState *save;
#if YYDEBUG
            if (yydebug)
            {
                fprintf(stderr, "%s[%d]: CONFLICT in state %d. ", YYDEBUGSTR, yydepth, yystate);
                if (yyps->save)
                    fputs("ALREADY in conflict, continuing trial parse.\n", stderr);
                else
                    fputs("Starting trial parse.\n", stderr);
            }
#endif
            save                  = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (save == NULL) goto yyenomem;
            save->save            = yyps->save;
            save->state           = yystate;
            save->errflag         = yyerrflag;
            save->yystack.s_mark  = save->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (save->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            save->yystack.l_mark  = save->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (save->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            save->yystack.p_mark  = save->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (save->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            ctry                  = yytable[yyn];
            if (yyctable[ctry] == -1)
            {
#if YYDEBUG
                if (yydebug && yychar >= YYEOF)
                    fprintf(stderr, "%s[%d]: backtracking 1 token\n", YYDEBUGSTR, yydepth);
#endif
                ctry++;
            }
            save->ctry = ctry;
            if (yyps->save == NULL)
            {
                /* If this is a first conflict in the stack, start saving lexemes */
                if (!yylexemes)
                {
                    yylexemes = (YYINT *) malloc((YYLVQUEUEGROWTH) * sizeof(YYINT));
                    if (yylexemes == NULL) goto yyenomem;
                    yylvals   = (YYSTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYSTYPE));
                    if (yylvals == NULL) goto yyenomem;
                    yylvlim   = yylvals + YYLVQUEUEGROWTH;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpsns   = (YYLTYPE *) malloc((YYLVQUEUEGROWTH) * sizeof(YYLTYPE));
                    if (yylpsns == NULL) goto yyenomem;
                    yylplim   = yylpsns + YYLVQUEUEGROWTH;
#endif
                }
                if (yylvp == yylve)
                {
                    yylvp  = yylve = yylvals;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp  = yylpe = yylpsns;
#endif
                    yylexp = yylexemes;
                    if (yychar >= YYEOF)
                    {
                        *yylve++ = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                        *yylpe++ = yylloc;
#endif
                        *yylexp  = (YYINT) yychar;
                        yychar   = YYEMPTY;
                    }
                }
            }
            if (yychar >= YYEOF)
            {
                yylvp--;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp--;
#endif
                yylexp--;
                yychar = YYEMPTY;
            }
            save->lexeme = (int) (yylvp - yylvals);
            yyps->save   = save;
        }
        if (yytable[yyn] == ctry)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                                YYDEBUGSTR, yydepth, yystate, yyctable[ctry]);
#endif
            if (yychar < 0)
            {
                yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylpp++;
#endif
                yylexp++;
            }
            if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                goto yyoverflow;
            yystate = yyctable[ctry];
            *++yystack.s_mark = (YYINT) yystate;
            *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            *++yystack.p_mark = yylloc;
#endif
            yychar  = YYEMPTY;
            if (yyerrflag > 0) --yyerrflag;
            goto yyloop;
        }
        else
        {
            yyn = yyctable[ctry];
            goto yyreduce;
        }
    } /* End of code dealing with conflicts */
#endif /* YYBTYACC */
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: state %d, shifting to state %d\n",
                            YYDEBUGSTR, yydepth, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yylloc;
#endif
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;
#if YYBTYACC

    yynewerrflag = 1;
    goto yyerrhandler;
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */

yyerrlab:
    /* explicit YYERROR from an action -- pop the rhs of the rule reduced
     * before looking for error recovery */
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif

    yynewerrflag = 0;
yyerrhandler:
    while (yyps->save)
    {
        int ctry;
        YYParseState *save = yyps->save;
#if YYDEBUG
        if (yydebug)
            fprintf(stderr, "%s[%d]: ERROR in state %d, CONFLICT BACKTRACKING to state %d, %d tokens\n",
                            YYDEBUGSTR, yydepth, yystate, yyps->save->state,
                    (int)(yylvp - yylvals - yyps->save->lexeme));
#endif
        /* Memorize most forward-looking error state in case it's really an error. */
        if (yyerrctx == NULL || yyerrctx->lexeme < yylvp - yylvals)
        {
            /* Free old saved error context state */
            if (yyerrctx) yyFreeState(yyerrctx);
            /* Create and fill out new saved error context state */
            yyerrctx                 = yyNewState((unsigned)(yystack.s_mark - yystack.s_base + 1));
            if (yyerrctx == NULL) goto yyenomem;
            yyerrctx->save           = yyps->save;
            yyerrctx->state          = yystate;
            yyerrctx->errflag        = yyerrflag;
            yyerrctx->yystack.s_mark = yyerrctx->yystack.s_base + (yystack.s_mark - yystack.s_base);
            memcpy (yyerrctx->yystack.s_base, yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yyerrctx->yystack.l_mark = yyerrctx->yystack.l_base + (yystack.l_mark - yystack.l_base);
            memcpy (yyerrctx->yystack.l_base, yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yyerrctx->yystack.p_mark = yyerrctx->yystack.p_base + (yystack.p_mark - yystack.p_base);
            memcpy (yyerrctx->yystack.p_base, yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yyerrctx->lexeme         = (int) (yylvp - yylvals);
        }
        yylvp          = yylvals   + save->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yylpp          = yylpsns   + save->lexeme;
#endif
        yylexp         = yylexemes + save->lexeme;
        yychar         = YYEMPTY;
        yystack.s_mark = yystack.s_base + (save->yystack.s_mark - save->yystack.s_base);
        memcpy (yystack.s_base, save->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
        yystack.l_mark = yystack.l_base + (save->yystack.l_mark - save->yystack.l_base);
        memcpy (yystack.l_base, save->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        yystack.p_mark = yystack.p_base + (save->yystack.p_mark - save->yystack.p_base);
        memcpy (yystack.p_base, save->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
        ctry           = ++save->ctry;
        yystate        = save->state;
        /* We tried shift, try reduce now */
        if ((yyn = yyctable[ctry]) >= 0) goto yyreduce;
        yyps->save     = save->save;
        save->save     = NULL;
        yyFreeState(save);

        /* Nothing left on the stack -- error */
        if (!yyps->save)
        {
#if YYDEBUG
            if (yydebug)
                fprintf(stderr, "%sdebug[%d,trial]: trial parse FAILED, entering ERROR mode\n",
                                YYPREFIX, yydepth);
#endif
            /* Restore state as it was in the most forward-advanced error */
            yylvp          = yylvals   + yyerrctx->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylpp          = yylpsns   + yyerrctx->lexeme;
#endif
            yylexp         = yylexemes + yyerrctx->lexeme;
            yychar         = yylexp[-1];
            yylval         = yylvp[-1];
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yylloc         = yylpp[-1];
#endif
            yystack.s_mark = yystack.s_base + (yyerrctx->yystack.s_mark - yyerrctx->yystack.s_base);
            memcpy (yystack.s_base, yyerrctx->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
            yystack.l_mark = yystack.l_base + (yyerrctx->yystack.l_mark - yyerrctx->yystack.l_base);
            memcpy (yystack.l_base, yyerrctx->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            yystack.p_mark = yystack.p_base + (yyerrctx->yystack.p_mark - yyerrctx->yystack.p_base);
            memcpy (yystack.p_base, yyerrctx->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
            yystate        = yyerrctx->state;
            yyFreeState(yyerrctx);
            yyerrctx       = NULL;
        }
        yynewerrflag = 1;
    }
    if (yynewerrflag == 0) goto yyinrecovery;
#endif /* YYBTYACC */

    YYERROR_CALL("syntax error");
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yyerror_loc_range[1] = yylloc; /* lookahead position is error start position */
#endif

#if !YYBTYACC
    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
#endif
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: state %d, error recovery shifting to state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* lookahead position is error end position */
                yyerror_loc_range[2] = yylloc;
                YYLLOC_DEFAULT(yyloc, yyerror_loc_range, 2); /* position of error span */
                *++yystack.p_mark = yyloc;
#endif
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    fprintf(stderr, "%s[%d]: error recovery discarding state %d\n",
                                    YYDEBUGSTR, yydepth, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                /* the current TOS position is the error start position */
                yyerror_loc_range[1] = *yystack.p_mark;
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
                if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark, yystack.p_mark);
#else
                    YYDESTRUCT_CALL("error: discarding state",
                                    yystos[*yystack.s_mark], yystack.l_mark);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
                --yystack.s_mark;
                --yystack.l_mark;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                --yystack.p_mark;
#endif
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            fprintf(stderr, "%s[%d]: state %d, error recovery discarding token %d (%s)\n",
                            YYDEBUGSTR, yydepth, yystate, yychar, yys);
        }
#endif
#if defined(YYDESTRUCT_CALL)
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval, &yylloc);
#else
            YYDESTRUCT_CALL("error: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
#endif /* defined(YYDESTRUCT_CALL) */
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
    yym = yylen[yyn];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: state %d, reducing by rule %d (%s)",
                        YYDEBUGSTR, yydepth, yystate, yyn, yyrule[yyn]);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            if (yym > 0)
            {
                int i;
                fputc('<', stderr);
                for (i = yym; i > 0; i--)
                {
                    if (i != yym) fputs(", ", stderr);
                    fputs(YYSTYPE_TOSTRING(yystos[yystack.s_mark[1-i]],
                                           yystack.l_mark[1-i]), stderr);
                }
                fputc('>', stderr);
            }
#endif
        fputc('\n', stderr);
    }
#endif
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)

    /* Perform position reduction */
    memset(&yyloc, 0, sizeof(yyloc));
#if YYBTYACC
    if (!yytrial)
#endif /* YYBTYACC */
    {
        YYLLOC_DEFAULT(yyloc, &yystack.p_mark[-yym], yym);
        /* just in case YYERROR is invoked within the action, save
           the start of the rhs as the error start position */
        yyerror_loc_range[1] = yystack.p_mark[1-yym];
    }
#endif

    switch (yyn)
    {
case 1:
#line 339 "b.y"
	{
		log_writer.write_grammar_rule("start : program ");
		yyval.info = new SymbolInfo("program","start");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
		tree_writer.write_parse_tree(yyval.info->get_parse_tree_node());
	}
#line 1534 "y.tab.c"
break;
case 2:
#line 349 "b.y"
	{	
		log_writer.write_grammar_rule("program : program unit ");
		yyval.info = new SymbolInfo("program unit","program");
		yyval.info->set_line_counts(yystack.l_mark[-1].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(2,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 1544 "y.tab.c"
break;
case 3:
#line 356 "b.y"
	{
		log_writer.write_grammar_rule("program : unit ");
		yyval.info = new SymbolInfo("unit","program");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 1554 "y.tab.c"
break;
case 4:
#line 365 "b.y"
	{
		log_writer.write_grammar_rule("unit : var_declaration  ");
		yyval.info = new SymbolInfo("var_declaration","unit");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 1564 "y.tab.c"
break;
case 5:
#line 372 "b.y"
	{
		log_writer.write_grammar_rule("unit : func_declaration ");
		yyval.info = new SymbolInfo("func_declaration","unit");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 1574 "y.tab.c"
break;
case 6:
#line 379 "b.y"
	{
		log_writer.write_grammar_rule("unit : func_definition  ");
		yyval.info = new SymbolInfo("func_definition","unit");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 1584 "y.tab.c"
break;
case 7:
#line 388 "b.y"
	{
		log_writer.write_grammar_rule("func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON ");
		yyval.info = new SymbolInfo("type_specifier ID LPAREN parameter_list RPAREN SEMICOLON","func_declaration");
		yyval.info->set_line_counts(yystack.l_mark[-5].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(6,yystack.l_mark[-5].info,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		debug_child_identifiers(yystack.l_mark[-2].info,string("func_declaration"));

		/* At first, processing the parameters*/

		vector<string> parameter_types = process_parameters(yystack.l_mark[-2].info);

		/* Now processing the function itself*/

		string function_name = yystack.l_mark[-4].info->get_name();
		string return_type = yystack.l_mark[-5].info->get_name();
		int line_number = yystack.l_mark[-4].info->get_start_line_count();

		handle_function_declaration(line_number, function_name, return_type, parameter_types);
	}
#line 1608 "y.tab.c"
break;
case 8:
#line 409 "b.y"
	{
		log_writer.write_grammar_rule("func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON ");
		yyval.info = new SymbolInfo("type_specifier ID LPAREN RPAREN SEMICOLON","func_declaration");
		yyval.info->set_line_counts(yystack.l_mark[-4].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(5,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		/* At first, processing the parameters*/

		vector<string> parameter_types;  /* empty */

		/* Now processing the function itself*/

		string function_name = yystack.l_mark[-3].info->get_name();
		string return_type = yystack.l_mark[-4].info->get_name();
		int line_number = yystack.l_mark[-3].info->get_start_line_count();

		handle_function_declaration(line_number, function_name, return_type, parameter_types);
	}
#line 1630 "y.tab.c"
break;
case 9:
#line 430 "b.y"
	{
		/* At first, processing the parameters*/

		vector<string> parameter_types = process_parameters(yystack.l_mark[-1].info);

		/* Now processing the function itself*/

		string function_name = yystack.l_mark[-3].info->get_name();
		string return_type = yystack.l_mark[-4].info->get_name();
		int line_number = yystack.l_mark[-3].info->get_start_line_count();

		handle_function_definition(line_number, function_name, return_type, parameter_types);

		ST->enter_scope();
		for(auto child: yystack.l_mark[-1].info->get_child_identifiers())
		{
			ST->insert_into_current_scope(child->get_name(), child->get_evaluated_type());

			/* not needed as function parameter is never array*/
			/* write_ARRAY_to_SymbolInfo(ST->lookup(child->get_name()));*/
		}

		exit_scope_pending = 1;  /* this scope must be exited on finding RCURL*/
	}
#line 1658 "y.tab.c"
break;
case 10:
#line 455 "b.y"
	{
		log_writer.write_grammar_rule("func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement ");
		yyval.info = new SymbolInfo("type_specifier ID LPAREN parameter_list RPAREN compound_statement","func_definition");
		yyval.info->set_line_counts(yystack.l_mark[-6].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(6,yystack.l_mark[-6].info,yystack.l_mark[-5].info,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[0].info);

		debug_child_identifiers(yystack.l_mark[-3].info,string("func_definition"));
	}
#line 1670 "y.tab.c"
break;
case 11:
#line 464 "b.y"
	{
		/* At first, processing the parameters*/

		vector<string> parameter_types;   /* empty list*/

		/* Now processing the function itself*/

		string function_name = yystack.l_mark[-2].info->get_name();
		string return_type = yystack.l_mark[-3].info->get_name();
		int line_number = yystack.l_mark[-2].info->get_start_line_count();

		handle_function_definition(line_number, function_name, return_type, parameter_types);

		ST->enter_scope();
		for(auto child: yystack.l_mark[0].info->get_child_identifiers())
		{
			ST->insert_into_current_scope(child->get_name(), child->get_evaluated_type());

			/* not needed as function parameter is never array*/
			/* write_ARRAY_to_SymbolInfo(ST->lookup(child->get_name()));*/
		}

		exit_scope_pending = 1;  /* this scope must be exited on finding RCURL*/
	}
#line 1698 "y.tab.c"
break;
case 12:
#line 489 "b.y"
	{
		log_writer.write_grammar_rule("func_definition : type_specifier ID LPAREN RPAREN compound_statement");
		yyval.info = new SymbolInfo("type_specifier ID LPAREN RPAREN compound_statement","func_definition");
		yyval.info->set_line_counts(yystack.l_mark[-5].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(5,yystack.l_mark[-5].info,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[0].info);
	}
#line 1708 "y.tab.c"
break;
case 13:
#line 499 "b.y"
	{
		log_writer.write_grammar_rule("parameter_list  : parameter_list COMMA type_specifier ID");
		yyval.info = new SymbolInfo("parameter_list COMMA type_specifier ID","parameter_list");
		yyval.info->set_line_counts(yystack.l_mark[-3].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(4,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		yystack.l_mark[0].info->set_evaluated_type(yystack.l_mark[-1].info->get_name());

		yyval.info->set_child_identifiers(yystack.l_mark[-3].info->get_child_identifiers());
		yyval.info->add_child_identifier(yystack.l_mark[0].info);	

	}
#line 1724 "y.tab.c"
break;
case 14:
#line 512 "b.y"
	{
		log_writer.write_grammar_rule("parameter_list  : parameter_list COMMA type_specifier ");
		yyval.info = new SymbolInfo("parameter_list COMMA type_specifier","parameter_list");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		yyval.info->set_child_identifiers(yystack.l_mark[-2].info->get_child_identifiers());
		yyval.info->add_child_identifier(yystack.l_mark[0].info);
	}
#line 1737 "y.tab.c"
break;
case 15:
#line 522 "b.y"
	{
		log_writer.write_grammar_rule("parameter_list  : type_specifier ID");
		yyval.info = new SymbolInfo("type_specifier ID","parameter_list");
		yyval.info->set_line_counts(yystack.l_mark[-1].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(2,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		yystack.l_mark[0].info->set_evaluated_type(yystack.l_mark[-1].info->get_name());

		yyval.info->add_child_identifier(yystack.l_mark[0].info);
	}
#line 1751 "y.tab.c"
break;
case 16:
#line 533 "b.y"
	{
		log_writer.write_grammar_rule("parameter_list  : type_specifier ");
		yyval.info = new SymbolInfo("type_specifier","parameter_list");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);

		/* here, the type will be 'type_specifier' and the name will be INT, FLOAT etc*/
		/* this means, this parameter was declared unnamed*/
		/* type can be retrieved from evaluated_type*/
		/* this will be handled while building the SymbolInfo for function*/
		yyval.info->add_child_identifier(yystack.l_mark[0].info);  
	}
#line 1767 "y.tab.c"
break;
case 17:
#line 549 "b.y"
	{
		log_writer.write_grammar_rule("compound_statement : LCURL statements RCURL  ");
		yyval.info = new SymbolInfo("LCURL statements RCURL","compound_statement");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		ST->print_all_scope_tables();

		if(exit_scope_pending)
		{
			ST->exit_scope();
			exit_scope_pending = 0;
		}
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
#line 1805 "y.tab.c"
break;
case 18:
#line 584 "b.y"
	{
		log_writer.write_grammar_rule("compound_statement : LCURL RCURL  ");
		yyval.info = new SymbolInfo("LCURL RCURL","compound_statement");
		yyval.info->set_line_counts(yystack.l_mark[-1].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(2,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		if(exit_scope_pending)
		{
			ST->exit_scope();
		}
	}
#line 1820 "y.tab.c"
break;
case 19:
#line 598 "b.y"
	{
		debug_child_identifiers(yystack.l_mark[-1].info, "var_declaration");

		log_writer.write_grammar_rule("var_declaration : type_specifier declaration_list SEMICOLON  ");
		yyval.info = new SymbolInfo("type_specifier declaration_list SEMICOLON","var_declaration");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		string type = yystack.l_mark[-2].info->get_name();

		if(type == "VOID")
		{
			for(auto child: yystack.l_mark[-1].info->get_child_identifiers())
			{
				child->set_evaluated_type(type);
				int line_number = child->get_start_line_count();
				error_writer.write_error(line_number, string("Variable or field ") + quote + child->get_name() + quote + string(" declared void"));
			}
		}
		else
		{
			for(auto child: yystack.l_mark[-1].info->get_child_identifiers())
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
						if(e->get_evaluated_type()==type && e->check_property(IS_ARRAY)==c->check_property(IS_ARRAY) && e->arr_size==c->arr_size)
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
#line 1887 "y.tab.c"
break;
case 20:
#line 664 "b.y"
	{
		log_writer.write_grammar_rule("type_specifier	: INT ");
		yyval.info = new SymbolInfo("INT","type_specifier");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
		yyval.info->set_evaluated_type("INT");      /* this is needed for nameless parameters*/
	}
#line 1898 "y.tab.c"
break;
case 21:
#line 672 "b.y"
	{
		log_writer.write_grammar_rule("type_specifier	: FLOAT ");
		yyval.info = new SymbolInfo("FLOAT","type_specifier");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
		yyval.info->set_evaluated_type("FLOAT");	/* this is needed for nameless parameters*/
	}
#line 1909 "y.tab.c"
break;
case 22:
#line 680 "b.y"
	{
		log_writer.write_grammar_rule("type_specifier	: VOID");
		yyval.info = new SymbolInfo("VOID","type_specifier");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
		yyval.info->set_evaluated_type("VOID");		/* this is needed for nameless parameters*/
	}
#line 1920 "y.tab.c"
break;
case 23:
#line 690 "b.y"
	{
		log_writer.write_grammar_rule("declaration_list : declaration_list COMMA ID  ");
		yyval.info = new SymbolInfo("declaration_list COMMA ID","declaration_list");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		yyval.info->set_child_identifiers(yystack.l_mark[-2].info->get_child_identifiers());
		yyval.info->add_child_identifier(yystack.l_mark[0].info);
	}
#line 1933 "y.tab.c"
break;
case 24:
#line 700 "b.y"
	{
		log_writer.write_grammar_rule("declaration_list : declaration_list COMMA ID LSQUARE CONST_INT RSQUARE ");
		yyval.info = new SymbolInfo("declaration_list COMMA ID LSQUARE CONST_INT RSQUARE","declaration_list");
		yyval.info->set_line_counts(yystack.l_mark[-5].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(6,yystack.l_mark[-5].info,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		yystack.l_mark[-3].info->set_property(IS_ARRAY);
		yystack.l_mark[-3].info->set_arr_size(stoi(yystack.l_mark[-1].info->get_name()));

		yyval.info->set_child_identifiers(yystack.l_mark[-5].info->get_child_identifiers());
		yyval.info->add_child_identifier(yystack.l_mark[-3].info); /* IS_ARRAY*/
	}
#line 1949 "y.tab.c"
break;
case 25:
#line 713 "b.y"
	{
		log_writer.write_grammar_rule("declaration_list : ID ");
		yyval.info = new SymbolInfo("ID","declaration_list");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);

		yyval.info->add_child_identifier(yystack.l_mark[0].info);
	}
#line 1961 "y.tab.c"
break;
case 26:
#line 722 "b.y"
	{
		log_writer.write_grammar_rule("declaration_list : ID LSQUARE CONST_INT RSQUARE ");
		yyval.info = new SymbolInfo("ID LSQUARE CONST_INT RSQUARE","declaration_list");
		yyval.info->set_line_counts(yystack.l_mark[-3].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(4,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);

		yystack.l_mark[-3].info->set_property(IS_ARRAY);
		yystack.l_mark[-3].info->set_arr_size(stoi(yystack.l_mark[-1].info->get_name()));

		yyval.info->add_child_identifier(yystack.l_mark[-3].info);
	}
#line 1976 "y.tab.c"
break;
case 27:
#line 736 "b.y"
	{
		log_writer.write_grammar_rule("statements : statement  ");
		yyval.info = new SymbolInfo("statement","statements");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 1986 "y.tab.c"
break;
case 28:
#line 743 "b.y"
	{
		log_writer.write_grammar_rule("statements : statements statement  ");
		yyval.info = new SymbolInfo("statements statement","statements");
		yyval.info->set_line_counts(yystack.l_mark[-1].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(2,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 1996 "y.tab.c"
break;
case 29:
#line 752 "b.y"
	{
		log_writer.write_grammar_rule("statement : var_declaration ");
		yyval.info = new SymbolInfo("var_declaration","statement");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2006 "y.tab.c"
break;
case 30:
#line 759 "b.y"
	{
		log_writer.write_grammar_rule("statement : expression_statement  ");
		yyval.info = new SymbolInfo("expression_statement","statement");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2016 "y.tab.c"
break;
case 31:
#line 766 "b.y"
	{
		log_writer.write_grammar_rule("statement : compound_statement ");
		yyval.info = new SymbolInfo("compound_statement","statement");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2026 "y.tab.c"
break;
case 32:
#line 773 "b.y"
	{
		log_writer.write_grammar_rule_default_style("statement","FOR LPAREN expression_statement expression_statement expression RPAREN statement");
		yyval.info = new SymbolInfo("FOR LPAREN expression_statement expression_statement expression RPAREN statement","statement");
		yyval.info->set_line_counts(yystack.l_mark[-6].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(7,yystack.l_mark[-6].info,yystack.l_mark[-5].info,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2036 "y.tab.c"
break;
case 33:
#line 780 "b.y"
	{
		log_writer.write_grammar_rule_default_style("statement","IF LPAREN expression RPAREN statement");
		yyval.info = new SymbolInfo("IF LPAREN expression RPAREN statement","statement");
		yyval.info->set_line_counts(yystack.l_mark[-4].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(5,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2046 "y.tab.c"
break;
case 34:
#line 787 "b.y"
	{
		log_writer.write_grammar_rule_default_style("statement","IF LPAREN expression RPAREN statement ELSE statement");
		yyval.info = new SymbolInfo("IF LPAREN expression RPAREN statement ELSE statement","statement");
		yyval.info->set_line_counts(yystack.l_mark[-6].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(7,yystack.l_mark[-6].info,yystack.l_mark[-5].info,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2056 "y.tab.c"
break;
case 35:
#line 794 "b.y"
	{
		log_writer.write_grammar_rule_default_style("statement","WHILE LPAREN expression RPAREN statement");
		yyval.info = new SymbolInfo("WHILE LPAREN expression RPAREN statement","statement");
		yyval.info->set_line_counts(yystack.l_mark[-4].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(5,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2066 "y.tab.c"
break;
case 36:
#line 801 "b.y"
	{
		log_writer.write_grammar_rule_default_style("statement","WHILE LPAREN expression RPAREN statement");
		yyval.info = new SymbolInfo("PRINTLN LPAREN ID RPAREN SEMICOLON","statement");
		yyval.info->set_line_counts(yystack.l_mark[-4].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(5,yystack.l_mark[-4].info,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2076 "y.tab.c"
break;
case 37:
#line 808 "b.y"
	{
		log_writer.write_grammar_rule("statement : RETURN expression SEMICOLON");
		yyval.info = new SymbolInfo("RETURN expression SEMICOLON","statement");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2086 "y.tab.c"
break;
case 38:
#line 817 "b.y"
	{
		log_writer.write_grammar_rule("expression_statement : SEMICOLON ");
		yyval.info = new SymbolInfo("SEMICOLON","expression_statement");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2096 "y.tab.c"
break;
case 39:
#line 824 "b.y"
	{
		log_writer.write_grammar_rule("expression_statement : expression SEMICOLON 		 ");
		yyval.info = new SymbolInfo("expression SEMICOLON","expression_statement");
		yyval.info->set_line_counts(yystack.l_mark[-1].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(2,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2106 "y.tab.c"
break;
case 40:
#line 833 "b.y"
	{
		log_writer.write_grammar_rule("variable : ID 	 ");
		yyval.info = new SymbolInfo("ID","variable");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2116 "y.tab.c"
break;
case 41:
#line 840 "b.y"
	{
		log_writer.write_grammar_rule("variable : ID LSQUARE expression RSQUARE  	 ");
		yyval.info = new SymbolInfo("ID LSQUARE expression RSQUARE","variable");
		yyval.info->set_line_counts(yystack.l_mark[-3].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(4,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2126 "y.tab.c"
break;
case 42:
#line 849 "b.y"
	{
		log_writer.write_grammar_rule("expression 	: logic_expression	 ");
		yyval.info = new SymbolInfo("logic_expression","expression");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2136 "y.tab.c"
break;
case 43:
#line 856 "b.y"
	{
		log_writer.write_grammar_rule("expression 	: variable ASSIGNOP logic_expression 		 ");
		yyval.info = new SymbolInfo("variable ASSIGNOP logic_expression","expression");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2146 "y.tab.c"
break;
case 44:
#line 865 "b.y"
	{
		log_writer.write_grammar_rule("logic_expression : rel_expression 	 ");
		yyval.info = new SymbolInfo("rel_expression","logic_expression");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2156 "y.tab.c"
break;
case 45:
#line 872 "b.y"
	{
		log_writer.write_grammar_rule("logic_expression : rel_expression LOGICOP rel_expression 	 	 ");
		yyval.info = new SymbolInfo("rel_expression LOGICOP rel_expression","logic_expression");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2166 "y.tab.c"
break;
case 46:
#line 881 "b.y"
	{
		log_writer.write_grammar_rule("rel_expression	: simple_expression ");
		yyval.info = new SymbolInfo("simple_expression","rel_expression");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2176 "y.tab.c"
break;
case 47:
#line 888 "b.y"
	{
		log_writer.write_grammar_rule("rel_expression	: simple_expression RELOP simple_expression	  ");
		yyval.info = new SymbolInfo("simple_expression RELOP simple_expression","rel_expression");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2186 "y.tab.c"
break;
case 48:
#line 897 "b.y"
	{
		log_writer.write_grammar_rule("simple_expression : term ");
		yyval.info = new SymbolInfo("term","simple_expression");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2196 "y.tab.c"
break;
case 49:
#line 904 "b.y"
	{
		log_writer.write_grammar_rule("simple_expression : simple_expression ADDOP term  ");
		yyval.info = new SymbolInfo("simple_expression ADDOP term","simple_expression");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2206 "y.tab.c"
break;
case 50:
#line 913 "b.y"
	{
		log_writer.write_grammar_rule("term :	unary_expression ");
		yyval.info = new SymbolInfo("unary_expression","term");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2216 "y.tab.c"
break;
case 51:
#line 920 "b.y"
	{
		log_writer.write_grammar_rule("term :	term MULOP unary_expression ");
		yyval.info = new SymbolInfo("term MULOP unary_expression","term");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2226 "y.tab.c"
break;
case 52:
#line 929 "b.y"
	{
		log_writer.write_grammar_rule_default_style("unary_expression","ADDOP unary_expression");
		yyval.info = new SymbolInfo("ADDOP unary_expression","unary_expression");
		yyval.info->set_line_counts(yystack.l_mark[-1].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(2,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2236 "y.tab.c"
break;
case 53:
#line 936 "b.y"
	{
		log_writer.write_grammar_rule_default_style("unary_expression","NOT unary_expression");
		yyval.info = new SymbolInfo("NOT unary_expression","unary_expression");
		yyval.info->set_line_counts(yystack.l_mark[-1].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(2,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2246 "y.tab.c"
break;
case 54:
#line 943 "b.y"
	{
		log_writer.write_grammar_rule("unary_expression : factor ");
		yyval.info = new SymbolInfo("factor","unary_expression");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2256 "y.tab.c"
break;
case 55:
#line 952 "b.y"
	{
		log_writer.write_grammar_rule("factor	: variable ");
		yyval.info = new SymbolInfo("variable","factor");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2266 "y.tab.c"
break;
case 56:
#line 959 "b.y"
	{
		log_writer.write_grammar_rule("factor	: ID LPAREN argument_list RPAREN  ");
		yyval.info = new SymbolInfo("ID LPAREN argument_list RPAREN","factor");
		yyval.info->set_line_counts(yystack.l_mark[-3].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(4,yystack.l_mark[-3].info,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2276 "y.tab.c"
break;
case 57:
#line 966 "b.y"
	{
		log_writer.write_grammar_rule("factor	: LPAREN expression RPAREN   ");
		yyval.info = new SymbolInfo("LPAREN expression RPAREN","factor");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2286 "y.tab.c"
break;
case 58:
#line 973 "b.y"
	{
		log_writer.write_grammar_rule("factor	: CONST_INT   ");
		yyval.info = new SymbolInfo("CONST_INT","factor");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2296 "y.tab.c"
break;
case 59:
#line 980 "b.y"
	{
		log_writer.write_grammar_rule("factor	: CONST_FLOAT   ");
		yyval.info = new SymbolInfo("CONST_FLOAT","factor");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2306 "y.tab.c"
break;
case 60:
#line 987 "b.y"
	{
		log_writer.write_grammar_rule_default_style("factor","variable INCOP");
		yyval.info = new SymbolInfo("variable INCOP","factor");
		yyval.info->set_line_counts(yystack.l_mark[-1].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(2,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2316 "y.tab.c"
break;
case 61:
#line 994 "b.y"
	{
		log_writer.write_grammar_rule_default_style("factor","variable DECOP");
		yyval.info = new SymbolInfo("variable DECOP","factor");
		yyval.info->set_line_counts(yystack.l_mark[-1].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(2,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2326 "y.tab.c"
break;
case 62:
#line 1003 "b.y"
	{
		log_writer.write_grammar_rule("argument_list : arguments  ");
		yyval.info = new SymbolInfo("arguments","argument_list");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2336 "y.tab.c"
break;
case 63:
#line 1012 "b.y"
	{
		log_writer.write_grammar_rule("arguments : arguments COMMA logic_expression ");
		yyval.info = new SymbolInfo("arguments COMMA logic_expression","arguments");
		yyval.info->set_line_counts(yystack.l_mark[-2].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(3,yystack.l_mark[-2].info,yystack.l_mark[-1].info,yystack.l_mark[0].info);
	}
#line 2346 "y.tab.c"
break;
case 64:
#line 1019 "b.y"
	{
		log_writer.write_grammar_rule("arguments : logic_expression");
		yyval.info = new SymbolInfo("logic_expression","arguments");
		yyval.info->set_line_counts(yystack.l_mark[0].info->get_start_line_count(), yystack.l_mark[0].info->get_end_line_count());
		yyval.info->set_children_tree_nodes(1,yystack.l_mark[0].info);
	}
#line 2356 "y.tab.c"
break;
#line 2358 "y.tab.c"
    default:
        break;
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark -= yym;
#endif
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
        {
            fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
            if (!yytrial)
#endif /* YYBTYACC */
                fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[YYFINAL], yyval));
#endif
            fprintf(stderr, "shifting from state 0 to final state %d\n", YYFINAL);
        }
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        *++yystack.p_mark = yyloc;
#endif
        if (yychar < 0)
        {
#if YYBTYACC
            do {
            if (yylvp < yylve)
            {
                /* we're currently re-reading tokens */
                yylval = *yylvp++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                yylloc = *yylpp++;
#endif
                yychar = *yylexp++;
                break;
            }
            if (yyps->save)
            {
                /* in trial mode; save scanner results for future parse attempts */
                if (yylvp == yylvlim)
                {   /* Enlarge lexical value queue */
                    size_t p = (size_t) (yylvp - yylvals);
                    size_t s = (size_t) (yylvlim - yylvals);

                    s += YYLVQUEUEGROWTH;
                    if ((yylexemes = (YYINT *)realloc(yylexemes, s * sizeof(YYINT))) == NULL)
                        goto yyenomem;
                    if ((yylvals   = (YYSTYPE *)realloc(yylvals, s * sizeof(YYSTYPE))) == NULL)
                        goto yyenomem;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    if ((yylpsns   = (YYLTYPE *)realloc(yylpsns, s * sizeof(YYLTYPE))) == NULL)
                        goto yyenomem;
#endif
                    yylvp   = yylve = yylvals + p;
                    yylvlim = yylvals + s;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                    yylpp   = yylpe = yylpsns + p;
                    yylplim = yylpsns + s;
#endif
                    yylexp  = yylexemes + p;
                }
                *yylexp = (YYINT) YYLEX;
                *yylvp++ = yylval;
                yylve++;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
                *yylpp++ = yylloc;
                yylpe++;
#endif
                yychar = *yylexp++;
                break;
            }
            /* normal operation, no conflict encountered */
#endif /* YYBTYACC */
            yychar = YYLEX;
#if YYBTYACC
            } while (0);
#endif /* YYBTYACC */
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                fprintf(stderr, "%s[%d]: state %d, reading token %d (%s)\n",
                                YYDEBUGSTR, yydepth, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
    {
        fprintf(stderr, "%s[%d]: after reduction, ", YYDEBUGSTR, yydepth);
#ifdef YYSTYPE_TOSTRING
#if YYBTYACC
        if (!yytrial)
#endif /* YYBTYACC */
            fprintf(stderr, "result is <%s>, ", YYSTYPE_TOSTRING(yystos[yystate], yyval));
#endif
        fprintf(stderr, "shifting from state %d to state %d\n", *yystack.s_mark, yystate);
    }
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    *++yystack.p_mark = yyloc;
#endif
    goto yyloop;
#if YYBTYACC

    /* Reduction declares that this path is valid. Set yypath and do a full parse */
yyvalid:
    if (yypath) YYABORT;
    while (yyps->save)
    {
        YYParseState *save = yyps->save;
        yyps->save = save->save;
        save->save = yypath;
        yypath = save;
    }
#if YYDEBUG
    if (yydebug)
        fprintf(stderr, "%s[%d]: state %d, CONFLICT trial successful, backtracking to state %d, %d tokens\n",
                        YYDEBUGSTR, yydepth, yystate, yypath->state, (int)(yylvp - yylvals - yypath->lexeme));
#endif
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    yylvp          = yylvals + yypath->lexeme;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yylpp          = yylpsns + yypath->lexeme;
#endif
    yylexp         = yylexemes + yypath->lexeme;
    yychar         = YYEMPTY;
    yystack.s_mark = yystack.s_base + (yypath->yystack.s_mark - yypath->yystack.s_base);
    memcpy (yystack.s_base, yypath->yystack.s_base, (size_t) (yystack.s_mark - yystack.s_base + 1) * sizeof(YYINT));
    yystack.l_mark = yystack.l_base + (yypath->yystack.l_mark - yypath->yystack.l_base);
    memcpy (yystack.l_base, yypath->yystack.l_base, (size_t) (yystack.l_mark - yystack.l_base + 1) * sizeof(YYSTYPE));
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
    yystack.p_mark = yystack.p_base + (yypath->yystack.p_mark - yypath->yystack.p_base);
    memcpy (yystack.p_base, yypath->yystack.p_base, (size_t) (yystack.p_mark - yystack.p_base + 1) * sizeof(YYLTYPE));
#endif
    yystate        = yypath->state;
    goto yyloop;
#endif /* YYBTYACC */

yyoverflow:
    YYERROR_CALL("yacc stack overflow");
#if YYBTYACC
    goto yyabort_nomem;
yyenomem:
    YYERROR_CALL("memory exhausted");
yyabort_nomem:
#endif /* YYBTYACC */
    yyresult = 2;
    goto yyreturn;

yyabort:
    yyresult = 1;
    goto yyreturn;

yyaccept:
#if YYBTYACC
    if (yyps->save) goto yyvalid;
#endif /* YYBTYACC */
    yyresult = 0;

yyreturn:
#if defined(YYDESTRUCT_CALL)
    if (yychar != YYEOF && yychar != YYEMPTY)
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval, &yylloc);
#else
        YYDESTRUCT_CALL("cleanup: discarding token", yychar, &yylval);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */

    {
        YYSTYPE *pv;
#if defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED)
        YYLTYPE *pp;

        for (pv = yystack.l_base, pp = yystack.p_base; pv <= yystack.l_mark; ++pv, ++pp)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv, pp);
#else
        for (pv = yystack.l_base; pv <= yystack.l_mark; ++pv)
             YYDESTRUCT_CALL("cleanup: discarding state",
                             yystos[*(yystack.s_base + (pv - yystack.l_base))], pv);
#endif /* defined(YYLTYPE) || defined(YYLTYPE_IS_DECLARED) */
    }
#endif /* defined(YYDESTRUCT_CALL) */

#if YYBTYACC
    if (yyerrctx)
    {
        yyFreeState(yyerrctx);
        yyerrctx = NULL;
    }
    while (yyps)
    {
        YYParseState *save = yyps;
        yyps = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
    while (yypath)
    {
        YYParseState *save = yypath;
        yypath = save->save;
        save->save = NULL;
        yyFreeState(save);
    }
#endif /* YYBTYACC */
    yyfreestack(&yystack);
    return (yyresult);
}
