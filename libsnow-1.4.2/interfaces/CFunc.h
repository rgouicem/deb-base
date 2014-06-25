#ifndef _CFUNC_H_
#define _CFUNC_H_


#include <list>
#include "CFuncTerm.h"
using namespace std;

// Class defining a color function on an arc of a net

class CFunc {
protected : 
  list<CFuncTerm> lst;
public :
  CFunc () {} ;
  CFunc (list<CFuncTerm> l ):lst(l) {};
  virtual ~CFunc () {};

  list<vector<Element> > getAssymetry (PNClass *) ;
  void RewriteWithStaticClasses ();

  map<list<PNClass*>,CFunc >  ApplyStaticBinding (const map<Variable*,PNClass *> & binding);

  void FindOrderedClasses();  
  void FindMentionedVars (set<Variable *> &vars);
  // For implementation reasons, used in ExportToGSPN
  int getMultiplicityBWfunc ();

  const list<CFuncTerm>& Lst() const {return lst;}
  list<CFuncTerm> & Lst() {return lst;}
  void Insert (const CFuncTerm &m) { lst.push_front(m); }

  friend  ostream & operator<< (ostream & os , const CFunc &m) ;

  string ExportToGSPN () ;
  string ExportToCami () ;

  bool operator==(const CFunc& func) const {return this->lst == func.lst;}

};





#endif
