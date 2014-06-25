#include "Places.h"
#include <algorithm>
#include <set>
#include "PNetIO.h"
#include "CalcSubClasses.h"

//Implementation for class Places



//default destructor
Places::~Places (){}

// *******************basic operators

// clone by operator=
Places& Places::operator=(const Places& p){
  if (this != &p) {
    lst = p.lst;
  }
  return *this;
}

// comparison by operator==
int operator==(const Places& a,const Places& b){
  list<Place>::const_iterator ita,itb;
  if ( a.lst.size() != b.lst.size() )
    return 0;
  else 
    for (ita = a.lst.begin(),itb=b.lst.begin();ita!=a.lst.end() && itb!=b.lst.end();ita++,itb++) 
      if (!(*ita == *itb)) return 0;
  return 1;
}

// comparison by operator<
int operator<(const Places& a,const Places& b){
  list<Place>::const_iterator ita,itb;
  if ( a.lst.size() < b.lst.size() )
    return 1;
  else if ( a.lst.size() > b.lst.size() )
    return 0;
  else 
    for (ita = a.lst.begin(),itb=b.lst.begin();ita!=a.lst.end() && itb!=b.lst.end();ita++,itb++) 
      if (*ita < *itb) return 1;
  return 0;
}

// printing with operator<<
ostream& operator<<(ostream& os,const Places& p){
  list<Place>::const_iterator it;
  os<<"Places: {"<<endl;
  for (it = p.lst.begin();it != p.lst.end();it++) {
    os<<*it<<endl; 
  }
  os<<"} End Places"<<endl;
  return os; 
}

// **********Advanced ops
/* sorted insert */
int Places::Insert (Place &p) {
  list<Place>::iterator it;
  it = upper_bound(lst.begin(),lst.end(),p);
  lst.insert(it,1,p);
  return 1;
}

/*find by id */
Place * Places::Find(int id) {
  list<Place>::iterator it;
  it=find(lst.begin(),lst.end(),Place(id));
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}


Place* Places::FindName (const string &name) {
  list<Place>::iterator it;
  class FindName fn(name);
  it=find_if(lst.begin(),lst.end(),fn);
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}

void Places::RewriteWithStaticClasses () {
  list<Place>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->RewriteWithStaticClasses();
  }

}
void Places::RewriteWithDynamicClasses () {
  list<Place>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->RewriteWithDynamicClasses();
  }

}

/*find by Macao num */
Place * Places::FindMacao(long macaoNum) {
  list<Place>::iterator it;
  class FindMacao fm(macaoNum);
  it=find_if(lst.begin(),lst.end(),fm);
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}


list<vector<Element> > Places::getAssymetry (PNClass *p) {
  list<Place>::iterator it;
  list<vector<Element> > lres,lret;

  lret.push_back(p->Elts());
  for (it = lst.begin() ; it!= lst.end() ; it++ ) {
    lres = it->getAssymetry(p);
    lret.insert(lret.end(),lres.begin(),lres.end()) ;
  }

  lret = calcSub::uniquePartition (lret);

  return lret;
}

long Places::reIndex (long curMacaoid) {
  list<Place>::iterator it;
  int i;
  for (it = lst.begin(),i=0;it!= lst.end();it++,i++ ) {
    it->Id(i) ;
    if (it->NumeroMacao() == -1) { it->NumeroMacao(++curMacaoid); }
    else if (it->NumeroMacao() >= curMacaoid) { curMacaoid = it->NumeroMacao()+1 ; }
  }
  return curMacaoid;
}


int Places::ExportToCami(ostream &os) {
  list<Place>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->ExportToCami(os);
  }
  return 0;
}

int Places::ExportToSmart (ostream &os) {
  
  /* Place declarations */
  os << "     place ";
  list<Place>::iterator it;
  for (it = lst.begin();it!= lst.end(); /*NOP, increment is in loop*/ ) {
    os << it->Name() ;
    /* Just a little bit of consistency control ... */
    if (it->Dom() != NULL && it->Dom()->Size() != 0 ) {
      cerr << " Smart output is only possible for GSPN. Colored nets are not currently supported. Error is fatal sorry. "<< endl ;
      exit (1);
    }
    it++ ;
    if (it != lst.end())
      os << ", ";
  }
  os << "; " << endl ;

  // partition (fine grain used )
  os << "     partition ( ";
  for (it = lst.begin();it!= lst.end(); /*NOP, increment is in loop*/ ) {
    os << it->Name() ;
    /* Just a little bit of consistency control ... */
    if (it->Dom() != NULL && it->Dom()->Size() != 0 ) {
      cerr << " Smart output is only possible for GSPN. Colored nets are not currently supported. Error is fatal sorry. "<< endl ;
      exit (1);
    }
    it++ ;
    if (it != lst.end())
      os << ", ";
  }
  os << "); " << endl ;


  /* Initial marking definition */
  os << "     init( ";
  bool first = true;
  for (it = lst.begin();it!= lst.end();it++ ) {
    if (it->getMarking() != NULL) {
      list<Mark> & l = it->getMarking()->Lst();
      if (! l.empty()) {
	if (! first) {
	  os << ", ";
	} else {
	  first = false;
	}
	os << it->Name() << ":" << l.begin()->Mult() ;
      }
    }
  }
  os << "); " << endl ;
  
  return 0;
}


int Places::ExportToPnddd (ostream &os) {
  
  /* Place declarations */
  list<Place>::iterator it;
  for (it = lst.begin();it!= lst.end();  it++  ) {
    os << "#place "<< it->Name() ;
    /* Just a little bit of consistency control ... */
    if (it->Dom() != NULL && it->Dom()->Size() != 0 ) {
      cerr << " Pnddd output is only possible for GSPN. Colored nets are not currently supported. Error is fatal sorry. "<< endl ;
      exit (1);
    }
    if (it->getMarking() != NULL) {
      list<Mark> & l = it->getMarking()->Lst();
      if (! l.empty()) {
	int n = l.begin()->Mult() ;
	os << " mk(" ;
	if (n>1)
	  os << n ;
	os << "<..>)";
      }
    }
    os << endl;
  }

  return 0;
}




int Places::ExportToGSPN(ostream &deff,ostream &netff,int &numligne) {

  list<Place>::iterator it;
  for (it = lst.begin();it!= lst.end();it++ ) {
    it->ExportMarkGSPN(deff,numligne);
    it->ExportToGSPN(netff,numligne);
    netff << endl ;
  }
  return 0;

}


void Places::renamePlaces () {

  list<Place>::iterator it;
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
	s << "P_" << i++;
	if (names.find(s.str()) == names.end() ) {
	  it->Name(s.str());
	  break;
	}
      }
    }
  }


}
