#include "Element.h"


#include <cstdio>

//Implementation for class Element
//default constructor
Element::Element (int id,const string &desc) {
  this->desc=desc;
  this->id=id;
}

Element::Element (int id) {
  char buff[12];
  sprintf (buff,"%d",id);
  this->desc=string(buff);
  this->id=id;
}
// *******************basic operators

// comparison by operator==
bool operator==(const Element&a,const Element&b){ return a.id == b.id; }
// comparison by operator<
bool  operator<(const Element&a,const Element&b) { return a.id < b.id; }


// printing with operator<<
ostream& operator<<(ostream& os,const Element& e){
  os <<e.id << ":" << e.desc;
  return os;
}

int Element::ExportToCami(string &s) {
  s += desc;
  return desc.length();
}

int Element::ExportToGSPN (string &s) {
  if (desc[0] <= '9' && desc[0] >= '0')
    s+= "num";
  s += desc;
  return desc.length();
}
