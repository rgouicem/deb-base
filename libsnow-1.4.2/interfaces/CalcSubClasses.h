#ifndef _CALCSUBCLASSES_H_
#define CALCSUBCLASSES_H_

#include <list>
#include <vector>
#include "Element.h"
#include "Marking.h"
#include "CFuncElt.h"


// This namespace introduces functions that work on vector<Element> to calculate the subclass partition

namespace calcSub {

  /* takes a list of *****two***** sets of elements { S1 ,S2 } and returns a list of
     { S1 \ S2 , S2 \ S1 , S1 ^ S2 } *********************************************/
  list<vector<Element> > intersect (list<vector<Element> > & );
  /* Wraps the call to previous primitive */
  list<vector<Element> > intersect (vector<Element> v1,vector<Element> v2 ) ;
  
  /* Does the Intersect between every pair of sets and returns a refined partition */
  list<vector<Element> > uniquePartition (list<vector<Element> > ) ;

  /* Gives a partition of v into elements of l, used in Rewrite rules when subclasses have been obtained*/
  vector<vector<Element> > SeparateInto (vector<Element> v,list<vector<Element> > & l) ;

  /* Used to group/fusion the terms of a Marking or ColorFunction
     the second term is anonymous and is just necessary for template initialization,
     For : Marking m(calcSub::groupTemplate(lst,(vector<Element> *) NULL));
     Or  :   CFunc m(calcSub::groupTemplate(lst,(CFuncElt *) NULL )); */
  template<class T,class T2>
  list<T> groupTemplate (list<T> & mm,T2*) ;

  // display and debug routines 
  void print (ostream & os, list<vector<Element> > l) ; 
  void print (ostream & os, vector<Element> v,bool = true) ;

  /* ensemblist Union and \ operations */
  vector<Element> operator+ ( vector<Element> &v1,vector<Element> &v2);
  vector<Element> operator- ( vector<Element> &v1,vector<Element> &v2);

  /* for template calls ... */
  inline bool isAddable ( CFuncElt &e1,CFuncElt &e2 ) { return (e1.IsVar() == e2.IsVar()); }
  inline bool isAddable ( vector<Element> &,vector<Element> &) {return true;}

  // calculates next and returns 0 if ok, 1 if overflow has occured
  // used to explore combinations in Mark.cpp and CFuncTerm.cpp
  int next (list<pair <int,int> > &l);

}




template<class T,class T2>
list<T> calcSub::groupTemplate (list<T> & mm,T2 *) {
  
  list<T> cur (mm);
  list<T> traite = list<T> ();
  T *curElt;
  typename list<T>::iterator jt;
  
  
  
  while (! cur.empty()) {
    curElt = &cur.front();
    for (jt = ++cur.begin(); jt != cur.end() ; ) {
       pair<typename list<T2>::iterator,typename list<T2>::iterator> m;
      m = mismatch( curElt->begin(),curElt->end(),jt->begin());
      if (m.first == curElt->end() ) {
	/*  a l1 + b l1 = (a+b) l1 */
	curElt->Mult() += jt->Mult();
	jt = cur.erase(jt);
      } else if (curElt->Mult() == jt->Mult()) {
	pair<typename list<T2>::iterator,typename list<T2>::iterator> m2;
	m2 = mismatch(++m.first,curElt->end(),++m.second);
	m.first--; m.second--;
	if (m2.first == curElt->end() && isAddable(*m.first,*m.second)  ) {
	  /* a* (<l1,m,l2> + <l1,m',l2>) = a * <l1,m+m',l2> */
	  * m.first = *m.first + * m.second;
	  cur.insert(cur.end(),traite.begin(),traite.end());
	  traite = list<T> ();
	  cur.erase(jt);
	    jt = ++cur.begin();
	} else {  
	    /* a * ( <l1,m1,l2,m2,..> + <l1,m1',l2,m2',..> ) -> NOP */
	  jt++;
	  continue;
	}
      } else {
	/* a * l1 + b* l2 and l1 !=l2  -> NOP */
	jt++;
	continue;
      }
    }
    traite.push_back(*curElt);
    cur.pop_front();
  }
  
  return traite;
}


#endif
