%option noyywrap

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "y.tab.h"

extern FILE* logout, errorout;

%}


WS [ \t]+
DIGIT [0-9]
NUMBER [-]?{DIGIT}+(\.{DIGIT}+)?

%%

{NUMBER}  {
	     double num = atof(yytext);
		 fprintf(logout, "NUMBER %.0lf found\n", num);
		 yylval.value = num;
	     return NUMBER;
	  }

"+"  	{ 
			fprintf(logout, "token PLUS found\n");
			return PLUS; 
		}
"-"     { 
			return MINUS; 
		}
"/"     { 
			return SLASH; 
		}
"*"     { 
			return ASTERISK; 
		}
"("     { 
			return LPAREN; 
		}
")"     { 
			return RPAREN; 
		}
"\n"    { 
			return NEWLINE; 
		}
{WS} { /* No action and no return */}
