#include "PNClasses.h"
#include <algorithm>
#include "PNetIO.h"
#include <sstream>
#include "CalcSubClasses.h"
//Implementation for class PNClasses
//default constructor
PNClasses::PNClasses () {

}
//default destructor
PNClasses::~PNClasses (){}

// *******************basic operators

// comparison by operator==
bool operator==(const PNClasses& a,const PNClasses& b){
  list<PNClass>::const_iterator ita,itb;
  if ( a.lst.size() != b.lst.size() )
    return 0;
  else 
    for (ita = a.lst.begin(),itb=b.lst.begin();ita!=a.lst.end() && itb!=b.lst.end();ita++,itb++) 
      if (!(*ita == *itb)) return 0;
  return 1;

}

// comparison by operator<
bool operator<(const PNClasses& a,const PNClasses& b){
  list<PNClass>::const_iterator ita,itb;
  if ( a.lst.size() < b.lst.size() )
    return 1;
  else if ( a.lst.size() > b.lst.size() )
    return 0;
  else 
    for (ita = a.lst.begin(),itb=b.lst.begin();ita!=a.lst.end() && itb!=b.lst.end();ita++,itb++) 
      if (*ita < *itb) return 1;
  return 0;
}

// printing with operator<<
ostream& operator<<(ostream& os,const PNClasses& p){
  list<PNClass>::const_iterator it;
  os<<"Classes: {"<<endl;
  for (it = p.lst.begin();it != p.lst.end();it++) {
    os<<*it<<","<<endl; 
  }
  os<<"} End Classes"<<endl;
  return os; 
}

// **********Advanced ops
/* sorted insert */
PNClass * PNClasses::Insert (PNClass &p) {
  list<PNClass>::iterator it;
  it = upper_bound(lst.begin(),lst.end(),p);
  lst.insert(it,p);
  it--;
  return &(*it);
}

PNClass* PNClasses::Find(int id2find) {
  list<PNClass>::iterator it;
  it=find(lst.begin(),lst.end(),PNClass(id2find));
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}

PNClass* PNClasses::FindName (const string &name) {
  list<PNClass>::iterator it;
  class FindName fn(name);
  it=find_if(lst.begin(),lst.end(),fn);
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}

void PNClasses::MakeStatic (list<vector<Element> > l, PNClass * pC) {
  list<vector<Element> >::iterator it;
  list<PNClass *> subclasses;
  list<PNClass *>::iterator jt;

  /* check if existing static subs correspond to elements of l */
  if (! pC->subclasses.empty()) {
    for (it = l.begin(); it != l.end() ; it++) {
      // cerr << "l" ;calcSub::print(cerr,l);
      for (jt = pC->subclasses.begin() ; jt != pC->subclasses.end() ; jt++) {
	vector<Element> inter;
	// cerr << "it" ;calcSub::print(cerr,*it);
	// cerr << "jt" ;calcSub::print (cerr,(*jt)->elts);
	if ( (*jt)->elts == *it ) 
	  { /* already a static sub corresponding to this set */
	    l.erase(it); 
	    it--; 
	    break; 
	  }
	back_insert_iterator<vector<Element> > ii(inter);
	set_intersection((*jt)->elts.begin(),(*jt)->elts.end(),it->begin(),it->end(),ii);
	// cerr << "inter" ;calcSub::print (cerr,inter);
	if ( ! inter.empty() ) {
	  /* incompatibility between l and current subclass, destroy current subclass */
	  string name = (*jt)->Name();
	  pC->subclasses.erase(jt);  
	  list<PNClass>::iterator it2;
	  class FindName fn(name);
	  it2=find_if(lst.begin(),lst.end(),fn);
	  lst.erase(it2);
	  break;
	}
      }
    }

  }

  if (pC->isOrdered && l.size() > 1) {
    vector<Element>::iterator vit ;
    vector<Element> v = pC->elts ;
    int i=0;
    for (vit = v.begin() ; vit != v.end() ; vit++,i++) {
      ostringstream ost;
      ost << pC->Name() << "_" << pC->SubClasses().size() << "ss";
      string name = ost.str();
      PNClass c = PNClass(lst.size()+1,name,pC,false);
      PNClass *pSub  = Insert(c);
      
      pSub->AddElt(*vit);
      
      pC->addSub (pSub);
    }
  } else {
    for (it=l.begin() ; it!=l.end() ; it++ ) {      
      ostringstream ost;
      ost << pC->Name() << "_" << pC->SubClasses().size() << "ss";
      string name = ost.str();
      PNClass c = PNClass(lst.size()+1,name,pC,false);
      PNClass *pSub  = Insert(c);
      
      vector<Element>::iterator vit ;
      vector<Element> &v = *it;
      
      for (vit = v.begin() ; vit != v.end() ; vit++) {
	pSub->AddElt(*vit);
      }
      pC->addSub (pSub);
    }
  }
}
  
