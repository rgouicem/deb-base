#include "Transitions.h"
#include <algorithm>
#include "PNetIO.h"
#include "CalcSubClasses.h"


//Implementation for class Transitions
//default constructor
Transitions::Transitions () {

}


//default destructor
Transitions::~Transitions (){}

// *******************basic operators

// clone by operator=
Transitions& Transitions::operator=(const Transitions& t){
  if (this != &t) {
    lst = t.lst;
  }
  return *this;
}

// comparison by operator==
int operator==(const Transitions& a,const Transitions& b){
  list<Transition>::const_iterator ita,itb;
  if ( a.lst.size() != b.lst.size() )
    return 0;
  else 
    for (ita = a.lst.begin(),itb=b.lst.begin();ita!=a.lst.end() && itb!=b.lst.end();ita++,itb++) 
      if (!(*ita == *itb)) return 0;
  return 1;
}

void Transitions::FindOrderedClasses () {
  list<Transition>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->FindOrderedClasses();
  }

}


void  Transitions::RewriteWithStaticClasses() {
  list<Transition>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->RewriteWithStaticClasses();
  }
}


// comparison by operator<
int operator<(const Transitions& a,const Transitions& b){
  list<Transition>::const_iterator ita,itb;
  if ( a.lst.size() < b.lst.size() )
    return 1;
  else if ( a.lst.size() > b.lst.size() )
    return 0;
  else 
    for (ita = a.lst.begin(),itb=b.lst.begin();ita!=a.lst.end() && itb!=b.lst.end();ita++,itb++) 
      if (*ita < *itb) return 1;
  return 0;
}


list<vector<Element> > Transitions::getAssymetry (Variable *v) {
  list<Transition>::iterator it;
  list<vector<Element> > lres,lret;

  for (it = lst.begin() ; it!= lst.end() ; it++ ) {
    lres = it->getAssymetry(v);
    lret.insert(lret.end(),lres.begin(),lres.end()) ;
  }

  lret = calcSub::uniquePartition (lret);
//  calcSub::print(cerr,lret);
  return lret;

}


// printing with operator<<
ostream& operator<<(ostream& os,const Transitions& t){
  list<Transition>::const_iterator it;
  os<<"Transitions: {"<<endl;
  for (it = t.lst.begin();it != t.lst.end();it++) {
    os<<*it<< endl;
  }
  os<<"} End Transitions"<<endl;
  return os; 
  return os;
}

// **********Advanced ops
/* sorted insert */
Transition * Transitions::Insert (Transition &t) {
  list<Transition>::iterator it;
  it = upper_bound(lst.begin(),lst.end(),t);
  lst.insert(it,t);
  it--;
  return &(*it);
}
/*find by internal id */
Transition * Transitions::Find(int id) {
  list<Transition>::iterator it;
  it=find(lst.begin(),lst.end(),Transition(id));
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}

/*find by Macao num */
Transition * Transitions::FindMacao(long macaoNum) {
  list<Transition>::iterator it;
  class FindMacao fm(macaoNum);
  it=find_if(lst.begin(),lst.end(),fm);
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}

Transition* Transitions::FindName (const string &name) {
  list<Transition>::iterator it;
  class FindName fn(name);
  it=find_if(lst.begin(),lst.end(),fn);
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}

long Transitions::reIndex (long curMacaoid) {
  list<Transition>::iterator it;
  int i;
  for (it = lst.begin(),i=0;it!= lst.end();it++,i++ ) {
    it->Id(i) ;
    if (it->NumeroMacao() == -1) { it->NumeroMacao(++curMacaoid); }
    else if (it->NumeroMacao() >= curMacaoid) { curMacaoid = it->NumeroMacao()+1 ; }
  }
  return curMacaoid;
}



int Transitions::ExportToCami(ostream &os) {
  list<Transition>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->ExportToCami(os);
  }
  return 0;
}

int Transitions::ExportToSmart(ostream &os) {
  list<Transition>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->ExportToSmart(os);
  }
  return 0;
}

int Transitions::ExportToPnddd(ostream &os) {
  list<Transition>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->ExportToPnddd(os);
  }
  return 0;
}


int Transitions::ExportToGSPN(ostream &os,ostream &deff,int &numligne) {
  list<Transition>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->ExportToGSPN(os,deff,numligne);
  }
  return 0;
}



void Transitions::renameTrans () {

  list<Transition>::iterator it;
  set<string> names;
  for (it = lst.begin();it!= lst.end();it++ ) {
    if ( it->Name() != "" )
      names.insert(it->Name());
  }

  int i = 1;
  for (it = lst.begin();it!= lst.end();it++ ) {
    if ( it->Name() == "" ) {
      while (1) {
	stringstream s;
	s << "T_" << i++;
	if (names.find(s.str()) == names.end() ) {
	  it->Name(s.str());
	  break;
	}
      }
    }
  }


}
