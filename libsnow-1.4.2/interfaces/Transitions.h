#ifndef _TRANSITIONS_H_
#define _TRANSITIONS_H_
#include <iostream>
#include <utility>


#include "Transition.h"
//Declaration header file for class Transitions
class Transition;

class Transitions {
protected:
  list<Transition> lst;
public:
  // *******************Constructor/destructor 
  //default constructor
  Transitions ();
  //constructor by copy
  Transitions (const Transitions&);
  //default destructor
  virtual ~Transitions ();

  
  // *******************basic operators
  // clone by operator=
  Transitions& operator=(const Transitions&);
  // comparison by operator==
  friend int operator==(const Transitions&,const Transitions&);
  // comparison by operator<
  friend int operator<(const Transitions&,const Transitions&);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const Transitions&);

  // ****************** convenience accessors
  const int size () const { return lst.size(); }

  const list<Transition>& Lst() const {return lst;}
  list<Transition>& Lst () {return lst;}
  // ADVANCED OPS
  /* insert sorted */
  Transition* Insert(Transition &);
  /*find by internal id */
  Transition * Find(int id);
  /*find by Macao num */
  Transition * FindMacao(long macaoNum);
  /* find by name */
  Transition * FindName(const string &n);
  /* reindex / construct macaoIds */
  long reIndex (long curMacaoid) ;
  /* construct default name for unnamed transitions */
  void renameTrans ();


  list<vector<Element> > getAssymetry (Variable *v);
  void RewriteWithStaticClasses();
  void FindOrderedClasses();

  void BindVariablesToStaticClasses();

  int ExportToCami(ostream &);
  int ExportToGSPN(ostream &,ostream &,int &);
  int ExportToSmart(ostream &);
  int ExportToPnddd(ostream &os);

};
#endif

