#ifndef _PARSERWRAPPER_H_
#define _PARSERWRAPPER_H_
#include "Guard.h"
#include "Marking.h"
#include "CFunc.h"
#include <string>
#include <set>
class PNet;
class CFunc;
class Marking;
class Domain;
class Arc;

using namespace std ;

/* IMPORTANT: Use setModel to allow semantic/syntaxic verification of the expressions parsed, 
   and to construct correctly the expressions parsed. A context is REQUIRED for the parse 
   to complete, either through the second optional parameter pPN, or through an initial call to SetModel */




namespace ParserWrapper {
  extern PNet* PN;
  extern set<string> tobsFilter;
  extern Guard * result_guard;
  extern Marking* result_mark;
  extern CFunc* result_func;
  extern PNClass* result_gcol;
  extern Domain *pDom;
  extern const char * bufname ;
  extern string tname;


  int SetTobsFilter (const set<string> & s);
  int ParseTobs (const string &path, PNet *pPN=NULL) ;
  Guard * ParseGuard( const string &,const string & ttname = "", bool isGSPN=false, PNet *pPN=NULL) ;  
  Marking * ParseMarking (const string &,Domain *dom, bool isGSPN = false, PNet *pPN = NULL ) ;
  CFunc * ParseFunc (const string & str,Domain *dom,bool isGSPN=false,Arc *a=NULL, PNet *pPN = NULL) ;
  PNClass * gspnParseColor (const string & str, const string & name, PNet *pPN = NULL ) ;

  void setModel (PNet *);

}
#endif
