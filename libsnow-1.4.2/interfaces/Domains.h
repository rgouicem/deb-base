#ifndef _DOMAINS_H_
#define _DOMAINS_H_
#include <iostream>
#include <list>
#include <utility>
#include "Domain.h"
//Declaration header file for class Domains


class Domains {
protected:
  list<Domain> lst;
  int NextFreeDomId;
public:
  // *******************Constructor/destructor 
  //default constructor
  Domains ();
  //default destructor
  virtual ~Domains ();

  // *******************basic operators
  list<Domain> & Lst () { return lst; }
  const std::list<Domain>& Lst() const {return this->lst;}

  //printing with operator<<
  friend ostream& operator<<(ostream&,const Domains&);

  // ADVANCED OPS
  /* insert sorted (no duplicates) and return pointer to insert point*/
  Domain * Insert(Domain &);
  Domain * FindName(const string & name);
  Domain * FindNameList(const list<string> & l);

  
  int ExportToCami(ostream &,int &);
};
#endif

