%option noyywrap

%{
#include<bits/stdc++.h>
using namespace std;

#include "y.tab.h"

extern ofstream logout, errorout;

%}


WS [ \t]+
DIGIT [0-9]
NUMBER [-]?{DIGIT}+(\.{DIGIT}+)?

%%

{NUMBER}  {
	     double num = atof(yytext);
		 logout<<"NUMBER found\n";
		 yylval.value = num;
	     return NUMBER;
	  }

"+"  	{ 
			logout<<"token PLUS found\n";
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
