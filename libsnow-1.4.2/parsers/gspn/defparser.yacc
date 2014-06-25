/****************************************************************************
*
*  Module name        : defparser.yacc
*  Version number     : Beta 0
*  Created date       : Thu Oct 23 17:35:56 CEST 2003
*  Created by         : Yann Thierry-Mieg
*
*  Subject            : Parser for greatspn WN .def file, returns a list of declarations found in the file
*
*  Revision Date : 
*
****************************************************************************/


%{

#define YYDEBUG 0
#define _PARSER_SANDRINE_YACC_
#define _MARS_TYPE_COMPATIBLE_
#define PRINTF_ALLOWED 1

#include <list>
#include <string>
#include <vector>

#include "PNet.h"
#include "CFunc.h"
#include "CFuncTerm.h"
#include "CFuncElt.h"
#include "Domain.h"
#include "FuncParser.h"
#include <cstdio>
using namespace std;

extern int funcerror(const char*)	;
extern int funcparse();
extern int unput_car();

/* Les variables globales */

extern FILE *yyin;

extern PNet  *PN;
extern CFunc* result_func;	/* utilise pour mettre le resultat de l'analyse */
extern Domain *pDom; /* domain of the place considered */
extern char yytext[];
extern int funclex(void);
extern int funcerror (const char*);

/* Variables locales */
namespace FuncParser {
list<PNClass *>::iterator curclass;
}
using namespace FuncParser;

%}


/*------------------------------ Definition des TOKENS -----------------------------*/

%token <entier> ALL  PLUS MOINS
%token <entier> VIRG  PLUS MOINS
%token <entier> POINT INC MULT  PLUS MOINS
%token <entier> INF SUP INC DEC

%token <chaine> IDENT NBR_SS 



/*------------------------------ Definition de precedences -----------------------*/


/*------------------------------ Definition des types de retour des regles --------------------*/


%type <func> CFunc DebutCFunc
%type <functerm> CFuncTerm 
%type <lelts> SuiteElements Elements
%type <elements> Elt
%type <entier> Nombre
%type <chaine> IdentElt


%union {
  char	*chaine;
  int	entier;
  class CFunc * func;
  class CFuncTerm * functerm;
  list<CFuncElt > * lelts;
  CFuncElt * elements;
}




/*--------- Definition de la derniere regle reduite dans la grammaire  ------------------------------*/

%start DebutCFunc

%%

DebutCFunc               : CFunc {
                          result_func = $1;
                        }
                        ;

CFunc			: CFuncTerm MOINS CFunc
				{
			      $3->Lst().front().Mult() = -$3->Lst().front().Mult();
			      $3->Insert (*$1);
			      delete $1;
			      $$ = $3;
				}
			| CFuncTerm PLUS CFunc
                            {   
			      $3->Insert (*$1);
			      delete $1;
			      $$ = $3;
			    }
                        | CFuncTerm 
                           {
			     CFunc *m = new CFunc ();
			     m->Insert(*$1);
			     delete $1;
			     $$ = m;
			   }	
                        ;			

CFuncTerm                    : Nombre MULT INF Elements SUP 
                           {
			     CFuncTerm * m = new CFuncTerm(pDom, *$4,$1);
			     delete $4;
			     $$ = m;
			   }	
                         | INF Elements SUP
                           {
		       	     CFuncTerm * m = new CFuncTerm(pDom, *$2);
			     delete $2;
			     $$ = m;
			   }	
                         | Nombre 
                           {
			     if (pDom != NULL) { yyerrok; } /*null color domain*/
			     CFuncTerm * m = new CFuncTerm($1);
			     $$ =m;
			   }	
                        ;


Elements                : InitialList SuiteElements
                           {
			     if (curclass != pDom->Elts().end()) { yyerrok; }
			     $$ = $2; 	
			   }
                        ;

InitialList             : /*empty*/     {
              			     curclass = pDom->Elts().begin() ;
			   }
                        ;


SuiteElements           :  Elt { 
                                list<CFuncElt > *pM = new list<CFuncElt >() ;
				pM->push_front (*$1);
				delete $1;
				$$ = pM;
                           }
                        |  Elt VIRG SuiteElements 
                           { 
			     $3->push_front(*$1);
			     delete $1;
			     $$ = $3;
			   }
                        ;
Elt                     : IdentElt   
                        {  
			   
			   Element *e = (*curclass)->Find($1);
			   // test for variable
			   if (!e) { 
				Variable * var = PN->LVar.FindName($1);
				if (!var || var->PClass() != *curclass) yyerrok; 
				CFuncElt * fe = new CFuncElt(var);
				curclass++;
				$$ = fe;
			   } else { 
			     // else plain element
			     
			     vector<Element> * v = new vector<Element> () ;
			     v->push_back(*e);
			     curclass++;
			     CFuncElt * fe = new CFuncElt(*v);
			     $$ = fe ;
			   }
			}
                        | IDENT POINT ALL
                            {
				/* place is uncolored ; no diffusion allowed !!! */
				if (!*curclass) { cerr << "Diffusion from null domain place !!" <<endl; yyerror("Diffusion from null domain place !!");}
			     PNClass * pC = PN->LClasse.FindName($1);
			     if (!pC) { cerr << "Unknown class "<< $1; yyerrok; }
			     if (pC->Name() != (*curclass)->Name() && pC->Parent()->Name() != (*curclass)->Name()) {cerr<<("Domain of token incompatible with place");yyerrok;}
			     vector<Element> * v = new vector<Element> (pC->Elts());
			     curclass++;
			     CFuncElt * fe = new CFuncElt(*v);
			     $$  = fe ;
			    }
                        | IDENT INC Nombre 
                           {
				Variable * var = PN->LVar.FindName($1);
				if (!var || var->PClass() != *curclass) yyerrok; 
				CFuncElt * fe = new CFuncElt(var,$3);
				curclass++;
				$$ = fe;
			   }
                        | IDENT DEC Nombre 
                           {
				Variable * var = PN->LVar.FindName($1);
				if (!var || var->PClass() != *curclass) yyerrok; 
				CFuncElt * fe = new CFuncElt(var,-$3);
				curclass++;
				$$ = fe;
			   }
                        ;

IdentElt                : IDENT { $$ =$1}
                        | NBR_SS {  $$ = $1;}
                        ;

Nombre                  : NBR_SS {
			  int i;
			  if (sscanf($1,"%d",&i) ==1)
			    $$ = i;
			  else { yyerrok ; }
                        }
                        | /*empty*/ {
                           $$ = 1;
                        }
                        ;
			
%%



int yyerror (const char *s) {
	
	cerr << "ouch !!! " << s << endl;
}




