%option noyywrap yylineno

%x TOKENIZING_CONST_CHAR
%x TOKENIZING_STRING
%x TOKENIZING_SLASH_COMMENT
%x TOKENIZING_STAR_COMMENT

%{

/* 	Changelog

	println added
	DECOP for -- instead of INCOP
	LTHIRD, RTHIRD token sent to parser instead of LSQUARE and RSQUARE
	error, warning prints commented out
	token writing commented out
	RETURN statements added

*/



#include<bits/stdc++.h>
#include "headers/2005001_SymbolTable.hpp"

using namespace std;



// include tokens declared in the parser so that they can be returned
#include "y.tab.h"



// external variables that are declared in parser
extern YYSTYPE yylval;
extern ofstream logout, errorout, debugout;

extern SymbolInfo* current_function_definition_parameter_list;
extern int exit_scope_pending;

extern int total_error_count;

int line_count = 1;
int error_count = 0;
int warning_count = 0;

string current_const_char_text = "";  // stores the contents of yytext while in TOKENIZING_CONST_CHAR state
string current_string_text = "";
string current_slash_comment_text = "";
string current_star_comment_text = "";

string current_converted_text = "";

int current_string_line_count = 0;
int current_lexeme_start_line_number = 0;

int space_count = 0;
int tab_count = 0;

int expected_tab_count = 0; // for proper indentation

int nesting_level = 0;

bool new_line_just_started = true;

int number_of_buckets_in_scopetables = 11;

SymbolTable* ST = new SymbolTable(number_of_buckets_in_scopetables, false, logout, false);


class TokenLexemePair  // A wrapper for the token-lexeme pair
{
public:
	string token, lexeme;
	TokenLexemePair(string a, string b)
	{
		token = a;
		lexeme = b;
	}
	string to_string()
	{
		return "<"+token+", "+lexeme+">";
	}
};

void restart_indentation_tracker()
{
	new_line_just_started = true;
	tab_count = space_count = 0;
}

void stop_indentation_tracker()
{
	new_line_just_started = false;
}

void capitalize_all_letters(string& s)
{
	int len = s.length();
	for(int i=0; i<len; i++)
	{
		s[i] = toupper(s[i]);
	}
}

TokenLexemePair get_keyword_pair(string text)
{
	string token, lexeme;
	token = lexeme = text;
	capitalize_all_letters(token);
	return TokenLexemePair(token,lexeme);
}

char get_special_character_ASCII(string text)
{
	char ascii;
	char ch = text[1];
	switch(ch)
	{
		case '0': ascii = '\0'; break;
		case 'v': ascii = '\v'; break;
		case 'b': ascii = '\b'; break;
		case 'r': ascii = '\r'; break;
		case 'f': ascii = '\f'; break;
		case 'a': ascii = '\a'; break;
		case '\'': ascii = '\''; break;
		case '\\': ascii = '\\'; break;
		case 't': ascii = '\t'; break;
		case 'n': ascii = '\n'; break;
		case '\"': ascii = '\"'; break;
		default : ascii = '#';          // this is a marker for wrongly formed special character
	}
	return ascii;
}

void write_to_token_file(TokenLexemePair t)
{
	//tokenout<<t.to_string()<<"\n";
}

void write_to_log_file(TokenLexemePair t)
{
	int line_number = line_count;
	if(t.token == "SINGLE LINE STRING" || t.token == "MULTI LINE STRING" || t.token == "SINGLE LINE COMMENT" || t.token == "MULTI LINE COMMENT")
	{
		line_number = current_lexeme_start_line_number;
	}
	logout<<"Line# "<<line_number<<": Token <"<<t.token<<"> Lexeme "<<t.lexeme<<" found\n";
}

void handle_warning(string text)
{
	warning_count++;
	//logout<<"Line# "<<current_lexeme_start_line_number<<": "<<text<<".\n";
}

void check_indentation()
{
	if(new_line_just_started)
	{
		//cout<<line_count<<" "<<tab_count<<" "<<space_count<<endl;
		if(!((tab_count == expected_tab_count) && (space_count == 0)))
		{
			string warning_text;
			if(space_count > 0)
			{
				warning_text = "Warning, tab requrired but got space";
			}
			else
			{
				warning_text = "warning, " + to_string(expected_tab_count) + " of tabs needed but got "+ to_string(tab_count) +" tabs";
			}
			handle_warning(warning_text);
		}
		stop_indentation_tracker();
	}
}

bool check_info_needed(string token)
{
	vector<string> tokens_with_info = {"ADDOP", "MULOP", "RELOP", "LOGICOP", "BITOP", "CONST_INT", "CONST_FLOAT", "ID", "CONST_CHAR", "SINGLE_LINE_STRING", "MULTI_LINE_STRING"};
	for(string s: tokens_with_info)
	{
		if(s == token)
		{
			return true;
		}
	}
	return false;
}

// string tokens do not come here, they are handles specially
void handle_token_lexeme_pair(TokenLexemePair t)
{
	expected_tab_count = (t.token == "RCURL" ? nesting_level-1 : nesting_level);
	check_indentation();

	//write_to_token_file(t);
	write_to_log_file(t);

	//cout<<t.lexeme<<" "<<t.token<<"\n";

	// if(check_info_needed(t.token))
	// {
	// 	yylval.info = new SymbolInfo(t.lexeme, t.token);
	// }

	// for convenience in building parse-tree, generate for all tokens

	SymbolInfo* info = new SymbolInfo(t.lexeme, t.token);
	info->set_line_counts(line_count, line_count); // since this function handles single-line tokens
	yylval.info = info;

	// perform additional tasks

	if(t.token == "LCURL")
	{
		debugout<<"LCURL found"<<endl;
		nesting_level++;
		ST->enter_scope();
		debugout<<"entered scope"<<endl;
		if(current_function_definition_parameter_list != NULL)
		{
			for(auto child: current_function_definition_parameter_list->get_child_identifiers())
			{
				string name = child->get_name();
				string type = child->get_evaluated_type();
				//debugout<<name<<"\n";

				ST->insert_into_current_scope(name, type);
				auto inserted_parameter = ST->lookup(name);
				inserted_parameter->set_evaluated_type(type);

				// not needed as function parameter is never array
				// write_ARRAY_to_SymbolInfo(ST->lookup(child->get_name()));
			}
			current_function_definition_parameter_list = NULL;  
			// note that this is not memory leakage, at this parameter list is needed later...
			// a pointer to the actual object is kept in the parse tree
		}
		debugout<<"Inserted parameters..."<<endl;
		exit_scope_pending++;   // new scope need to be exited when RCURL is found
		debugout<<ST->stringify_all_scope_tables()<<endl;
	}
	else if(t.token == "RCURL")
	{
		if(nesting_level)
		{
			nesting_level--;
		}
		//ST->exit_scope();  //  this will be done in parser
	}
	// if(t.token == "ID")
	// {
	// 	bool success = ST->insert_into_current_scope(t.lexeme, t.token);
	// 	if(success)
	// 	{
	// 		ST->print_all_scope_tables();
	// 	}
	// 	else
	// 	{
	// 		logout<<"\t"<<t.lexeme<<" already exists in the current ScopeTable\n";
	// 	}
	// }
}

void handle_error(string error_name, string text)
{
	expected_tab_count = nesting_level;
	check_indentation();
	
	error_count++;
	logout<<"Error at line# "<<line_count<<": "<<error_name<<" "<<text<<"\n";

	// added for syntax and semantic analysis
	total_error_count++;
}

void handle_current_const_char()
{
	int len = current_converted_text.length();
	if(len == 2)
	{
		handle_error("EMPTY_CONST_CHAR", current_const_char_text);
	}
	else if(len > 3)
	{
		handle_error("MULTICHAR_CONST_CHAR", current_const_char_text);
	}
	else 
	{
		char ch = current_converted_text[1];
		if(ch == '#')
		{
			handle_error("UNRECOGNIZED_CHAR",current_const_char_text);
		}
		else
		{
			handle_token_lexeme_pair(TokenLexemePair("CONST_CHAR",string(1,current_converted_text[1])));
		}
	}
}

void handle_current_string()
{
	expected_tab_count = nesting_level;
	check_indentation();

	string token;
	if(current_string_line_count == 1)
	{
		token = "SINGLE LINE STRING";
	}
	else
	{
		token = "MULTI LINE STRING";
	}
	
	//write_to_token_file(TokenLexemePair(token,current_converted_text));
	write_to_log_file(TokenLexemePair(token,current_string_text));

	SymbolInfo* info = new SymbolInfo(current_converted_text, token);
	info->set_line_counts(current_lexeme_start_line_number, line_count);
	yylval.info = info;
}

void handle_current_slash_comment()
{
	expected_tab_count = nesting_level;
	check_indentation();
	write_to_log_file(TokenLexemePair("SINGLE LINE COMMENT", current_slash_comment_text));
}

void handle_current_star_comment()
{
	expected_tab_count = nesting_level;
	check_indentation();
	write_to_log_file(TokenLexemePair("MULTI LINE COMMENT", current_star_comment_text));
}


%}

