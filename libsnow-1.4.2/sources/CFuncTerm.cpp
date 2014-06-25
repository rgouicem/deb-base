#include "CFuncTerm.h"
#include "CalcSubClasses.h"
#include "PNClass.h"

list<vector<Element> > CFuncTerm::getAssymetry (PNClass *p) {
  list<PNClass *>::iterator clit;
  list<CFuncElt >::iterator it;
  list<vector<Element> >  lres;

  if (dom) {
  for (clit = dom->Elts().begin() , it = elts.begin() ; clit != dom->Elts().end()  ; clit ++,it++ ) {
    if (*clit == p) {
      lres.push_back(it->getAssymetry());
    }
  }
  if (! lres.empty())
    lres = calcSub::uniquePartition (lres);
  }

  return lres;

}

vector<pair<list<PNClass *>,CFuncTerm> > CFuncTerm::ApplyStaticBinding (const map<Variable*,PNClass *> & binding) {
  vector<pair<list<PNClass *>,CFuncTerm> > ret,next;
  ret.push_back(make_pair(list<PNClass *>(),*this));
  if (!dom ||  ! dom->Size() ) {
    return ret;
  }

  
  list<CFuncElt >::iterator it;
  list<PNClass *>::iterator jt;
  int i=0;
  // foreach CFuncElt of the token
  for (it = elts.begin(),jt = dom->Elts().begin(), i=0;it != elts.end();it++,jt++,i++) { 
    // it's a variable : return dom(var) as given in binding
    if (it->IsVar()) {
      list<pair<Variable *,int> > & vars = it->getVars(); 
      next.clear();
      for (vector<pair<list<PNClass *>,CFuncTerm> >::iterator kt = ret.begin() ; kt != ret.end() ;kt++) {
	for (list<pair<Variable *,int> >::iterator lt = vars.begin(); lt!=vars.end() ;lt++) {
	  list<PNClass *> l = kt->first;
	  l.push_back( (binding.find(lt->first))->second );
	  CFuncElt c (lt->first,lt->second);
	  CFuncTerm ct(kt->second);
	  list<CFuncElt >::iterator tochange = ct.elts.begin();
	  advance(tochange,i);
	  *tochange=c;

	  next.push_back(make_pair(l,ct));
	}
      }
      ret = next;
    } else {
      // it's a set that respects static subclass partitions
      PNClass * ppc = (*jt)->FindSubContaining(it->elts);
      next.clear();
      for (vector<pair<list<PNClass *>,CFuncTerm> >::iterator kt = ret.begin() ; kt != ret.end() ;kt++) {
	  list<PNClass *> l = kt->first;
	  l.push_back(ppc);
	  next.push_back(make_pair(l,kt->second));
      }
      ret = next;
    }
  }

  return ret;

}

list<class CFuncTerm> CFuncTerm::RewriteWithStaticClasses () {

  list<CFuncTerm> ret;
  list<CFuncElt >::iterator it;
  list<PNClass *>::iterator jt;
  
  if (!dom || !dom->Size()) {
    ret.push_back(*this);
    return ret;
  }

  list<vector<CFuncElt > > compose;
  list<pair <int,int> > pos;
  // i.e: m= < {1,2} , {a,b}, x >
  // C = {1},{2} ; D = {a,b} ; E = {x},{y,z}
  // compose = { {[1],[2]}, {[a,b]},{[x]} }


  for (it = elts.begin(),jt = dom->Elts().begin();it != elts.end();it++,jt++) { 
    if (it->IsVar() ||  it->EltsSize() < 2) {
      vector<CFuncElt > cur;
      cur.push_back(*it);
      compose.push_back(cur);
      pos.push_back( make_pair(0,1) );
    } else {
      list<vector<Element> > subs = (*jt)->getSubSets();
      vector<vector<Element> > curtmp = calcSub::SeparateInto (it->Elts(),subs);
      vector<CFuncElt > cur;
      vector<vector<Element> >::iterator iit ;
      for (iit = curtmp.begin() ; iit != curtmp.end() ; iit++) {
	cur.push_back(CFuncElt(*iit));
      }
      compose.push_back(cur);
      pos.push_back( make_pair(0,cur.size()) );
    }
  }

  list< pair<int,int> >::iterator pit;
  list<vector<CFuncElt > >::iterator cit;

  do {
    list<CFuncElt > forret;

    for (pit = pos.begin(),cit = compose.begin() ; pit != pos.end() ;pit++,cit++) {
      forret.push_back((*cit)[pit->first]);
    }
    ret.push_back ( CFuncTerm(dom,forret,mult));
  } while (! calcSub::next(pos) );
  
  return ret;

}

void CFuncTerm::FindMentionedVars (set<Variable *> &vars) {
  for (list<CFuncElt >::iterator it = elts.begin();it != elts.end();it++) 
    it->FindMentionedVars (vars) ;
}

void CFuncTerm::FindOrderedClasses () {
  list<CFuncElt >::iterator it;
  for (it = elts.begin();it != elts.end();it++) 
    it->FindOrderedClasses () ;
}

void  CFuncTerm::ExportToCami (ostream &os) {
  list<CFuncElt >::iterator it;
  list<PNClass*>::iterator jt;



  if (dom && dom->Size()) {
    if (mult != 1 && mult!=-1) os << (mult>0?mult:-mult) << "*" ;
    os << "<";
    for (it = elts.begin(),jt = dom->Elts().begin();it != elts.end();it++,jt++) { 
   
      it->ExportToCami(os,*jt);
      if (it != --elts.end())
	os << ",";
    }
    os << ">";
  } else {
    os << mult;
  }

}

void  CFuncTerm::ExportToGSPN (ostream &os) {
  list<CFuncElt >::iterator it;
  list<PNClass*>::iterator jt;

  if (dom && dom->Size()) {
    if (mult != 1 && mult != -1) os << (mult>0?mult:-mult) ;
    os << "<";
    for (it = elts.begin(),jt = dom->Elts().begin();it != elts.end();it++,jt++) { 
   
      it->ExportToGSPN(os,*jt);
      if (it != --elts.end())
	os << ",";
    }
    os << ">";
  } else {
    os << mult;
  }

}


ostream & operator<< (ostream & os , const CFuncTerm &m) {
  list<CFuncElt >::const_iterator it;

  if (m.dom && m.dom->Size()) {
    //    cerr << *m.dom ;
    if (m.mult != 1) os << m.mult << "*" ;
    os << "<";
    for (it = m.elts.begin();it != m.elts.end();it++) { 
      os << *it;
      if (it != -- m.elts.end())
	os << ",";
    }
    os << ">";
  } else {
    os << m.mult;
  }

  
  return os;
}
