/****************************************************************************
*
*  Module name        : reseau.lex
*  Version number     : 2.0
*  Created date       : Thu Jun  7 15:11:20 ETE 1990 (Around there)
*  Created by         : Bonnaire Xavier
*
*  Subject            : grammar for observation transitions
*
*  Major Revision for libPetri : Yann Thierry-Mieg , 09/2002
*  Revision for FrameKit Environment : Alioune DIAGNE
*  Revision Date : April, 15th, 1996
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
#include "Transition.h"
#include "tobsParser.h"


// #undef exit
// #define exit( YY_EXIT_FAILURE )  if (YY_EXIT_FAILURE) FkExit(kFk_EndOnError); else FkExit(kFk_EndNoProblem)

%}


%option noyymore
%option yylineno
%option noinput
%option nounput
%option prefix="tobs"

%a 3000
%o 4000

/* states */


lettre				[A-Z|a-z]
chiffre				[0-9]
under				"_"
blancs				[" "\t\n]
ascii				[\t\40-\176]

%%  
^"#trans"                           return (TRANS);
^"#inhibitor"                       return (INHIB);
^"#pre"                             return (PRE);
^"#guard"                           return (GUARD);
^"#endtrans"                         return (ENDTRANS);
["][^"]*["]                            { tobslval.str = new std::string(yytext + 1,
                                                        yyleng - 2);
                          return STRING;
                        }       
({lettre}|{under})({lettre}|{under}|{chiffre})*	{char *tmp = strdup(yytext);
				                 tobslval.chaine =tmp;
						// cerr << "Read IDENT:" << tobslval.chaine<<endl; 
                                                 return(IDENT);
  			 			 }
[ \t\n]			{}
%%



int yywrap ( void ) { return (1); }

