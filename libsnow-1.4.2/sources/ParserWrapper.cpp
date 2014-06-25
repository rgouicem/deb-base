#include "ParserWrapper.h"
#include "GuardParser.h"
#include "PNet.h"
#include "Arc.h"
#include <cstdio>
#include <unistd.h>
#include <set>
#include <string>
using namespace std;
// extern PNet* PN = NULL;
extern FILE * guardin;
extern FILE * gguardin;
extern FILE * markin;
extern FILE * gmarkin;
extern FILE * funcin;
extern FILE * gfuncin;
extern FILE * gcolin;
extern FILE * tobsin;
extern int markparse();
extern int gmarkparse();
extern int funcparse();
extern int gfuncparse();
extern int guardparse();
extern int gguardparse();
extern int gcolparse();
extern int tobsparse();
// Guard * result_guard=NULL;
// extern Marking* result_mark=NULL;
// extern CFunc* result_func=NULL;
// extern PNClass* result_gcol=NULL;
// extern Domain *pDom = NULL;
// extern const char * bufname = NULL;
 
namespace ParserWrapper {
  PNet* PN = NULL;
  set<string> tobsFilter =set<string> ();
  Guard * result_guard=NULL;
  Marking* result_mark=NULL;
  CFunc* result_func=NULL;
  PNClass* result_gcol=NULL;
  Domain * pDom = NULL;
  string tname = "";
  const char * bufname = NULL;


  void setModel (PNet *pPN) {
    PN = pPN;
  }
  int SetTobsFilter (const set<string> & s) {
    tobsFilter = s;
    return 0;
  }
  
  int ParseTobs (const string &path, PNet *pPN) {
    tobsin = fopen (path.c_str(),"r");
    if (pPN) PN = pPN;
    tobsparse();
    fclose(tobsin);
    return 1;
  }


  Guard * ParseGuard (const string & str, const string & ttname, bool isGSPN, PNet *pPN ) {
 //   cerr << "Parsing guard :" << str <<endl ;
    FILE ** ff;
    if (isGSPN) ff = &gguardin;
    else ff = &guardin ;
    *ff = tmpfile();
    fprintf(*ff,"%s\n",str.c_str());
    rewind(*ff); 

    if (pPN) PN = pPN;
    if (isGSPN) tname = ttname ;
    if (isGSPN) gguardparse();
    else guardparse();
    fclose(*ff);
 //   cerr << "Obtained guard:" << *result_guard<<endl;
    return result_guard;
  }

  Marking * ParseMarking (const string & str,Domain *dom,bool isGSPN, PNet *pPN ) {
//    cerr << "Parsing marking :" << str <<endl ;
    FILE ** ff;
    if (isGSPN) ff = &gmarkin ;
    else ff = &markin;
		  
    *ff = tmpfile();
    fprintf(*ff,"%s\n",str.c_str());
    rewind(*ff);
    if (pPN) PN = pPN;
    pDom = dom;
    if (isGSPN) gmarkparse();
    else markparse();
    fclose(*ff);

    return result_mark;
  }

  CFunc * ParseFunc (const string & str,Domain *dom,bool isGSPN, Arc * a,PNet *pPN ) {
//    cerr << "parsing Color function :" << str << endl;
    FILE ** ff;
    if (isGSPN) ff = &gfuncin ;
    else ff = &funcin;

    *ff = tmpfile();
    fprintf(*ff,"%s\n",str.c_str());
    rewind(*ff);
    if (pPN) PN = pPN;
    pDom = dom;
    if (isGSPN) tname = a->getTrans()->Name();
    if (isGSPN) gfuncparse();
    else funcparse();
    fclose(*ff);

    return result_func;
  }

  PNClass * gspnParseColor (const string & str, const string & name, PNet *pPN ) {
//     cerr << "parsing domain "<< name <<":#"<<str<<"#"<<endl;
//     cerr << "Pnet :" <<*PN<<endl;

    gcolin = tmpfile();
    fprintf(gcolin,"%s\n",str.c_str());
    rewind(gcolin);
    if (pPN) PN = pPN;
    bufname = name.c_str();
    if (!(result_gcol = PN->LClasse.FindName(name))) { 
      result_gcol = PN->LClasse.Insert(*(new PNClass()));
      result_gcol->Name(name) ;
    }

    gcolparse();
    fclose(gcolin);

    return result_gcol;
  }

  
  

}

