#ifndef _CFUNCELT_H_
#define _CFUNCELT_H_

#include <vector>
#include <list>
#include <set>
#include <utility>
#include <cassert>
#include "Element.h"
class Variable;

class PNClass;
using namespace std;

/* An Element of a CFuncTerm, either a vector of elements or of Variable */

class CFuncElt {
  vector<Element> elts;
  list<pair<Variable *,int> >  vars ; // list<Variable,Successor> i.e <v1,0>,<v2,1>,<v3,-1> = v1 + v2++ + v3-- ;
  bool isVar;
 public:
  // Constructors
  CFuncElt(const vector<Element> &vv=vector<Element>()) :elts(vv),isVar(false) {}
  CFuncElt(Variable *vv,int succe=0):isVar(true) { vars.push_back(make_pair(vv,succe)); }
  
  bool IsVar () const {return isVar; }
  int EltsSize () const { return elts.size();}

  const vector<Element>& Elts() const {return elts;}
  vector<Element>& Elts () { return elts; };

  const list< pair<Variable*, int> >& getVars() const {assert(isVar);return vars;}
  list<pair<Variable *,int> >& getVars() { assert(isVar);return vars;}

  vector<Element> getAssymetry ();
  void FindOrderedClasses();
  void FindMentionedVars (set<Variable *> &allvars);
  bool operator== ( class CFuncElt&e) { return elts==e.elts && vars == e.vars && isVar == e.isVar; }

  // To offer a homogeneous prototype with Mark, 
  // analoguous to vector<Element>::operator+ defined in calcSubClasses
  friend class CFuncElt  operator+( class CFuncElt&, class CFuncElt &);
    

  void ExportToGSPN (ostream &os,PNClass *pC); 
  void ExportToCami (ostream &os,PNClass *pC); 
  friend ostream & operator<< (ostream &os,const CFuncElt & ce) ; 
  friend class CFuncTerm;

  bool operator==(const CFuncElt& elt) const
    {
      return (this->isVar == elt.isVar)
	&& (this->elts == elt.elts)
	&& (this->vars == elt.vars);
    }
};


#endif
