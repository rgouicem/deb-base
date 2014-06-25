/****************************************************************************
*
*  Module name        : gcollexer
*  Version number     : beta
*  Created date       : 11/2003
*  Created by         : Yann Thierry-Mieg
*
*  Subject            : Lexer for WN color declarations, gspn syntax
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
#include "GcolParser.h"



// #undef exit
// #define exit( YY_EXIT_FAILURE )  if (YY_EXIT_FAILURE) FkExit(kFk_EndOnError); else FkExit(kFk_EndNoProblem)


%}

%option yylineno
%option noyymore
%option noinput
%option nounput
%option prefix="gcol"

%a 3000
%o 4000


%%
\n                    { ungetc((char)0,gcolin);
                        /*return(NEWLINE);*/ }
\-		      { return(MINUS);       }
\{		      { return(OBRCS);       }
\}		      { return(CBRCS);       }
\,		      { return(COMMA);       }
o/[' ']		      { 
			return(O) ;
		      }
u/[' ']		      { 
			return(US);
		      }
[0-9]+		      { 
			gcollval.entier = atoi(yytext) ;
			return(NUM);
		      }
[a-zA-Z][a-zA-Z_0-9]* { 
			gcollval.chaine = strdup (yytext) ;
			 /*fprintf(stdout,"STRING\n");*/return(STRING);
		      }
[' ']|[\0]            {
                        /* Do NOTHING !! DO NOT ECHO ON STDOUT !!! */
                      }
%%



int yywrap ( void ) { return (1); }