void PNClasses::MakeDynamic (list<vector<Element> > l, PNClass * pC) {
  list<vector<Element> >::iterator it;
  list<PNClass *> subclasses;
  //  list<PNClass *>::iterator jt;

//  cerr << "l" ; calcSub::print(cerr,l);

  
//   /* check if existing static subs correspond to elements of l */
//   if (! pC->subclasses.empty()) {
//     for (it = l.begin(); it != l.end() ; it++) {
//       // cerr << "l" ;calcSub::print(cerr,l);
//       for (jt = pC->subclasses.begin() ; jt != pC->subclasses.end() ; jt++) {
// 	vector<Element> inter;
// 	// cerr << "it" ;calcSub::print(cerr,*it);
// 	// cerr << "jt" ;calcSub::print (cerr,(*jt)->elts);
// 	if ( (*jt)->elts == *it ) 
// 	  { /* already a static sub corresponding to this set */
// 	    l.erase(it); 
// 	    it--; 
// 	    break; 
// 	  }
// 	back_insert_iterator<vector<Element> > ii(inter);
// 	set_intersection((*jt)->elts.begin(),(*jt)->elts.end(),it->begin(),it->end(),ii);
// 	// cerr << "inter" ;calcSub::print (cerr,inter);
// 	if ( ! inter.empty() ) {
// 	  /* incompatibility between l and current subclass, destroy current subclass */
// 	  string name = (*jt)->Name();
// 	  pC->subclasses.erase(jt);  
// 	  list<PNClass>::iterator it2;
// 	  class FindName fn(name);
// 	  it2=find_if(lst.begin(),lst.end(),fn);
// 	  lst.erase(it2);
// 	  break;
// 	}
//       }
//     }
//   }

//   if (pC->isOrdered && l.size() > 1) {
//     // add elements to Zi
//     vector<Element>::iterator vit ;
//     vector<Element> &v = pC->elts ;

//       for (vit = v.begin() ; vit != v.end() ; vit++) {
// 	// find the appropriate static subclass
// 	for (list<PNClass *>::iterator jt = pC->subclasses.begin() ; jt != pC->subclasses.end() ; jt++) {
// 	PNClass * ssc = *jt;
// 	if (ssc->elts.front() == *vit && ssc->elts.size() == 1) {
// 	  // OK : *it is part of ssc

// 	  // give the new Zi a name
// 	  ostringstream ost;
// 	  ost << ssc->name << "_" << ssc->subclasses.size() ;
// 	  string name = ost.str();

// 	  // let it be
// 	  PNClass c = PNClass(lst.size()+1,name,&(*ssc),false);
// 	  // insert in PNClasses
// 	  PNClass *pSub  = Insert(c);
	  
// 	  // add element to Zi
// 	  pSub->AddElt(*vit);

// 	  ssc->addSub (pSub);
// 	  // found the right ssc, break to next it
// 	  break;
// 	}
// 	}
//       }
//    } else {
    // for each set of elements we must assign to a dynamic subclass
    for (it=l.begin() ; it!=l.end() ; it++ ) {  
      // find the appropriate static subclass
      for (list<PNClass *>::iterator jt = pC->subclasses.begin() ; jt != pC->subclasses.end() ; jt++) {
	PNClass * ssc = *jt;
	if ( includes(ssc->elts.begin(),ssc->elts.end(),it->begin(),it->end()) ) {
	  // OK : *it is part of ssc

	  // give the new Zi a name
	  ostringstream ost;
	  ost << ssc->name << "_" << ssc->subclasses.size() ;
	  string name = ost.str();

	  // let it be
	  PNClass c = PNClass(lst.size()+1,name,&(*ssc),false);
	  // insert in PNClasses
	  PNClass *pSub  = Insert(c);
	  
	  // add elements to Zi
	  vector<Element>::iterator vit ;
	  vector<Element> &v = *it;
	  
	  for (vit = v.begin() ; vit != v.end() ; vit++) {
	    pSub->AddElt(*vit);
	  }
	  ssc->addSub (pSub);
	  // found the right ssc, break to next it
	  break;
	}
      }
    }
//  }
}

  
int PNClasses::ExportToCami(ostream &os,int &numligne) {
  if (! this->lst.empty())
    {
      os << "CM(11:declaration,1,"<<numligne++<<",1,5:Class)\n";
      list<PNClass>::iterator it;
      for (it = lst.begin();it!=lst.end();it++) {
	it->ExportToCami(os,numligne);
      }
    }
  return 0;
}

void PNClasses::ExportToGSPN (ostream &os) {
  list<PNClass>::iterator it;

  for (it = lst.begin();it!=lst.end();it++) {
    it->ExportToGSPN(os);
  }

}

void PNClasses::MakeStatic () {
  list<PNClass>::iterator it= lst.begin();

  for (it=lst.begin() ; it!=lst.end() ; it++ ) {
    /* find primary non static classes */
    if (it->type != Statique) {
   
      vector<Element> v = it->Elts();
      vector<Element> vv = vector<Element> ();
      list<PNClass>::iterator jt ;


      for (jt = lst.begin() ; jt != lst.end() ; jt++) {
	/* find sub classes of considered primary class */
	if (jt->type == Statique && *jt->parent == *it ) {

	  vector<Element> w = jt->Elts() ;

	  /* any element not in subclass is kept  in our v list */
	  for (unsigned int i=0 ; i<v.size() ; i++) {
	    if ( find(w.begin(),w.end(),v[i]) == w.end() ) vv.insert(vv.end(),v[i]); 
	  }
	  v = vv ;
	  vv = vector<Element> ();
	} 
      } // forall subclasses of our primary

      /* v now contains any elments of our primary not element of a subclass */
      if (v.size() != 0) {
	ostringstream name ;
	name << it->Name() << "_" << size();
	string nname = name.str() ;
	PNClass *pc = new PNClass (lst.size(), nname,&(*it));
	for (unsigned int i=0; i < v.size() ; i++) pc->AddEltToStatic (v[i].desc) ;
	Insert(*pc);
      }
      

    } // if not statique
    
  } // for all classes
}
  


void PNClasses::Reindex () {
  list<PNClass>::iterator it;
  int i=0;
  for (it=lst.begin() ; it!=lst.end() ; it++,i++ ) {
    it->id = i;

  }
  
}