KEYWORD			"if"|"do"|"float"|"switch"|"for"|"int"|"void"|"default"|"else"|"break"|"double"|"case"|"while"|"char"|"return"|"continue"|"println"|"void"
ADDOP			"+"|"-"
MULOP			"*"|"/"|"%"
INCOP			"++"
DECOP			"--"
RELOP			"<"|"<="|">"|">="|"=="|"!=" 
ASSIGNOP		"="
LOGICOP			"&&"|"||"
BITOP			"&"|"|"|"^"|"<<"|">>"
NOT				"!"
LPAREN 			"("
RPAREN			")"
LCURL			"{"
RCURL			"}"
LSQUARE 		"["
RSQUARE 		"]"
COMMA 			","
SEMICOLON		";"
WHITESPACE 		[ \t\f\r\v]
ALPHABET		[a-zA-Z]
DIGIT 			[0-9]
ALPHANUMERIC			{ALPHABET}|{DIGIT}			
CONST_INT				{DIGIT}+
DECIMAL_FRACTION		\.{DIGIT}+
FLOAT_NUMBER			{DIGIT}*{DECIMAL_FRACTION}
EXPONENT				[eE][+-]?{DIGIT}+
CONST_FLOAT 			({DIGIT}+|{FLOAT_NUMBER}){EXPONENT}?
REDUNDANT_DECIMAL_FLOAT	{DIGIT}*(\.{DIGIT}*){2,}{EXPONENT}?
FRACTIONAL_EXPONENT		[eE][+-]?{DIGIT}*{DECIMAL_FRACTION}+
ILLFORMED_NUMBER		({DIGIT}+|{FLOAT_NUMBER}){FRACTIONAL_EXPONENT}
IDENTIFIER		({ALPHABET}|"_")({ALPHANUMERIC}|"_")*
INVALID_ID_SUFFIX_NUM_PREFIX	({DIGIT}+|{FLOAT_NUMBER}){IDENTIFIER}
BACKSLASH				"\\"
NON_BACKSLASH			[^\\]
SINGLE_QUOTE			"\'"
SPECIAL_CHARACTER	    "\\0"|"\\v"|"\\b"|"\\r"|"\\f"|"\\a"|"\\\'"|"\\\\"|"\\t"|"\\n"|"\\\""
DOUBLE_QUOTE			"\""
NEWLINE 				"\n"|"\r\n"
MULTICHAR_LITERAL		'[{NON_BACKSLASH}|{SPECIAL_CHARACTER}]{2,}'	

