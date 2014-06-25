#ifndef _TRANSITION_H_
#define _TRANSITION_H_
#include <iostream>
#include <list>
#include <string>
#include <utility>
#include "Arc.h"
#include "Domain.h"
#include "Guard.h"
#include "Position.h"
//Declaration header file for class Transition


class Transition {
 protected:
  int id;
  long numeroMacao;
  string name;
//  Domain *dom;
  Guard * guard;
  
  int priority;
  int nserv;
  // have to specify keyword class because of cross includes
  list<class Arc*> arcIn;
  list<class Arc*> arcOut;
  Position pos;
 public:
  Position guardpos,namepos;
  // *******************Constructor/destructor 
  //default constructor
  Transition (int id=-1,long NumMac=-1,string name="",/*Domain *dom =NULL,*/int priority=0);
  Transition (const Transition & t);
  //default destructor
  virtual ~Transition ();

  // *******************basic operators

  // comparison by operator==
  friend bool operator==(const Transition&,const Transition&);
  // comparison by operator<
  friend bool operator<(const Transition&,const Transition&);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const Transition&);

  // Add an arc 
  int AddArc (Arc *pa);
  // accessor
  Guard* setGuard (const string &,bool isGSPN = false);
  void setGuard(Guard* const guard) {this->guard = guard;}

  Guard* const getGuard() const {return this->guard;}

  void setPos (const Position& p) { pos = p; }

  const long NumeroMacao() const {return numeroMacao;}
  void NumeroMacao(const long n) {numeroMacao = n;}
  //  long NumeroMacao (int n=-1) {if (n!=-1) numeroMacao=n;return numeroMacao;}

  const int Id() const {return id;}
  void Id(const int n) {id = n;}
  //  int  Id (int n=-1) {if (n!=-1) id = n ; return id; }

  const string Name() const {return name;}
  void Name(const string s) {name = s;}
  //  string Name (const string &s="I WANT GET") { if (s!="I WANT GET") name=s;return name;}

  const int Priority() const {return priority;}
  void Priority(const int n) {priority = n;}
  //  int Priority(int n=-1) {if (n!=-1) priority=n;return priority;}

  const int Nserv() const {return nserv;}
  void Nserv(const int n) {nserv = n;}
  //  int Nserv (int n =-1) {if (n!=-1) nserv=n;return nserv;}

  inline list<vector<Element> > getAssymetry (Variable *v) {return (guard->getAssymetry(v));}
  void FindOrderedClasses();
  void  RewriteWithStaticClasses();

  int ExportToGSPN (ostream &,ostream &,int &);
  int ExportToCami (ostream &);
  int ExportToSmart (ostream &);
  int ExportToPnddd (ostream &);


  const list<Arc*>& getArcIn () const {return arcIn;}
  const list<Arc*>& getArcOut () const {return arcOut;}
  list<Arc*>& getArcIn () {return arcIn;}
  list<Arc*>& getArcOut () {return arcOut;}
};
#endif

