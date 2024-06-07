%{
#include<bits/stdc++.h>
using namespace std;

extern FILE* yyin;
FILE *fp;

ofstream logout("log.txt");
ofstream errorout("error.txt");

void yyerror(char *s){
    //
}

int yyparse(void);
int yylex(void);

%}

%union {
    double value;
}

%token <value> NUMBER
%token NEWLINE PLUS MINUS SLASH ASTERISK LPAREN RPAREN

%type <value> expr term factor 

%%
input:              /* empty string */
    | input line
    ;
line: NEWLINE
    | expr NEWLINE           { logout<<$1; }
    ;
expr: expr PLUS term         { 
        logout<<"expr: expr PLUS term\n";
        $$ = $1 + $3; 
    }
    | expr MINUS term        { $$ = $1 - $3; }
    | term  { logout<<"expr: term\n"; }
    ;
term: term ASTERISK factor   { $$ = $1 * $3; }
    | term SLASH factor      { $$ = $1 / $3; }
    | factor    { logout<<"term: factor\n"; }
    ;
factor: LPAREN expr RPAREN  { $$ = $2; }
      | NUMBER  { logout<<"factor: NUMBER\n"; }
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
	
	return 0;
}