%%

{WHITESPACE} 	{
					if(new_line_just_started)
					{
						if(yytext[0] == '\t')
						{
							tab_count++;
						}
						else if(yytext[0] == ' ')
						{
							space_count++;
						}
					}
				}

{NEWLINE} 	{
				line_count++;
				current_lexeme_start_line_number = line_count;
				restart_indentation_tracker();
			}

{KEYWORD}	{
				handle_token_lexeme_pair(get_keyword_pair(yytext));
				vector<string> keywords_to_be_parsed = {"if","float","for","int","void","else","while","return","println"};
				vector<int> tokens = {IF, FLOAT, FOR, INT, VOID, ELSE, WHILE, RETURN, PRINTLN};
				int len = tokens.size();
				for(int i=0; i<len; i++)
				{
					if(yytext == keywords_to_be_parsed[i])
					{
						return tokens[i];
					}
				}
			}

{ADDOP}		{
				handle_token_lexeme_pair(TokenLexemePair("ADDOP",yytext));
				return ADDOP;
			}

{MULOP}		{
				handle_token_lexeme_pair(TokenLexemePair("MULOP",yytext));
				return MULOP;
			}

{INCOP}		{
				handle_token_lexeme_pair(TokenLexemePair("INCOP",yytext));
				return INCOP;
			}

