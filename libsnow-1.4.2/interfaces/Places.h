#ifndef _PLACES_H_
#define _PLACES_H_
#include <iostream>
#include <list>
#include <vector>

#include "Element.h"
#include "Place.h"
using namespace std;
//Declaration header file for class Places


class Places {
protected:
  list<Place> lst;
public:
  // *******************Constructor/destructor 
  //default destructor
  virtual ~Places ();

  // *******************basic operators
  // clone by operator=
  Places& operator=(const Places&);
  // comparison by operator==
  friend int operator==(const Places&,const Places&);
  // comparison by operator<
  int operator<(const Places&);
  friend int operator<(const Places&,const Places&);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const Places&);

  // ****************** convenience accessors
  const int size () const {return lst.size();}

  const list<Place>& Lst() const {return lst;}
  list<Place>& Lst () { return lst;}

  // ADVANCED OPS
  /* insert sorted */
  int Insert(Place &);
  /* find by internal id */
  Place* Find(int);
  /* find by macao id */
  Place* FindMacao(long);
  /* find by name */
  Place* FindName(const string &n);

  /* reindex / construct macaoIds */
  long reIndex (long curMacaoid) ;
  /* setDefaultName : construct default names for unnamed places */
  void renamePlaces () ;

  list<vector<Element> > getAssymetry (PNClass *p) ;
  void RewriteWithStaticClasses ();
  void RewriteWithDynamicClasses ();


  int ExportToCami(ostream &);
  int ExportToGSPN(ostream &deff,ostream &netff,int &numligne);
  int ExportToSmart(ostream &);
  int ExportToPnddd(ostream &);
};
#endif

