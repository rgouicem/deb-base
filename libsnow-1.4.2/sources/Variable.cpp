#include <string>
#include <sstream>
#include "Variable.h"


//Implementation for class Variable
//default constructor
Variable::Variable (int id,string name,PNClass *pClass) {
  this->id =id;
  this->name=name;
  this->pClass = pClass; 
}

//default destructor
Variable::~Variable (){}

// *******************basic operators


// comparison by operator==
int operator==(const Variable& a,const Variable& b){
  if (a.id != -1 && b.id!= -1) 
    return (a.id == b.id);
  return (a.name == b.name ) ;
}

// comparison by operator<
int operator<(const Variable& a,const Variable &b){
  return (a.id < b.id);
}

// printing with operator<<
ostream& operator<<(ostream& os,const Variable& v){
  os << "Variable number "<< v.id << " name \""<< v.name << "\" of class "<< v.pClass->Name() ;
  return os;
}

string Variable::ExportToGSPN () {
  ostringstream ost ;
  if (name == "S") {
    ost << "varS";
  } else
    ost << name ;
  return ost.str();
}
