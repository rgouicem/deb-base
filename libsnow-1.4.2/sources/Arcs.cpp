#include "Arcs.h"
#include <algorithm>
#include "PNetIO.h"
#include "CalcSubClasses.h"

//Implementation for class Arcs
//default constructor
Arcs::Arcs () {

}

//default destructor
Arcs::~Arcs (){}

// *******************basic operators

// clone by operator=
Arcs& Arcs::operator=(const Arcs& a){
  if (this != &a)
    elts = a.elts;
  return *this;
}



// printing with operator<<
ostream& operator<<(ostream& os,const Arcs& p){
  Arcs::collection_t::const_iterator it;
  os<<"Arcs: {"<<endl;
  for (it = p.elts.begin();it != p.elts.end();it++) {
    os << *it << endl; 
  }
  os<<"} End Arcs"<<endl;
  return os; 
}


// **********Advanced ops
/* sorted insert */
Arc* Arcs::Insert (const Arc &a) {
  /*
  list<Arc>::iterator it;
  it = upper_bound(lst.begin(),lst.end(),a);
  lst.insert(it,a);
  it--;
  return &(*it);
  */
  pair<collection_t::iterator, bool> i = elts.insert(a);
  return const_cast<Arc*>(&(*i.first));
}

/*find by id */
Arc * Arcs::Find(int id) {
  /*
  list<Arc>::iterator it;
  it=find(lst.begin(),lst.end(),Arc(id));
  if (it!=lst.end()) return &(*it);
  else return NULL;
  */
  collection_t::iterator i = find(elts.begin(), elts.end(), Arc(id));
  if (i == elts.end())
    return NULL;
  else
    return const_cast<Arc*>(&(*i));
}

/*find by MacaoNum */
Arc * Arcs::FindMacao (long macaoNum) {
  /*
  list<Arc>::iterator it;
  class FindMacao fm(macaoNum);
  it=find_if(lst.begin(),lst.end(),fm);
  if (it!=lst.end()) return &(*it);
  else return NULL;
  */
  class FindMacao fm(macaoNum);
  collection_t::iterator i = find_if(elts.begin(), elts.end(), fm);
  if (i == elts.end())
    return NULL;
  else
    return const_cast<Arc*>(&(*i));
}

long Arcs::reIndex (long curMacaoid) {
  /*
  list<Arc>::iterator it;
  int i;
  for (it = lst.begin(),i=0;it!= lst.end();it++,i++ ) {
    it->Id(i) ;
    if (it->NumeroMacao() == -1) { it->NumeroMacao(++curMacaoid); }
    else if (it->NumeroMacao() >= curMacaoid) { curMacaoid = it->NumeroMacao()+1 ; }
  }
  return curMacaoid;
  */
  collection_t::iterator it;
  int i;
  for (it = elts.begin(), i=0;
       it != elts.end();
       it++, i++)
    {
      const_cast<Arc&>(*it).Id(i);
      if (const_cast<Arc&>(*it).NumeroMacao() == -1)
	const_cast<Arc&>(*it).NumeroMacao(++curMacaoid);
      else if (const_cast<Arc&>(*it).NumeroMacao() >= curMacaoid)
	curMacaoid = const_cast<Arc&>(*it).NumeroMacao()+1;
    }
  return curMacaoid;
}


void Arcs::FindOrderedClasses () {
  /*
  list<Arc>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->FindOrderedClasses();
  }
  */
  for (collection_t::iterator it = elts.begin();
       it != elts.end();
       ++it)
    const_cast<Arc&>(*it).FindOrderedClasses();
}

void Arcs::RewriteWithStaticClasses () {
  /*
  list<Arc>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->RewriteWithStaticClasses();
  }
  */
  for (collection_t::iterator it = elts.begin();
       it != elts.end();
       ++it)
    const_cast<Arc&>(*it).RewriteWithStaticClasses();
}



int Arcs::ExportToCami(ostream &os) {
  /*
  list<Arc>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->ExportToCami(os);
  }
  return 0;
  */
  for (collection_t::iterator it = elts.begin();
       it != elts.end();
       ++it)
    const_cast<Arc&>(*it).ExportToCami(os);
  return 0;
}


list<vector<Element> > Arcs::getAssymetry (PNClass *p) {
  /*
  list<Arc>::iterator it;
  list<vector<Element> > lres,lret;

  lret.push_back(p->Elts());
  for (it = lst.begin() ; it!= lst.end() ; it++ ) {
    lres = it->getAssymetry(p);
    lret.insert(lret.end(),lres.begin(),lres.end()) ;
  }

  lret = calcSub::uniquePartition (lret);

  return lret;
  */
  list< vector<Element> > lres, lret;
  lret.push_back(p->Elts());
  for (collection_t::iterator it = elts.begin();
       it != elts.end();
       ++it)
    {
      lres = const_cast<Arc&>(*it).getAssymetry(p);
      lret.insert(lret.end(), lres.begin(), lres.end());
    }
  lret = calcSub::uniquePartition(lret);
  return lret;
}
