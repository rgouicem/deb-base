#include "CFunc.h"
#include <sstream>
#include "CalcSubClasses.h"


list<vector<Element> > CFunc::getAssymetry (PNClass *p) {
  CFunc m(calcSub::groupTemplate(lst,(CFuncElt *) NULL ));
  list<vector<Element> > lcall,lcallres ;
  list<CFuncTerm>::iterator it;

  for (it = m.lst.begin();it!=m.lst.end() ; it++ ) {
    lcallres = it->getAssymetry(p);
    lcall.insert(lcall.end(),lcallres.begin(),lcallres.end());
  }
  lcall = calcSub::uniquePartition (lcall);

  return lcall;
}

map< list<PNClass*> , CFunc >  CFunc::ApplyStaticBinding (const map<Variable*,PNClass *> & binding) {
  map<list<PNClass*>,CFunc> res;
  for (list<CFuncTerm>::iterator it = lst.begin();it!=lst.end();it++) {
    vector< pair < list <PNClass *>,CFuncTerm > > tokens = it->ApplyStaticBinding (binding);
    vector< pair < list <PNClass *>,CFuncTerm > >::iterator jt = tokens.begin();
    for ( ; jt != tokens.end() ; jt ++) {
      res[jt->first].Insert(jt->second); 
    }
  }
  return res;
}

void CFunc::RewriteWithStaticClasses () {
  
  list<CFuncTerm> newlist;
  list<CFuncTerm>::iterator it;

  for (it = lst.begin();it!=lst.end();it++) {
    list<CFuncTerm> res = it->RewriteWithStaticClasses ();
    newlist.insert(newlist.end(),res.begin(),res.end());
  }

  lst = newlist;

}

void CFunc::FindMentionedVars (set<Variable *> &vars) {
  list<CFuncTerm>::iterator it;
  for (it = lst.begin();it!=lst.end() ; it++ ) {
    it->FindMentionedVars (vars);
  }
}


void  CFunc::FindOrderedClasses () {
  list<CFuncTerm>::iterator it;
  for (it = lst.begin();it!=lst.end() ; it++ ) {
    it->FindOrderedClasses ();
  }
}


string CFunc::ExportToCami () {
  list<CFuncTerm>::iterator it;
  ostringstream ost ;
  int i;
  int max =  lst.size() -1;


  for (it = lst.begin(),i=0;it!=lst.end() ;i++) {
    it->ExportToCami (ost);
    it++;
    if (i < max) {      
      if (it->Mult() > 0) ost << "+";
      else ost << "-";
    }
  }
  return ost.str();

}
int CFunc::getMultiplicityBWfunc () {
  return lst.front().Mult();
}


string CFunc::ExportToGSPN () {
  list<CFuncTerm>::iterator it;
  ostringstream ost ;
  int i;
  int max =  lst.size() -1;


  for (it = lst.begin(),i=0;it!=lst.end() ;i++ ) {
    it->ExportToGSPN (ost);
    it++;
    if (i < max) { 
      if (it->Mult() > 0) ost << "+";
      else ost << "-";
    }
  }

  return ost.str();

}


ostream & operator<< (ostream & os , const CFunc &m) {
  list<CFuncTerm>::const_iterator it;
  int i;
  int max =  m.lst.size() -1;

  for (it = m.lst.begin(),i=0;it!=m.lst.end() ;i++ ) {
    os << *it ;
       it++;
    if (i < max) { 
      os << "+";
    }
  }
  
  return os;
}
