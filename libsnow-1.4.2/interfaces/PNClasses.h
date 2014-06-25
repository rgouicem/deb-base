#ifndef _PNCLASSES_H_
#define _PNCLASSES_H_
#include <iostream>
#include <list>
#include "PNClass.h"
#include <utility>

//Declaration header file for class PNClasses


class PNClasses {
protected:
  list<PNClass> lst;
public:
  // *******************Constructor/destructor 
  //default constructor
  PNClasses ();
  //constructor by copy
  PNClasses (const PNClasses&);
  //default destructor
  virtual ~PNClasses ();

  // *******************basic operators
  // clone by operator=
  PNClasses& operator=(const PNClasses&);
  // comparison by operator==
  friend bool operator==(const PNClasses&,const PNClasses&);
  // comparison by operator<
  friend bool operator<(const PNClasses&,const PNClasses&);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const PNClasses&);


  // ****************** convenience accessors
  const int size () const {return lst.size();}

  const list<PNClass>& Lst() const {return lst;}
  list<PNClass>& Lst () { return lst; }
  // ADVANCED OPS
 

  /* insert sorted */
  PNClass* Insert(PNClass &);
  PNClass* Find(int);
  PNClass* FindName (const string & name);

  /* reindex classes and elements according to position in list */
  void Reindex () ;

  // splits classes into static subs, respecting both the current static sub partition (ie result will be finer) and l
  void MakeStatic (list<vector<Element> > l, PNClass * pC);
  // run make static before make dynamic and ensure l is a finer partition than the current partition in static subs
  // should make sure no dynamic subs are already defined ...
  void MakeDynamic (list<vector<Element> > l, PNClass * pC);
  void MakeStatic ();

  int ExportToCami(ostream &,int &);
  void ExportToGSPN (ostream & deff);
};
#endif

