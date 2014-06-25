#ifndef _MARKING_H_
#define _MARKING_H_


#include <list>
#include "Mark.h"
using namespace std;


class Marking {
protected : 
  list<Mark> lst;
public :
  Marking () {} ;
  Marking (list<Mark> l ):lst(l) {};

  const list<Mark>& Lst() const {return lst;}
  list<Mark>& Lst() {return lst; }

  virtual ~Marking () {};

  list<vector<Element> > getAssymetry (PNClass *) ;
  void RewriteWithStaticClasses ();
  void RewriteWithDynamicClasses ();
  
  void Insert (const Mark &m) { lst.push_front(m); }
  void InsertBack (const Mark &m) { lst.push_back(m); }
  friend  ostream & operator<< (ostream & os , const Marking &m) ;
  string ExportToCami () ;
  string ExportToGSPN () ;

//  friend Marking calcSub::groupMarking (Marking & m) ;
};





#endif
