#ifndef _ELEMENT_H_
#define _ELEMENT_H_
#include <iostream>
//#include <function>
#include <string>
#include <vector>
#include <utility>
#include <functional>
using namespace std;


  

//Declaration header file for class Element
class Element {
protected:
  int id;
public:
  string desc;

  // *******************Constructor/destructor 
  //default constructor
  Element (int,const string &);
  Element (int=-1);
  //default destructor
  virtual ~Element () {};
  

  // *******************basic operators

  int Id() const {return id;}
  void setid (int i)  { id = i ; }
  // comparison by operator==
  friend bool operator==(const Element&a,const Element&b);
  // comparison by operator<
  friend bool  operator<(const Element&a,const Element&b);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const Element&);
  friend ostream& operator<<(ostream&,vector<Element>&);

  int ExportToCami(string &s);
  int ExportToGSPN(string &s);

};


#endif

