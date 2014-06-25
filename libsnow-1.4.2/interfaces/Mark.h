#ifndef _MARK_H_
#define _MARK_H_

#include <vector>
#include <list>
#include <iostream>
 
#include "Element.h"
#include "Domain.h"
class PNClass;
using namespace std;

class Domain;

class Mark {
protected : 
  Domain * dom ;
  list<vector<Element> > elts;
  int mult ;
public :
  // colored marks
  Mark (Domain *pdom,list<vector<Element> >&l,int m= 1):dom(pdom),elts(l),mult(m){} ;
  // b & w marks
  Mark (int multi):dom(NULL),mult(multi) {};  
  virtual ~Mark () {};
  void ExportToCami (ostream &os) ;
  void ExportToGSPN (ostream &os) ;
  friend  ostream & operator<< (ostream & os , const Mark &m) ;

  const int& Mult() const {return mult;}
  int& Mult () { return mult; }

  // WARNING !! Assumes that Mark is more "grouped" than static subs
  // ie: Mark <1,a> is only possible if 1 and a are isolated in static subs
  // is ok if called after getAssymetry...
  list<class Mark> RewriteWithStaticClasses ();
  list<class Mark> RewriteWithDynamicClasses ();
  list<class Mark> RewriteWithSimpleTokens ();

  list<vector<Element> > getAssymetry (PNClass *) ;
  
  // list interface
  list<vector<Element> >::iterator begin() { return (elts.begin());}
  list<vector<Element> >::iterator end() { return (elts.end());}

  const list< vector<Element> >& Elts() const {return elts;}
  list<vector<Element> >& Elts() {return elts;}
  void Elts(list<vector<Element> >elts) {this->elts = elts;}
  
  Domain* const getDomain() const {return dom;}
  void setDomain(Domain* const domain) {dom = domain;}
};





#endif