{DECOP}		{
				handle_token_lexeme_pair(TokenLexemePair("DECOP",yytext));
				return DECOP;
			}

{RELOP}		{
				handle_token_lexeme_pair(TokenLexemePair("RELOP",yytext));
				return RELOP;
			}

{ASSIGNOP}	{
				handle_token_lexeme_pair(TokenLexemePair("ASSIGNOP",yytext));
				return ASSIGNOP;
			}

{LOGICOP}	{
				handle_token_lexeme_pair(TokenLexemePair("LOGICOP",yytext));
				return LOGICOP;
			}

{BITOP}		{
				handle_token_lexeme_pair(TokenLexemePair("BITOP",yytext));
				return BITOP;
			}

{NOT}		{
				handle_token_lexeme_pair(TokenLexemePair("NOT",yytext));
				return NOT;
			}

{LPAREN}	{
				handle_token_lexeme_pair(TokenLexemePair("LPAREN",yytext));
				return LPAREN;
			}

{RPAREN}	{
				handle_token_lexeme_pair(TokenLexemePair("RPAREN",yytext));
				return RPAREN;
			}

{LCURL}		{
				handle_token_lexeme_pair(TokenLexemePair("LCURL",yytext));
				return LCURL;
			}

{RCURL}		{
				handle_token_lexeme_pair(TokenLexemePair("RCURL",yytext));
				return RCURL;
			}

{LSQUARE}	{
				handle_token_lexeme_pair(TokenLexemePair("LSQUARE",yytext));
				// As per requirement, sending LTHIRD
				return LTHIRD;
			}

{RSQUARE}	{
				handle_token_lexeme_pair(TokenLexemePair("RSQUARE",yytext));
				// As per requirement, sending RTHIRD
				return RTHIRD;
			}
			
{COMMA}		{
				handle_token_lexeme_pair(TokenLexemePair("COMMA",yytext));
				return COMMA;
			}

{SEMICOLON}	{
				handle_token_lexeme_pair(TokenLexemePair("SEMICOLON",yytext));
				return SEMICOLON;
			}

{CONST_INT}	{
				handle_token_lexeme_pair(TokenLexemePair("CONST_INT",yytext));
				return CONST_INT;
			}

{CONST_FLOAT} {
			  	   handle_token_lexeme_pair(TokenLexemePair("CONST_FLOAT",yytext));
				   return CONST_FLOAT;
			  }

{REDUNDANT_DECIMAL_FLOAT} 	{
								handle_error("TOO_MANY_DECIMAL_POINTS", yytext);
							}

{ILLFORMED_NUMBER} 	{
						handle_error("ILLFORMED_NUMBER", yytext);
					}

{IDENTIFIER}	{
					handle_token_lexeme_pair(TokenLexemePair("ID",yytext));
					return ID;
				}

{INVALID_ID_SUFFIX_NUM_PREFIX}	{
									handle_error("INVALID_ID_SUFFIX_NUM_PREFIX", yytext);
								}

{MULTICHAR_LITERAL}	{
						handle_error("MULTICHAR_CONST_CHAR", yytext);
					}

{SINGLE_QUOTE}	{
					BEGIN TOKENIZING_CONST_CHAR;
					current_const_char_text = yytext;
					current_converted_text = yytext;
				} 

