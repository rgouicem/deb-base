#ifndef _ARCS_H_
#define _ARCS_H_
#include <iostream>
#include <list>
#include <vector>
#include <utility>
#include "Arc.h"
#include "Element.h"
#include "PNClass.h"
//Declaration header file for class Arcs


class Arcs {
 protected:
  typedef std::set<Arc> collection_t;
  collection_t elts;
 public:
  // *******************Constructor/destructor 
  //default constructor
  Arcs ();
  //constructor by copy
  Arcs (const Arcs&);
  //default destructor
  virtual ~Arcs ();


  // *******************basic operators
  // clone by operator=
  Arcs& operator=(const Arcs&);

  //printing with operator<<
  friend ostream& operator<<(ostream&,const Arcs&);
  // **********Advanced ops
  /* sorted insert */
  Arc * Insert (const Arc &a) ;
  /*find by id */
  Arc * Find(int ) ;
  /* find by macao id */
  Arc * FindMacao(long);
  /* reindex / construct macaoIds */
  long reIndex (long curMacaoid) ;

    // ****************** convenience accessors
  const int size () const {return elts.size();}
  

  void FindOrderedClasses();

  list<vector<Element> > getAssymetry (PNClass *p) ;
  void RewriteWithStaticClasses ();

  int ExportToCami(ostream &os);
  // Export to GSPN does not use this class, see Transition::ExportToGSPN for that

  const std::set<Arc>& get() const {return this->elts;}
  std::set<Arc>& get() {return this->elts;}
};
#endif

