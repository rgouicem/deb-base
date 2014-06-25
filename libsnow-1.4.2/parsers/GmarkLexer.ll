/****************************************************************************
*
*  Module name        : gmarklexer
*  Version number     : beta
*  Created date       : 11/2003
*  Created by         : Yann Thierry-Mieg
*
*  Subject            : Lexer for WN initial marking declarations, gspn syntax
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
#include "GmarkParser.h"



// #undef exit
// #define exit( YY_EXIT_FAILURE )  if (YY_EXIT_FAILURE) FkExit(kFk_EndOnError); else FkExit(kFk_EndNoProblem)


%}

%option yylineno
%option noyymore
%option noinput
%option nounput
%option prefix="gmark"

%a 3000
%o 4000


%%
\n                    { ungetc((char)0,gmarkin);
                        /*return(NEWLINE);*/ }
\+		      { return(PLUS);        }
\-		      { return(MINUS);       }
\,		      { return(COMMA);       }
\<		      { return(LT);      }
\>		      { return(GT);     }
S                     { return(S);           }
[0-9]+		      { 
			gmarklval.entier = atoi(yytext) ;
			return(NUM);
		      }
[a-zA-Z][a-zA-Z_0-9]* { 
			gmarklval.chaine = strdup (yytext) ;
			 /*fprintf(stdout,"STRING\n");*/return(STRING);
		      }
[' ']|[\0]            {
                        /* Do NOTHING !! DO NOT ECHO ON STDOUT !!! */
                      }
%%


int yywrap ( void ) { return (1); }
