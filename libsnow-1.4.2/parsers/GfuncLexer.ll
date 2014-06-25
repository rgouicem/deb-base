/****************************************************************************
*
*  Module name        : gfunclexer
*  Version number     : beta
*  Created date       : 11/2003
*  Created by         : Yann Thierry-Mieg
*
*  Subject            : Lexer for WN color function declarations, gspn syntax
*
*  Revision Date : 
*
****************************************************************************/

%{
#define _PARSER_SANDRINE_LEX_
#define _MARS_TYPE_COMPATIBLE_
#define PRINTF_ALLOWED 			1



#define YYDEBUG 0
 
#include <string>
#include <list>
#include <cstdio>
#include <iostream>
using namespace std;
#include "CFuncElt.h"
#include "GfuncParser.h"



// #undef exit
// #define exit( YY_EXIT_FAILURE )  if (YY_EXIT_FAILURE) FkExit(kFk_EndOnError); else FkExit(kFk_EndNoProblem)


%}

%option yylineno
%option noyymore
%option noinput
%option nounput
%option prefix="gfunc"

%a 3000
%o 4000


%%
\n                    { ungetc((char)0,gfuncin);
                        /*return(NEWLINE);*/ }
\+		      { return(PLUS);        }
\-		      { return(MINUS);       }
\<		      { return(LT);      }
\>		      { return(GT);     }
\!		      { return(ESCL);        }
\,		      { return(COMMA);       }
\[		      { /*fprintf(stdout,"[\n");*/return(OBRCK);       }
\]		      { /*fprintf(stdout,"]\n");*/return(CBRCK);       }
[0-9]+		      { 
			gfunclval.entier = atoi(yytext) ;
			return(NUM);
		      }
S                     { return(S);           }
[a-zA-Z][a-zA-Z_0-9]* { 
			gfunclval.chaine = strdup (yytext) ;
			 /*fprintf(stdout,"STRING\n");*/return(STRING);
		      }
[' ']|[\n]       {
                        /* Do NOTHING !! DO NOT ECHO ON STDOUT !!! */
                      }
%%

int yywrap ( void ) { return (1); }
