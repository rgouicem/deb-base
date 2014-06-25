#ifndef _PLACE_H_
#define _PLACE_H_
#include <iostream>
#include <list>
#include <string>

#include "Marking.h"
#include "Arc.h"
#include "Domain.h"
#include "Position.h"
//Declaration header file for class Place
using namespace std ;

class Place {
protected:
  int id;
  long numeroMacao;
  string name;
  Domain *dom;
  Position pos;
  // MARKING *****************************
  Marking * marking;
  list<class Arc*> arcIn;
  list<class Arc*> arcOut;
public:
  /* public for convenience */
  Position markpos,namepos,dompos;
  // *******************Constructor/destructor 
  //default constructor
  Place (int id=-1,long NumMac=-1,const string & name="",Domain *dom =NULL);

  //default destructor
  virtual ~Place ();

  // *******************basic operators
  // comparison by operator==
  friend int operator==(const Place&,const Place&);
  // comparison by operator<
  friend int operator<(const Place&,const Place&);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const Place&);
  
  // accessor
  void setPos (const Position& p) { pos = p; }
  // call with no args to get , with arg to set

  Domain* const Dom() const {return dom;}
  void Dom(Domain* const d) {dom = d;}
  //  Domain* Dom (Domain *d=NULL) { if (!d) return dom; else return (dom = d); }

  const long NumeroMacao() const {return numeroMacao;}
  void NumeroMacao(const long n) {numeroMacao = n;}
  //  long NumeroMacao (int n=-1) {if (n!=-1) numeroMacao=n;return numeroMacao;}

  const string Name() const {return name;}
  void Name(const string& s) {name = s;}
  //  string Name (const string &s="I WANT GET") { if (s!="I WANT GET") name=s;return name;}

  // Add an arc 
  int AddArc (Arc *pa);
  // accessor
  void setMarking (const string &s, bool isGSPN = false) ;
  void setMarking (Marking* const m) {marking = m;}
  Marking* const getMarking () const {return marking;}

  const int Id () const {return id;}
  void Id(const int n) {id = n;}
  //  int  Id (int n=-1) {if (n!=-1) id = n ; return id; }


  list<vector<Element> > getAssymetry (PNClass *p) ;
  void RewriteWithStaticClasses ();
  void RewriteWithDynamicClasses ();


  void ExportToGSPN(ostream &os,int);
  void ExportMarkGSPN(ostream &os,int &);
  int ExportToCami (ostream &);

  const list<Arc*>& getArcIn () const {return arcIn;}
  const list<Arc*>& getArcOut () const {return arcOut;}
  list<Arc*>& getArcIn () {return arcIn;}
  list<Arc*>& getArcOut () {return arcOut;}
};

#endif

