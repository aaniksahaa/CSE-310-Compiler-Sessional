%option noyywrap

%x TOKENIZING_CONST_CHAR
%x TOKENIZING_STRING
%x TOKENIZING_SLASH_COMMENT
%x TOKENIZING_STAR_COMMENT

%{

/* 

1. Handling \x or illformed char literal is not suported here as per specification. So, \x renders as x
2. Carriage return handled for Windows


*/

#include<bits/stdc++.h>
#include "headers/2005001_SymbolTable.hpp"

using namespace std;

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

ofstream tokenout("token.txt");
ofstream logout("log.txt");


int number_of_buckets_in_scopetables = 10;

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
		default : break;
	}
	return ascii;
}

void write_to_token_file(TokenLexemePair t)
{
	tokenout<<t.to_string()<<"\n";
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
	logout<<"Line# "<<current_lexeme_start_line_number<<": warning, "<<text<<".\n";
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
				warning_text = "tab required but got space";
			}
			else
			{
				warning_text = to_string(expected_tab_count) + " of tabs needed but got "+ to_string(tab_count) +" tabs";
			}
			handle_warning(warning_text);
		}
		stop_indentation_tracker();
	}
}

void handle_token_lexeme_pair(TokenLexemePair t)
{
	expected_tab_count = (t.token == "RCURL" ? nesting_level-1 : nesting_level);
	check_indentation();

	write_to_token_file(t);
	write_to_log_file(t);

	// perform additional tasks

	if(t.token == "LCURL")
	{
		nesting_level++;
		ST->enter_scope();
	}
	else if(t.token == "RCURL")
	{
		if(nesting_level)
		{
			nesting_level--;
		}
		ST->exit_scope();
	}
	if(t.token == "ID")
	{
		bool success = ST->insert_into_current_scope(t.lexeme, t.token);
		if(success)
		{
			ST->print_all_scope_tables();
		}
		else
		{
			logout<<"\t"<<t.lexeme<<" already exists in the current ScopeTable\n";
		}
	}
}

void handle_error(string error_name, string text)
{
	expected_tab_count = nesting_level;
	check_indentation();
	
	error_count++;
	logout<<"Error at line# "<<line_count<<": "<<error_name<<" "<<text<<"\n";
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
		handle_token_lexeme_pair(TokenLexemePair("CONST_CHAR",string(1,current_converted_text[1])));
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
	
	write_to_token_file(TokenLexemePair(token,current_converted_text));
	write_to_log_file(TokenLexemePair(token,current_string_text));
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

KEYWORD			"if"|"do"|"float"|"switch"|"for"|"int"|"void"|"default"|"else"|"break"|"double"|"case"|"while"|"char"|"return"|"continue"
ADDOP			"+"|"-"
MULOP			"*"|"/"|"%"
INCOP			"++"|"--"
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
REDUNDANT_DECIMAL_FLOAT	{DIGIT}*{DECIMAL_FRACTION}{2,}{EXPONENT}?
FRACTIONAL_EXPONENT		[eE][+-]?{DIGIT}*{DECIMAL_FRACTION}+
ILLFORMED_NUMBER		({DIGIT}+|{FLOAT_NUMBER}){FRACTIONAL_EXPONENT}
IDENTIFIER		({ALPHABET}|"_")({ALPHANUMERIC}|"_")*
INVALID_ID_SUFFIX_NUM_PREFIX	({DIGIT}+|{FLOAT_NUMBER}){IDENTIFIER}
BACKSLASH				"\\"
NON_BACKSLASH			[^\\]
SINGLE_QUOTE			"\'"
SPECIAL_CHARACTER	    "\\0"|"\\v"|"\\b"|"\\r"|"\\f"|"\\a"|"\\\'"|"\\\\"|"\\t"|"\\n"|"\\\""
DOUBLE_QUOTE			"\""
CARRIAGE_RETURN			"\r"
NEWLINE 				"\n"|"\r\n"	

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
			}

{ADDOP}		{
				handle_token_lexeme_pair(TokenLexemePair("ADDOP",yytext));
			}

{MULOP}		{
				handle_token_lexeme_pair(TokenLexemePair("MULOP",yytext));
			}

{INCOP}		{
				handle_token_lexeme_pair(TokenLexemePair("INCOP",yytext));
			}

{RELOP}		{
				handle_token_lexeme_pair(TokenLexemePair("RELOP",yytext));
			}

{ASSIGNOP}	{
				handle_token_lexeme_pair(TokenLexemePair("ASSIGNOP",yytext));
			}

{LOGICOP}	{
				handle_token_lexeme_pair(TokenLexemePair("LOGICOP",yytext));
			}

{BITOP}		{
				handle_token_lexeme_pair(TokenLexemePair("BITOP",yytext));
			}

{NOT}		{
				handle_token_lexeme_pair(TokenLexemePair("NOT",yytext));
			}

{LPAREN}	{
				handle_token_lexeme_pair(TokenLexemePair("LPAREN",yytext));
			}

{RPAREN}	{
				handle_token_lexeme_pair(TokenLexemePair("RPAREN",yytext));
			}

{LCURL}		{
				handle_token_lexeme_pair(TokenLexemePair("LCURL",yytext));
			}

{RCURL}		{
				handle_token_lexeme_pair(TokenLexemePair("RCURL",yytext));
			}

{LSQUARE}	{
				handle_token_lexeme_pair(TokenLexemePair("LSQUARE",yytext));
			}

{RSQUARE}	{
				handle_token_lexeme_pair(TokenLexemePair("RSQUARE",yytext));
			}
			
{COMMA}		{
				handle_token_lexeme_pair(TokenLexemePair("COMMA",yytext));
			}

{SEMICOLON}	{
				handle_token_lexeme_pair(TokenLexemePair("SEMICOLON",yytext));
			}

{CONST_INT}	{
				handle_token_lexeme_pair(TokenLexemePair("CONST_INT",yytext));
			}

{CONST_FLOAT} {
			  	   handle_token_lexeme_pair(TokenLexemePair("CONST_FLOAT",yytext));
			  }

{REDUNDANT_DECIMAL_FLOAT} 	{
								handle_error("TOO_MANY_DECIMAL_POINTS", yytext);
							}

{ILLFORMED_NUMBER} 	{
						handle_error("ILLFORMED_NUMBER", yytext);
					}

{IDENTIFIER}	{
					handle_token_lexeme_pair(TokenLexemePair("ID",yytext));
				}

{INVALID_ID_SUFFIX_NUM_PREFIX}	{
									handle_error("INVALID_ID_SUFFIX_NUM_PREFIX", yytext);
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
		handle_error("UNRECOGNIZED_CHAR", yytext);
	}

%%

int main(int argc,char *argv[]){
	
	if(argc!=2){
		printf("Please provide input file name and try again\n");
		return 0;
	}
	
	FILE *fin=fopen(argv[1],"r");
	if(fin==NULL){
		printf("Cannot open specified file\n");
		return 0;
	}

	yyin= fin;
	yylex();

	ST->print_all_scope_tables();

	logout<<"Total lines: "<<line_count<<endl;
	logout<<"Total errors: "<<error_count<<endl;
	logout<<"Total warnings: "<<warning_count<<endl;

	fclose(yyin);
	tokenout.close();
	logout.close();
	return 0;
}
