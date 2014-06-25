/****************************************************************************
*
*  Module name        : reseau.lex
*  Version number     : 2.0
*  Created date       : Thu Jun  7 15:11:20 ETE 1990 (Around there)
*  Created by         : Bonnaire Xavier
*
*  Subject            : Regles de lexique pour l'Analyseur/Coprocesseur du
*                       langage de declaration de reseaux de Petri.
*                       La grammaire doit etre de type LALR(1).
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
#include "GguardParser.h"


// #undef exit
// #define exit( YY_EXIT_FAILURE )  if (YY_EXIT_FAILURE) FkExit(kFk_EndOnError); else FkExit(kFk_EndNoProblem)

%}

%option yylineno
%option noyymore
%option noinput
%option nounput
%option prefix="gguard"

%a 3000
%o 4000


lettre				[A-Z|a-z]
chiffre				[0-9]
under				"_"
blancs				[" "\t\n]
ascii				[\t\40-\176]

%%
","					return(VIRG);
"["					return(CRO_O);
"]"					return(CRO_F);
"("					return(PAR_O);
")"					return(PAR_F);
"{"					return(ACC_O);
"}"					return(ACC_F);
"<"    					return(INF);
">"					return(SUP);
"!"                                     return ESCL;
"^"                                     return(CARET);
"++"					return(INC);
"--"					return(DEC);
"="					return(EGAL);
"<="					return(INF_E);
">="					return(SUP_E);
("<>"|"!=")	       			return(N_EGAL);
"TRUE"                                  return(T_TRUE);
"FALSE"                                 return(T_FALSE);
"NOT"                                  return(NOT);
"OR"                                    return (OR) ;
"AND"                                    return (AND) ;
"IN"                                    return(IN);
"d"                                     return(D);
{chiffre}+				{
		  			char *tmp = strdup(yytext);
					gguardlval.chaine =tmp;
					return(NBR_SS);
					}

({lettre}|{under})({lettre}|{under}|{chiffre})*	{char *tmp = strdup(yytext);
				                 gguardlval.chaine =tmp;
                                                 return(IDENT);
  						 }
{blancs}*				 ;

%%

int yywrap ( void ) { return (1); }
