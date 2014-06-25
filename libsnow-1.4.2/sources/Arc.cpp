#include "Arc.h"

#include "Transition.h"
#include "Place.h"

#include "PNetIO.h"
#include "ParserWrapper.h"
#include <sstream>
#include <algorithm>

//Implementation for class Arc
//default constructor
Arc::Arc (int id,Place *p,Transition *t,Direction dir,const string &valuation,long NumMacao,bool isInhibitor,bool isGSPN) {
  this->id=id;
  place = p;
  trans = t;
  this->dir =dir;
  numeroMacao = NumMacao;
  this->isInhibitor =isInhibitor;


  this->valuation=* ParserWrapper::ParseFunc(valuation,p->Dom(),isGSPN,this);
}



//default destructor
Arc::~Arc (){}

// *******************basic operators

// clone by operator=
Arc& Arc::operator=(const Arc& a){
  if (this != &a) {
      id= a.id;
      numeroMacao = a.numeroMacao;
      place = a.place;
      trans = a.trans;
      dir = a.dir;
      valuation=a.valuation;
  }
  return *this;
}

void Arc::setValuation(const string &s) {
  this->valuation=* ParserWrapper::ParseFunc(s,place->Dom());

}

void Arc::FindOrderedClasses () {
  valuation.FindOrderedClasses ();
}

// comparison by operator==
int operator==(const Arc& a,const Arc& b){
  return (a.id==b.id  && a.place==b.place && a.trans == b.trans);
}

// comparison by operator<
int operator<(const Arc& a,const Arc& b){
  return (a.id < b.id  // by id
	  || ( a.id == b.id && ( a.place < b.place  // equal id => by place
				 || ( a.place == b.place && a.trans < b.trans ) // equal id and place => by trans
				 )
	       )
	  );
  // hopefully no two arcs from place P to trans T with the same id
}

void Arc::RewriteWithStaticClasses () {  
  valuation.RewriteWithStaticClasses ();
}




// printing with operator<<
ostream& operator<<(ostream& os,const Arc& a){
  if (!a.isInhibitor) os << "Arc id=" ;
  else os << "Inhibitor Arc id=" ;
  os << a.id  ;
  os <<" MacaoNum="<<a.numeroMacao;
  os << " connects " ;
  if (a.dir == Trans2Place) {
    os << "Transition :" << a.trans->Name() << " to Place: "<< a.place->Name() ;
  } else {
    os  << "Place: " << a.place->Name() << " to Transition:"<< a.trans->Name() ;
  } 
  os<< "; valuation " << a.valuation << ";";
  
  return os;
}

int Arc::ExportToCami (ostream &os) {
  if (!isInhibitor) os << "CA(3:arc," << numeroMacao << "," ;
  else os << "CA(13:inhibitor arc," << numeroMacao << "," ;
  if ( dir == Trans2Place) 
    os << trans->NumeroMacao() <<"," << place->NumeroMacao() ;
  else os << place->NumeroMacao() <<"," << trans->NumeroMacao();
  os <<")\n";
  string v = valuation.ExportToCami();
  if (( v != "1") && (v.size() != 0)) {
    os << "CT(9:valuation,"<<numeroMacao <<"," << CamiFormat(v) << ")\n";
    os << valpos.getCamiPT(numeroMacao,"valuation") << endl;
  }
  return 0;
}

int Arc::ExportToSmart (ostream &os) {
  if (!isInhibitor)  {
    if ( dir == Trans2Place) 
      os << trans->Name() <<":" << place->Name() ;
    else os << place->Name() <<":" << trans->Name();
  } else {
    os <<  place->Name() <<":" << trans->Name();
  }
  string v = valuation.ExportToCami();
  if ( v!= "1") {
    os << ":" << v << "\n";
  }
  return 0;
}

int Arc::ExportToPnddd (ostream &os) {
  os <<  place->Name() ;

  string v = valuation.ExportToCami();
  if ( ! isInhibitor ) {
    os << ":";
    if ( v!= "1") {
      os << v ;
    }
    os << "<..>;";
  } else {
    os << "<" << v << ";" ;
  }
  return 0;
}


int Arc::ExportToGSPN (ostream &os,ostream &deff,int & numligne) {
  
  if (!place->Dom() || place->Dom()->Name() == "null") {
    os << "   ";
    os << valuation.getMultiplicityBWfunc () << "   "; // multiplicity of the arc
    os << place->Id()+1 << "   "; 
    os << pi.size() << " " ; // number of inflexion points
    os << "0" ; // delimiter gratos cimer GSPN. NOTE : no whitespace after this 0 or gspn will crash !
    os << endl;
  } else {
    os << "   ";
    os << "1   "; // multiplicity of the arc
    os << place->Id()+1 << "   "; 
    os << pi.size() << " " ; // number of inflexion points
    os << "0 " ; // delimiter gratos cimer GSPN
    os << valpos.getGspn() ; // function color position
    os << valuation.ExportToGSPN(); // color function
/*    // This block outputs the color function in .def file
    {
      ostringstream ost;
      ost << "F" << numligne++;
      string fname = ost.str();
      
      os << fname;

      deff <<  "(" << fname << " f " ;
      deff << "1.0 1.0 (@f\n" ;
      deff << valuation.ExportToGSPN(); // color function
      deff << "\n))\n";
    }
*/
    os << endl;
  }
  for (list<Position>::iterator it = pi.begin() ; it!=pi.end() ; it++ )
    os << it->getposV().first << " " << it->getposV().second << endl ;
  
  return 1;
}


list<vector<Element> > Arc::getAssymetry (PNClass *p) {

  if (!place->Dom() ||  (find(place->Dom()->Elts().begin(),place->Dom()->Elts().end(),p) == place->Dom()->Elts().end() ))
    return list<vector<Element> > ();
  else 
    return (valuation.getAssymetry(p));  
}
