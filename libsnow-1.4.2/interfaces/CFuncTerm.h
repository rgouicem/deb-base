#ifndef _CFUNCTERM_H_
#define _CFUNCTERM_H_


#include <list>
#include <iostream>
#include <map> 
#include "CFuncElt.h"
#include "Domain.h"
using namespace std;

// A term of Color function of an arc, used in CFunc, represents a tuple of a color function

class CFuncTerm {
protected : 
  Domain * dom ;
  list<CFuncElt> elts;
  int mult ;
public :
  // colored cfuncterms
  CFuncTerm (Domain *pdom,list<CFuncElt >&l,int m= 1):dom(pdom),elts(l),mult(m){} ;
  // b & w marks
  CFuncTerm (int multi):dom(NULL),mult(multi) {};  
  virtual ~CFuncTerm () {};

  // Accessors
  const int& Mult() const {return mult;}
  int& Mult () {return mult; } 

  Domain* const Dom() const {return dom;}
  
  // WARNING !! Assumes that Mark is more "grouped" than static subs
  // ie: Mark <1,a> is only possible if 1 and a are isolated in static subs
  // is ok if called after getAssymetry...
  list<class CFuncTerm> RewriteWithStaticClasses ();
  vector< pair< list <class PNClass *>,CFuncTerm> > ApplyStaticBinding (const map<Variable*,PNClass *> & binding) ;
  list<vector<Element> > getAssymetry (PNClass *) ;
  void FindOrderedClasses();
  void FindMentionedVars (set<Variable *> &vars);
  
  // list interface
  list<CFuncElt >::iterator begin() { return (elts.begin());}
  list<CFuncElt >::iterator end() { return (elts.end());}

  const list<CFuncElt>& Elts() const {return elts;}
  list<CFuncElt>& Elts() {return elts;}

  void ExportToGSPN (ostream &os) ;  
  void ExportToCami (ostream &os) ;
  friend  ostream & operator<< (ostream & os , const CFuncTerm &m) ;

  bool operator==(const CFuncTerm& term) const
    {
      return (this->dom == term.dom)
	&& (this->mult == term.mult)
	&& (this->elts == term.elts);
    }

};





#endif
