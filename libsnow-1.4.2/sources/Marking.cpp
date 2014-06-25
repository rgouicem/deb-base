#include "Marking.h"
#include <sstream>
#include "CalcSubClasses.h"


list<vector<Element> > Marking::getAssymetry (PNClass *p) {
  
//    cerr << "before :" ;
//    for (list<Mark>::iterator it = lst.begin() ; it != lst.end() ; it++ ) {
//      cerr << *it << "  ";
//    }
//    cerr << endl;


  // split into elementary marks to ensure correct grouping
  list<Mark> lstSplit;
  for (list<Mark>::iterator it = lst.begin() ; it != lst.end() ; it++ ) {
    list<Mark> split = it->RewriteWithSimpleTokens();
    lstSplit.splice(lstSplit.begin(),split );
  }
  // eliminate identical marks with multiplicity rule : a*<M> + b*<M> = (a+b)*<M>
  for (list<Mark>::iterator it = lstSplit.begin() ; it != lstSplit.end() ; it++ ) {
    list<Mark>::iterator jt = ++it;
    it--;
    for ( /*NOP*/  ; jt != lstSplit.end() ; /*NOP*/ ) {
      if (it->Elts() == jt->Elts()) {
	it->Mult() += jt->Mult();
	jt = lstSplit.erase(jt);
      } else {
	jt++;
      }
    }
  }
    


//    cerr << "after :" ;
//    for (list<Mark>::iterator it = lstSplit.begin() ; it != lstSplit.end() ; it++ ) {
//      cerr << *it << "  ";
//    }
//    cerr << endl;
   
//    list<Mark> lst2 = calcSub::groupTemplate(lstSplit,(vector<Element> *) NULL);
//    cerr << "after group :" ;
//    for (list<Mark>::iterator it = lst2.begin() ; it != lst2.end() ; it++ ) {
//      cerr << *it << "  ";
//    }
//    cerr << endl;

  Marking m(calcSub::groupTemplate(lstSplit,(vector<Element> *) NULL));
  list<vector<Element> > lcall,lcallres ;
  list<Mark>::iterator it;

  for (it = m.lst.begin();it!=m.lst.end() ; it++ ) {
    lcallres = it->getAssymetry(p);
    lcall.insert(lcall.end(),lcallres.begin(),lcallres.end());
  }
  lcall = calcSub::uniquePartition (lcall);

  *this = m;

  return lcall;
}

void Marking::RewriteWithStaticClasses () {
  
  list<Mark> newlist;
  list<Mark>::iterator it;

  for (it = lst.begin();it!=lst.end();it++) {
    list<Mark> res = it->RewriteWithStaticClasses();
    newlist.insert(newlist.end(),res.begin(),res.end());
  }

  lst = newlist;

}

void Marking::RewriteWithDynamicClasses () {
  
  list<Mark> newlist;
  list<Mark>::iterator it;

  for (it = lst.begin();it!=lst.end();it++) {
    list<Mark> res = it->RewriteWithDynamicClasses ();
    newlist.insert(newlist.end(),res.begin(),res.end());
  }

  lst = newlist;

}

string Marking::ExportToGSPN () {
  list<Mark>::iterator it;
  ostringstream ost ;
  int i;
  int max =  lst.size() -1;


  for (it = lst.begin(),i=0;it!=lst.end() ;i++, it++ ) {
    it->ExportToGSPN (ost);
    if (i < max) ost << " + ";
  }

  return ost.str();
}

string Marking::ExportToCami () {
  list<Mark>::iterator it;
  ostringstream ost ;
  int i;
  int max =  lst.size() -1;


  for (it = lst.begin(),i=0;it!=lst.end() ;i++, it++ ) {
    it->ExportToCami (ost);
    if (i < max) ost << ", ";
  }

  return ost.str();

}


ostream & operator<< (ostream & os , const Marking &m) {
  list<Mark>::const_iterator it;
  int i;
  int max =  m.lst.size() -1;

  for (it = m.lst.begin(),i=0;it!=m.lst.end() ;i++, it++ ) {
    os << *it ;
    if (i < max) os << ", ";
  }
  
  return os;
}
