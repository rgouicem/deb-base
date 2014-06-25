
#include "CFuncElt.h"
#include "Element.h"
#include "CalcSubClasses.h"
#include "Variable.h"
#include <algorithm>

vector<Element>  CFuncElt::getAssymetry () {

  if (isVar) return vector<Element> ();
  else 
    return elts;

}

void CFuncElt::FindOrderedClasses () {
  if (isVar) {
    list<pair<Variable *,int> > ::const_iterator var;
    for (var = vars.begin() ; var != vars.end() ; var++) {
      if (var->second != 0) var->first->PClass()->setOrdered(true);
    }
  }

}

void CFuncElt::FindMentionedVars (set<Variable *> &allvars) {
  if (isVar) {
    list<pair<Variable *,int> > ::const_iterator var;
    for (var = vars.begin() ; var != vars.end() ; var++)
      allvars.insert(var->first);
  }
}


void  CFuncElt::ExportToCami (ostream &os,PNClass *pC) {
  list<pair<Variable *,int> > ::const_iterator var;

  
  if (isVar) {
   for (var = vars.begin() ; var != vars.end() ; var++) {
      os << var->first->Name();
      if (var->second > 0) os << "++" << var->second;
      if (var->second < 0) os << "--" << (-var->second);
      if (var != --vars.end()) os << " + "; 
    } 
  } else {
    PNClass * ppc = pC->FindSubContaining(elts);
    if (ppc) {
      os << ppc->Name();
      os << ".ALL";
    } else {
      if (elts.size() > 1) {
	os << "{";
	calcSub::print(os,elts,false);
	os << "}.ALL";
      } else 
	os << elts.front().desc ;
    }  
  }

}

void  CFuncElt::ExportToGSPN (ostream &os,PNClass *pC) {
  list<pair<Variable *,int> > ::const_iterator var;

  
  if (isVar) {
   for (var = vars.begin() ; var != vars.end() ; var++) {
     if (var->second > 0) os  << var->second << "!" ;
     if (var->second < 0) os << (-var->second) << "^" ;
     os << var->first->ExportToGSPN();
     
     if (var != --vars.end()) os << " + "; 
   } 
  } else {
      PNClass * ppc = pC->FindSubContaining(elts);
      if (ppc) {
	os << "S "<<  ppc->Name();
      } else {
	if (elts.size() > 1) {
	  os << "{";
	  calcSub::print(os,elts,false);
	  os << "}.ALL";
	} else 
	  os << elts.front().desc ;
      }  
    
  }
}


ostream & operator<< (ostream &os,const CFuncElt & ce) {
  list<pair<Variable *,int> > ::const_iterator var;

  if (ce.isVar) {
    for (var = ce.vars.begin() ; var != ce.vars.end() ; var++) {
      os << var->first->Name();
      if (var->second > 0) os << "++" << var->second;
      if (var->second < 0) os << "--" << (-var->second);
      if (var != --ce.vars.end()) os << " + "; 
    }
  } else {
    if (ce.elts.size() > 1) {
      os << "{";
      calcSub::print(os,ce.elts,false);
      os << "}.ALL";
    } else 
      os << ce.elts.front().desc ;
  }

  return os;
}

/* for sorting in order of variable the succ */
class pair_var_succ {
public :
  pair_var_succ () {};
  bool operator()(const pair<Variable *,int>& a,const   pair<Variable *,int>& b) { return *a.first < *b.first?true:*a.first==*b.first?a.second<b.second:false; }
};


CFuncElt  operator+( CFuncElt&e1, CFuncElt &e2) {

  CFuncElt ret;
  vector<pair<Variable *,int> > vret;

  if (e1.isVar != e2.isVar )
    return ret;
  else ret.isVar = e1.isVar;
  
  ret.elts = calcSub::operator+(e1.elts , e2.elts) ;
  vret.insert(vret.begin(),e1.vars.begin(),e1.vars.end());
  vret.insert(vret.begin(),e2.vars.begin(),e2.vars.end());
  sort (vret.begin(),vret.end(),pair_var_succ());
  ret.vars.insert(ret.vars.end(),vret.begin(),vret.end());
  return ret;
}

