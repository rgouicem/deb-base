#ifndef _VARIABLES_H_
#define _VARIABLES_H_
#include <iostream>
#include <list>

#include "Variable.h"
//Declaration header file for class Variables


class Variables {
protected:
  list<Variable> lst;
public:
  // *******************Constructor/destructor 
  //default constructor
  Variables ();

  //default destructor
  virtual ~Variables ();

  // *******************basic operators


  //printing with operator<<
  friend ostream& operator<<(ostream&,const Variables&);

  // ****************** convenience accessors
  const int size () const { return lst.size(); }

  const list<Variable>& Lst() const {return lst;}
  list<Variable>& Lst () {return lst;}

  // ADVANCED OPS
  /* insert sorted */
  Variable* Insert(const Variable &);
  Variable* Find(int);
  list<Variable*> Find(PNClass * pclass);
  Variable* FindName (const string &);
  int ExportToCami(ostream &,int &); 
};
#endif

