%{
#include <stdio.h>
#include <stdlib.h>

extern FILE* yyin;
FILE *errorout,*logout,*fp;

void yyerror(char *s){
    fprintf(errorout, "%s\n", s);
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
    | expr NEWLINE           { fprintf(logout, "\t%.10g\n", $1); }
    ;
expr: expr PLUS term         { 
        fprintf(logout, "expr: expr PLUS term\n");
        $$ = $1 + $3; 
    }
    | expr MINUS term        { $$ = $1 - $3; }
    | term  { fprintf(logout, "expr: term\n"); }
    ;
term: term ASTERISK factor   { $$ = $1 * $3; }
    | term SLASH factor      { $$ = $1 / $3; }
    | factor    { fprintf(logout, "term: factor\n"); }
    ;
factor: LPAREN expr RPAREN  { $$ = $2; }
      | NUMBER  { fprintf(logout, "factor: NUMBER\n"); }
      ;
%%

int main(int argc,char *argv[])
{

	if((fp=fopen(argv[1],"r"))==NULL)
	{
		printf("Cannot Open Input File.\n");
		exit(1);
	}
	
	logout= fopen("log.txt","w");
	errorout= fopen("error.txt","w");	

	yyin=fp;
	yyparse();

	fclose(logout);
	fclose(errorout);
	
	return 0;
}