<TOKENIZING_CONST_CHAR>{

	{SINGLE_QUOTE}	{
						current_const_char_text += yytext;
						current_converted_text += yytext;
						handle_current_const_char();
						BEGIN INITIAL;			// end this state
					}
	{NEWLINE}	{
					handle_error("UNFINISHED_CONST_CHAR",current_const_char_text);
					line_count++;
					current_lexeme_start_line_number = line_count;
					restart_indentation_tracker();
					BEGIN INITIAL;
				}
	<<EOF>>		{
					handle_error("UNFINISHED_CONST_CHAR",current_const_char_text);
					BEGIN INITIAL;
				}
	{SPECIAL_CHARACTER}	{
							current_const_char_text += yytext;
							current_converted_text += get_special_character_ASCII(yytext);
						}
	\\{NON_BACKSLASH}	{
							current_const_char_text += yytext;
							current_converted_text += get_special_character_ASCII(yytext);
						}
	{NON_BACKSLASH}	{
						current_const_char_text += yytext;
						current_converted_text += yytext;
					}
}

{DOUBLE_QUOTE}	{
					BEGIN TOKENIZING_STRING;
					current_string_text = yytext;
					current_converted_text = "";
					current_string_line_count = 1;
					current_lexeme_start_line_number = line_count;
				}
<TOKENIZING_STRING>{
	{DOUBLE_QUOTE}	{
						current_string_text += yytext;
						handle_current_string();
						BEGIN INITIAL;
					}
	{BACKSLASH}{NEWLINE} {
								current_string_text += yytext;
								line_count++;
								current_string_line_count++;
							}
	{NEWLINE}	{
					handle_error("UNFINISHED_STRING",current_string_text);
					line_count++;
					current_lexeme_start_line_number = line_count;
					restart_indentation_tracker();
					BEGIN INITIAL;
				}
	<<EOF>>		{
					handle_error("UNFINISHED_STRING",current_string_text);
					BEGIN INITIAL;
				}
	{SPECIAL_CHARACTER}	{
							current_converted_text += get_special_character_ASCII(yytext);
							current_string_text += yytext;
						}
	. 	{
			current_string_text += yytext;
			current_converted_text += yytext;
		}
}

\/\/	{
			BEGIN TOKENIZING_SLASH_COMMENT;
			current_slash_comment_text = yytext;
			current_lexeme_start_line_number = line_count;
		}

<TOKENIZING_SLASH_COMMENT>{
	{BACKSLASH}{NEWLINE} 	{
								current_slash_comment_text += yytext;
								line_count++;
							}
	{NEWLINE}	{
					handle_current_slash_comment();
					line_count++;
					current_lexeme_start_line_number = line_count;
					restart_indentation_tracker();
					BEGIN INITIAL;
				}
	<<EOF>>		{
					handle_current_slash_comment();
					BEGIN INITIAL;
				}
	. 	{
			current_slash_comment_text += yytext;
		}
}

\/\*	{
			BEGIN TOKENIZING_STAR_COMMENT;
			current_star_comment_text = yytext;
			current_lexeme_start_line_number = line_count;
		}

<TOKENIZING_STAR_COMMENT>{
	\*\/	{
				current_star_comment_text += yytext;
				handle_current_star_comment();
				BEGIN INITIAL;
			}
	{NEWLINE}	{
					current_star_comment_text += yytext;
					line_count++;
				}
	<<EOF>>		{
					handle_error("UNFINISHED_COMMENT", current_star_comment_text);
					BEGIN INITIAL;
				}
	. 	{
			current_star_comment_text += yytext;
		}
}

.	{
		handle_error("UNRECOGNIZED CHAR", yytext);
	}

%%

/* int main(int argc,char *argv[]){
	
	if(argc != 2){
		cout<<"Sorry! Name of the input file must be provided. Please Try again."<<endl;
		return 0;
	}
	
	FILE *fin;
	
	// opening input file specified in the command argument
	fin = fopen(argv[1],"r");

	if(fin == NULL){
		cout<<"Sorry! The input file cannot be found."<<endl;
		return 0;
	}

	yyin= fin;
	yylex();

	// printing all scope tables after lexical analysis is complete
	ST->print_all_scope_tables();

	// printing line, error and warning counts
	logout<<"Total lines: "<<line_count<<endl;
	logout<<"Total errors: "<<error_count<<endl;
	logout<<"Total warnings: "<<warning_count<<endl;

	// closing the files that were opened
	fclose(yyin);
	tokenout.close();
	logout.close();

	return 0;
} */

/*

Command Shell Script

flex -o 2005001.cpp 2005001.l
g++ 2005001.cpp -lfl -o 2005001.out
./2005001.out input.txt

*/