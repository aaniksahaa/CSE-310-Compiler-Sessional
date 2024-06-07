%option noyywrap

%x TOKENIZING_CONST_CHAR

%{
#include<bits/stdc++.h>
using namespace std;

int line_count = 1;
int error_count = 0;
int warning_count = 0;

string current_const_char_text = "";  // stores the contents of yytext while in TOKENIZING_CONST_CHAR state

ofstream tokenout("token.txt");
ofstream logout("log.txt");

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
	char lexeme;
	char ch = text[1];
	switch(ch)
	{
		case '0': lexeme = '\0'; break;
		case 'v': lexeme = '\v'; break;
		case 'b': lexeme = '\b'; break;
		case 'r': lexeme = '\r'; break;
		case 'f': lexeme = '\f'; break;
		case 'a': lexeme = '\a'; break;
		case '\'': lexeme = '\''; break;
		case '\\': lexeme = '\\'; break;
		case 't': lexeme = '\t'; break;
		case 'n': lexeme = '\n'; break;
		case '\"': lexeme = '\"'; break;
		default : break;
	}
	return lexeme;
}

void handle_token_lexeme_pair(TokenLexemePair t)
{
	tokenout<<t.to_string()<<"\n";
	logout<<"Line# "<<line_count<<": Token <"<<t.token<<"> Lexeme "<<t.lexeme<<" found\n";
}

void handle_error(string error_name, string text)
{
	error_count++;
	logout<<"Error at line# "<<line_count<<": "<<error_name<<" "<<text<<"\n";
}

void handle_const_char()
{
	int len = current_const_char_text.length();
	if(len > 3)
	{
		handle_error("MULTICHAR_CONST_CHAR", current_const_char_text);
	}
	else 
	{
		handle_token_lexeme_pair(TokenLexemePair("CONST_CHAR",current_const_char_text));
	}
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
WHITESPACE 		[ \t\r\f\v]+ 
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
BACKSLASH				\\
NON_BACKSLASH			[^\\]
SINGLE_QUOTE			"\'"
SPECIAL_CHARACTER	    "\\0"|"\\v"|"\\b"|"\\r"|"\\f"|"\\a"|"\\\'"|"\\\\"|"\\t"|"\\n"|"\\\""
DOUBLE_QUOTE			"\""
CARRIAGE_RETURN			"\r"
NEWLINE 				(\n)|{CARRIAGE_RETURN}(\n)

%%

{WHITESPACE} {}

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

\n 	{line_count++;}

. {}

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
	fclose(yyin);
	tokenout.close();
	logout.close();
	return 0;
}
