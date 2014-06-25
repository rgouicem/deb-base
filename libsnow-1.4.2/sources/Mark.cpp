#include "Mark.h"
#include "CalcSubClasses.h"
#include "PNClass.h"

list<vector<Element> > Mark::getAssymetry (PNClass *p) {
  list<PNClass *>::iterator clit;
  list<vector<Element> >::iterator it;
  list<vector<Element> >  lres;

  for (clit = dom->Elts().begin() , it = elts.begin() ; clit != dom->Elts().end()  ; clit ++,it++ ) {
    if (*clit == p) {
      lres.push_back(*it);
    }
  } 
  lres = calcSub::uniquePartition (lres);

  return lres;

}


void  Mark::ExportToGSPN (ostream &os) {
 
  list<vector<Element> >::const_iterator it;
  list<PNClass*>::iterator jt;

  if (dom && dom->Size() ) {
    if (mult != 1) os << mult << " " ;
    os << "<";
    for (it = elts.begin(),jt = dom->Elts().begin();it != elts.end();it++,jt++) { 
      PNClass *pc = (*jt)->FindDynSubContaining(*it);
      if (pc) {
	if (pc->Type() == Statique)
	  os << "S " << pc->Name();
	else
	  os << pc->Name() ;
      } else {
	if (it->size() > 1) {
	  os << "{";
	  calcSub::print(os,*it,false);
	  os << "}.ALL";
	} else 
	  os << it->front().desc ;
      }
      if (it != --elts.end())
	os << ",";
    }
    os << ">";
  } else {
    os << mult;
  }


}




void  Mark::ExportToCami (ostream &os) {
 
  list<vector<Element> >::const_iterator it;
  list<PNClass*>::iterator jt;

  if (dom && dom->Size() ) {
    if (mult != 1) os << mult << "*" ;
    os << "<";
    for (it = elts.begin(),jt = dom->Elts().begin();it != elts.end();it++,jt++) { 
      PNClass *pc = (*jt)->FindSubContaining(*it);
      if (pc) {
	os << pc->Name();
	os << ".ALL";
      } else {
     if (it->size() > 1) {
	os << "{";
	calcSub::print(os,*it,false);
	os << "}.ALL";
     } else 
	os << it->front().desc ;
      }
      if (it != --elts.end())
	os << ",";
    }
    os << ">";
  } else {
    os << mult;
  }


}


list<class Mark> Mark::RewriteWithSimpleTokens () {
  list<Mark> ret;
  // example : <{1,2},{a,b,c},{x}> -> <{2},{c},{x}> + <{2},{b},{x}> + <{2},{a},{x}> +  <{1},{c},{x}> + <{1},{b},{x}> + <{1},{a},{x}> 
  
  list<pair <int,int> > pos;
  for (list<vector<Element> >::iterator it = elts.begin() ; it != elts.end() ; it++) {
    pos.push_back( make_pair(0,it->size()) ); 
  }  
  
  list< pair<int,int> >::iterator pit;
  list<vector<Element> >::iterator cit;
  do {
    list<vector<Element> > forret;
    
    for (pit = pos.begin(),cit = elts.begin() ; pit != pos.end() ;pit++,cit++) {
      forret.push_back(vector<Element> (1,(*cit)[pit->first]));
    }
    ret.push_back ( Mark(dom,forret,mult));
  } while (! calcSub::next(pos) );

  
  return ret;

}

list<class Mark> Mark::RewriteWithDynamicClasses () {
  return RewriteWithStaticClasses ();
}

list<class Mark> Mark::RewriteWithStaticClasses () {

  list<Mark> ret;
  list<vector<Element> >::iterator it;
  list<PNClass *>::iterator jt;
  
  if (!dom || !dom->Size()) {
    ret.push_back(*this);
    return ret;
  }

  list<vector<vector<Element> > > compose;
  list<pair <int,int> > pos;
  // i.e: m= < {1,2} , {a,b}, x >
  // C = {1},{2} ; D = {a,b} ; E = {x},{y,z}
  // compose = { {[1],[2]}, {[a,b]},{[x]} }


  for (it = elts.begin(),jt = dom->Elts().begin();it != elts.end();it++,jt++) { 
    if (it->size() < 2) {
      vector<vector<Element> > cur;
      cur.push_back(*it);
      compose.push_back(cur);
      pos.push_back( make_pair(0,1) );
    } else {
      list<vector<Element> > subs = (*jt)->getDynSubSets();
      vector<vector<Element> > cur = calcSub::SeparateInto (*it,subs);
      compose.push_back(cur);
      pos.push_back( make_pair(0,cur.size()) );
    }
  }

  list< pair<int,int> >::iterator pit;
  list<vector<vector<Element> > >::iterator cit;

  do {
    list<vector<Element> > forret;

    for (pit = pos.begin(),cit = compose.begin() ; pit != pos.end() ;pit++,cit++) {
      forret.push_back((*cit)[pit->first]);
    }
    ret.push_back ( Mark(dom,forret,mult));
  } while (! calcSub::next(pos) );
  
  return ret;

}


ostream & operator<< (ostream & os , const Mark &m) {
  list<vector<Element> >::const_iterator it;

  if (m.dom && m.dom->Size() ) {
    if (m.mult != 1) os << m.mult << "*" ;
    os << "<";
    for (it = m.elts.begin();it != m.elts.end();it++) { 
      if (it->size() > 1) {
	os << "{";
	calcSub::print(os,*it,false);
	os << "}.ALL";
      } else 
	os << it->front().desc ;
      if (it != -- m.elts.end())
	os << ",";
    }
    os << ">";
  } else {
    os << m.mult;
  }


  return os;
}
