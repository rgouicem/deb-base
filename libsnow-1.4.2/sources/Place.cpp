#include "Place.h"
#include "ParserWrapper.h"
#include "PNetIO.h"
#include <algorithm>

//Implementation for class Place
//default constructor
Place::Place (int id,long NumMac,const string & name,Domain *dom ) {
  this->id = id;
  this->numeroMacao=NumMac;
  this->name=name;
  this->dom=dom;
  marking=NULL;
}


//default destructor
Place::~Place (){}

// *******************basic operators


// comparison by operator==
int operator==(const Place& a,const Place& b){
  return (a.id==b.id);
}

// comparison by operator<
int operator<(const Place& a,const Place& b){
  return (a.id<b.id);
}



void Place::setMarking (const string &s, bool isGSPN ) {
  marking = ParserWrapper::ParseMarking(s,dom,isGSPN);
  //  std::cout << *marking << std::endl;
}


list<vector<Element> > Place::getAssymetry (PNClass *p) {

  if (!dom ||  (find(dom->Elts().begin(),dom->Elts().end(),p) == dom->Elts().end() ) || !marking )
    return list<vector<Element> > ();
  else 
    return (marking->getAssymetry(p));  
}


void Place::RewriteWithStaticClasses () {  
  if (marking) marking->RewriteWithStaticClasses ();
}

void Place::RewriteWithDynamicClasses () {  
  if (marking) marking->RewriteWithDynamicClasses ();
}





// printing with operator<<
ostream& operator<<(ostream& os,const Place& p){
  os << "Place "<<p.name<<" internal id="<<p.id<<" MacaoNum="<<p.numeroMacao;
  if (p.dom) os << " "<< *(p.dom);
  if (p.marking != NULL) os << " marking:\""<<*p.marking<<"\"";
  return os;
}

// Add an arc 
int Place::AddArc (Arc *pa) {
  if (pa->dir == Trans2Place) 
    arcIn.push_back(pa);
  else 
    arcOut.push_back(pa);
  return 1;
}





int Place::ExportToCami (ostream &os) {
  os << "CN(5:place," << numeroMacao << ")\n";
  os << pos.getCamiPO(numeroMacao) << endl;
  if (name != "") {
    os << "CT(4:name," << numeroMacao << ","<< CamiFormat(name) << ")\n";
    os << namepos.getCamiPT(numeroMacao,"name") << endl;
  }
  if (dom && dom->Size()) {
    os << "CT(6:domain," << numeroMacao << ","<< CamiFormat(dom->Name()) << ")\n";
    os << dompos.getCamiPT(numeroMacao,"domain") << endl;
  }
/*  else  */
/*    os << "CT(6:domain," << numeroMacao << ",4:null)\n"; */
  if (marking != NULL) {
    os << "CT(7:marking," << numeroMacao << "," << CamiFormat(marking->ExportToCami()) << ")\n";
    os << markpos.getCamiPT(numeroMacao,"marking") << endl;
  }
  return 0;
}

void Place::ExportMarkGSPN(ostream &os,int & numligne) {
  if ( marking != NULL && dom->Name() != "null") {
    os << "(" ;
    os << "M_" << name << " m" ; 
    // export positions
    os << " " << markpos.getGspn() << " (@m\n";
    // <S Lent,0> + <S rapide, 0> 
    // <S C1> + <S C2> + ...
    os << marking->ExportToGSPN() ;
    os << "\n))" << endl ;
    numligne ++;
  }

}

void Place::ExportToGSPN(ostream &os,int numligne ) {
  bool isBW = !dom || (dom->Name() == "null");

  os << name << "    " ;
  if ( marking == NULL ) { os << "0 "; }
  else if (isBW) { os << *marking << " " ; } //export number of marks for B/W places
  else { os << "-" << numligne+10000 << " " ; }
  os << pos.getGspn()  ; // pos of place circle center
  os << namepos.getGspn() ; // pos of place name tag 
  os << "0 "; // free 0 !!! gratos !! "&@`#  GSPN
  if (!isBW) {
    os << dompos.getGspn() ; // pos of domain tag
    dom->ExportToGSPN (os);
  }
}
