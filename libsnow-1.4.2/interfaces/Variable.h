#ifndef _VARIABLE_H_
#define _VARIABLE_H_
#include <iostream>
#include <string>

#include "PNClass.h"
//Declaration header file for class Variable
using namespace std;

class PNClass;

class Variable {
protected:
  int id;
  string name;
  PNClass* pClass;
public:
  // *******************Constructor/destructor 
  //default constructor
  Variable (int id=-1,string name="",class PNClass *PClass=NULL);
  //default destructor
  virtual ~Variable ();

  // Accessors
  PNClass* PClass() const {return this->pClass;}
  void PClass(PNClass* pClass) {this->pClass = pClass;}
  //  class PNClass* PClass (class PNClass *p =NULL) {if (p!=NULL) pClass=p;return pClass;}

  const string Name() const {return name;}
  void Name(const string s) {name = s;}
  //  string Name (const string &s="I WANT GET") { if (s!="I WANT GET") name=s;return name;}

  const int Id () const {return id;}
 
  // *******************basic operators
  // comparison by operator==
  friend int operator==(const Variable&,const Variable&);
  // comparison by operator<
  friend int operator<(const Variable&,const Variable&);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const Variable&);

  // Export to cami uses operator<<
  string ExportToGSPN();
};
#endif

