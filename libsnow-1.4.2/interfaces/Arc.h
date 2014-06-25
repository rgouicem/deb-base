#ifndef _ARC_H_
#define _ARC_H_
#include <iostream>

#include "Position.h"
#include "CFunc.h"
class Place ;
class Transition ;
//Declaration header file for class Arc
using namespace std ;

/* Direction of an arc */
enum Direction {
  Trans2Place,
  Place2Trans
};

class Arc {
protected:
  int id;
  long numeroMacao;
  // have to specify class keyword because of cross includes 
  class Place *place;
  class Transition *trans;
  bool isInhibitor;
  ///////////////////VALUATION
  CFunc valuation;
  /* inflexion points */
  list<Position> pi;
public:
  Position valpos;
  Direction dir;

  // *******************Constructor/destructor 
  //default constructor
  Arc (int id=-1,Place *p=NULL,Transition *t=NULL,Direction dir=Trans2Place,const string &valuation="1",long NumMacao=-1,bool isInhibitor=false,bool isGSPN =false);
  
  //constructor by copy : Use compiler generated
  //  Arc (const Arc&);
  //default destructor
  virtual ~Arc ();

  // *******************basic operators
  // clone by operator=
  Arc& operator=(const Arc&);
  friend int operator==(const Arc&,const Arc&);
  // comparison by operator<
  friend int operator<(const Arc&,const Arc&);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const Arc&);

  /* Get Assymetry */
  list<vector<Element> > getAssymetry (PNClass *p) ;
  /* Get references to successor operator */
  void FindOrderedClasses();
  set<Variable *> FindMentionedVars () { set<Variable *> ret; valuation.FindMentionedVars (ret); return ret;}
  void RewriteWithStaticClasses ();


  // accessors call with arg to set, no args to get
  void addInflexion (const Position &p) { pi.push_back(p) ;}
  
  const long NumeroMacao() const {return numeroMacao;}
  void NumeroMacao(const long n) {numeroMacao = n;}
  //  long NumeroMacao (long n=-1) {if (n!=-1) numeroMacao=n;return numeroMacao;}

  const int Id() const {return id;}
  void Id(const int n) {id = n;}
  //  int  Id (int n=-1) {if (n!=-1) id = n ; return id; }

  const CFunc& Valuation() const {return valuation;}
  CFunc& Valuation () {return valuation;}

  void setValuation(const string &s);
  const bool IsInhibitor () const {return isInhibitor; }
  Transition * getTrans () const {return trans;}
  Place* getPlace () const {return place;}

  // Export Primitives
  int ExportToGSPN(ostream &,ostream&,int &);
  int ExportToCami (ostream &);
  int ExportToPnddd (ostream &);
  int ExportToSmart (ostream &);
};

#endif


