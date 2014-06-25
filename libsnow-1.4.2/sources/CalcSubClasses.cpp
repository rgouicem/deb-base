#include "CalcSubClasses.h"
#include <algorithm>

#include <utility>
using namespace std::rel_ops;

namespace calcSub {

  void print (ostream & out, vector<Element> elts, bool crochets) 
  {
    
    int min,max,cur;
    bool hole = false;
    vector<Element>::iterator it;
    
    
    if (crochets) out << "[";
    if (!elts.empty()) {
      it=elts.begin();
      min=it->Id();
      cur=min;
      max=-1;
      for (it++;it!=elts.end();it++) {	
	if (it->Id() == ++cur) max = cur;
	else { // We have a hole!!
	  hole = true;
	  break;
	} 
      }
      
      if (!hole) {
	out << elts.front();
	out << "..";
	out << elts.back();
      } else
	for (it = elts.begin() ; it != elts.end() ; it++ ) {
	  out << *it  ;
	  if (it != --elts.end()) out << ", ";
	}
    } // !elts.empty()
    if (crochets) out << "]" << endl;
  }
  

  void print (ostream & os, list<vector<Element> > l) 
  {
    list<vector<Element> >::iterator it ;
    
    os << "{"<<endl;
    for (it=l.begin();it!= l.end();it++) {
      print (os,*it);
    }
    os << "}"<< endl;
  

  }
  
  list<vector<Element> > intersect (vector<Element> v1,vector<Element> v2 ) {
    list<vector<Element> > l;
    l.push_back(v1);
    l.push_back(v2);
    return intersect(l);

  }


  /* takes a list of *****two***** sets of elements { S1 ,S2 } and returns a list of
     { S1 \ S2 , S2 \ S1 , S1 ^ S2 } *********************************************/
  list<vector<Element> > intersect (list<vector<Element> > & l) {
    

/*    cerr << "Intersect called with before sort : ";
      print(cerr,l);
*/

    /* sort our sets before anything else */
    sort (l.begin()->begin(),l.begin()->end());
    sort ((++l.begin())->begin(),(++l.begin())->end());

//    cerr << "Intersect called with after sort : ";
//    print(cerr,l);


    /*introduce working variables */
    vector<Element> S1 = *l.begin() ;
    vector<Element> S2 = *(++l.begin()) ;
    vector<Element> v1 ; /*  S1 \ S2 */
    vector<Element> v2 ; /* S2 \ S1 */
    vector<Element> v3 ; /*  S1 ^ S2 */
    list<vector<Element> > ret = list<vector<Element> > ();
    
    vector<Element>::iterator  it,jt; /* will traverse S1 and S2 respectively */

    /* Buffer end positions */
    vector<Element>::iterator S1end = S1.end(), S2end = S2.end() ;
    
    
    /** ok now for a laugh :) */
    for (it=S1.begin() , jt = S2.begin() ; it!=S1end || jt!=S2end ;  ) {
//       cerr << "it,jt :" ;
//       if (it == S1end) cerr << "S1end" ; else cerr << *it;
//       cerr <<"," ;
//       if (jt == S2end) cerr << "S2end" ; else cerr << *jt;
    
      if (it != S1end && jt != S2end && *it == *jt) {
// 	cerr << "   -> v3 ";
	v3.push_back(*it);
	it++;
	jt++;
      } else if (it != S1end && (jt == S2end || *jt > *it  )  ) {
// 	cerr << "   -> v1 ";
	v1.push_back (*it);
	it++;
      } else {
// 	cerr << "   -> v2 ";
	v2.push_back(*jt);
	jt++;
      }
//      cerr << endl;
    }
    
    /* all done */
    ret.push_back(v1);
    ret.push_back(v2);
    ret.push_back(v3);
    
    return ret;
  }


  /* for sorting in descending order of set size */
  class size_elts {
  public :
    size_elts () {};
    bool operator()(const vector<Element>& a,const  vector<Element>& b) { int as,bs ; return ((as = a.size())< (bs = b.size()))?true:as==bs && as!=0?a.front()<b.front():false; }
  };

  // for type-match on if?true:false; expression :>
  inline void nop () {}

  /* takes a list l = {S1,S2, .. Sn} and returns a minimal partition into 
     non intersecting subsets that respects the original sets   
     i.e : l = { [1,2,3] , [3,4,5] } => ret = { [1,2] , [4,5] , [3] } 
  */
  list<vector<Element> > uniquePartition (list<vector<Element> > l) {
    list<vector<Element> > *lnext;
    list<vector<Element> > lret,lcall1;
    list<vector<Element> > *lcur,ltmp;
    vector<vector<Element> > vcall = vector<vector<Element> > ();
    list<vector<Element> >::iterator it,jt;

    // sort according to set size
    {
    list<vector<Element> >::iterator it;
    for (it = l.begin();it != l.end() ; it++ )
      vcall.push_back( *it );
    sort(vcall.begin(),vcall.end(),size_elts());
    vector<vector<Element> >::iterator vt;
    for (vt = vcall.begin();vt != vcall.end() ; vt++ )
      lcall1.push_back( *vt );
    

    lcur = &lcall1;
    }
    lcur->unique();
    vector<Element> v1=vector<Element> (),v2=vector<Element> (),v3=vector<Element> ();

    
//    cerr << "calling uniquePqrtition with l =" ; print(cerr,*lcur) ;
//    cerr << endl;

    for (   ;
	 !lcur->empty() && !lcur->begin()->empty() ;
	 (lcur != &lcall1?delete lcur:nop()),lcur = lnext ) {
      // added lcur != &lcall1? because of stupid double free problem in gcc 3.0.4 , it insists on freeing lcall1 at the end of procedure 
      v1 = *lcur->begin();

      lnext = new list<vector<Element> > ();
      
      if (lcur->size() == 1) {
	it = upper_bound(lret.begin(),lret.end(),*lcur->begin(),size_elts());
	lret.insert(it,*lcur->begin());
	break;
      } else { 
	for (jt = ++(lcur->begin()); jt != lcur->end() ; jt++) {
	  // for calling intersect function defined above
	  list<vector<Element> > lcall=list<vector<Element> > ();
	  // for storing the result of the call
	  list<vector<Element> > lcallres;
	  
	  lcall.push_back(v1);
	  lcall.push_back(*jt);
	  
	  /*
	  cerr << endl << "intersect with : ";
	  print(cerr,lcall);
	  */
	  lcallres = intersect(lcall);
	  
	  v1 = *lcallres.begin(); // S1 / S2
	  v2 = *(++(lcallres.begin())); // S2 / S1
	  v3 = *(++(++(lcallres.begin())));// S1 inter S2

	  /*
	  cerr << endl << "after intersect :";
	  print(cerr,lcallres);
	  cerr<<endl;
	  */
	  
    
	  
	  if (v1.size() == 0 ) {
	    /* S1 was a part of S2 */
	    if (v2.size() != 0) {
		// add S2 /S1 to next
	     	it = upper_bound(lnext->begin(),lnext->end(),v2,size_elts());
		lnext->insert(it,v2);
	    }
	    // add S1 inter S2 to next
	    it = upper_bound(lnext->begin(),lnext->end(),v3,size_elts());
	    lnext->insert(it,v3);
	    
	    // Since the current v1 is now empty , add all the sets not yet explored to next
	    list<vector<Element> >::iterator kt;
	    // damn it cant write jt+1 : operation not defined iterator + int on lists
	    for (kt = ++jt--; kt != lcur->end() ; kt++) {
	      it = upper_bound(lnext->begin(),lnext->end(),*kt,size_elts());
	      lnext->insert(it,*kt);
	    }
	    break;
	  } else if (v2.size() == 0) {
	    /* shouldn t happen ?? with ordering of sets except if v1 is size 0 too*/
	    
	  } else if (v3.size() == 0) {
	    /* S1 inter S2 is empty, iterate with v1 and add S2 to lnext */
	    it = upper_bound(lnext->begin(),lnext->end(),v2,size_elts());
	    lnext->insert(it,v2);
	    if (jt == --(lcur->end())) {
	      it = upper_bound(lret.begin(),lret.end(),v1,size_elts());
	      lret.insert(it,v1);
	    }
	  } else {
	    /* S1 and S2 have common elements */
	    it = upper_bound(lnext->begin(),lnext->end(),v2,size_elts());
	    lnext->insert(it,v2);
	    // add S1 inter S2 to next
	    it = upper_bound(lnext->begin(),lnext->end(),v3,size_elts());
	    lnext->insert(it,v3);

	    if (jt == --(lcur->end())) {
	      it = upper_bound(lret.begin(),lret.end(),v1,size_elts());
	      lret.insert(it,v1);
	    }
	    
	  }
	  
	}
      }
      
      /*
      cerr << "lcur is" ;
      print (cerr,*lcur);
      cerr << "lnext is" ;
      print (cerr,*lnext);
      cerr << "lret is" ;
      print (cerr,lret);
      */
      
      
    }

    for (it = lret.begin() ; it!= lret.end() ; it++ ) 
      sort (it->begin(),it->end());
    
    
//    cerr << "\nFINALLY lret is" ;
//    print (cerr,lret);
    
    return lret ;
  }


  vector<vector<Element> > SeparateInto (vector<Element> v,list<vector<Element> > & l) {
    vector<vector<Element> > ret;
    list<vector<Element> >::iterator it;
    vector<Element> res;

    for (it =l.begin();it!= l.end() && !v.empty();it++) {
      res = v - *it;
      if (res == v) continue;
      else {
	ret.push_back(*it);
	v = res;
      }
    }
    return ret;
  }




  vector<Element> operator+ ( vector<Element> &v1,vector<Element> &v2) {
    vector<Element> ret = vector<Element> (v1.size() + v2.size() );
    unsigned int i,j,k;
    
    for (i=0,j=0,k=0 ; i<v1.size() || j<v2.size() ;k++ ) {
      if (i == v1.size()) ret[k] =  v2[j++];
      else if (j == v2.size())  ret[k] =  v1[i++];
      else ret[k] = v1[i] < v2[j] ? v1[i++] : v2[j++] ;
    }
    return ret;
  }
  
  vector<Element> operator- ( vector<Element> &v1,vector<Element> &v2) {
    vector<Element> ret ;
    unsigned int i,j,k;
    
    for (i=0,j=0,k=0 ; i<v1.size() || j<v2.size() ; ) {
      if (i == v1.size()) break;
      else if (j == v2.size())  {
	ret.insert(ret.end(),v1.begin()+i,v1.end());
	break;
      } else {
	if (v1[i] < v2[j]) { ret.push_back(v1[i++]); }
	else if (v1[i] == v2[j]) { i++;j++;}
	else { j++; }
      }
    }
    return ret;
  }


  // calculates next and returns 0 if ok, 1 if overflow has occured
  // used to explore combinations in Mark.cpp and CFuncTerm.cpp
  int next (list<pair <int,int> > &l) {
    list<pair <int,int> >::reverse_iterator rit;
    
    int retenue = 1;
    for (rit = l.rbegin();rit != l.rend() && retenue;rit++) {
      if (rit->first + 1 == rit->second) {
	rit->first = 0;
      } else {
	rit->first ++;
	retenue =0;
      }
    }
    return retenue;
  }



  
 
} // namespace



// OPTIMIZED BUGGY VERSION OF UNIQUEPARTITION
// if (v1.size() == 0 ) {
// 	    /* S1 was a part of S2 */
// 	    if (v2.size() != 0) {
// 		// add S2 /S1 to next
// 	     	it = upper_bound(lnext->begin(),lnext->end(),v2,size_elts());
// 		lnext->insert(it,v2);
// 	    } 
// 	    // add S1 inter S2 to next
// 	  //  it = upper_bound(lnext->begin(),lnext->end(),v3,size_elts());
// 	  //  lnext->insert(it,v3);
	    
// 	    // Since the current v1 is now empty , iterate with v1 = v3
// //	    v1 = v3 ;
// //	    if (jt == --(lcur->end())) {
// //	      it = upper_bound(lret.begin(),lret.end(),v1,size_elts());
// //	      lret.insert(it,v1);
// //	    }

// 	    list<vector<Element> >::iterator kt;
// 	    // damn it cant write jt+1 : operation not defined iterator + int on lists
// 	    for (kt = ++jt--; kt != lcur->end() ; kt++) {
// 	      it = upper_bound(lnext->begin(),lnext->end(),*kt,size_elts());
// 	      lnext->insert(it,*kt);
// 	    }
// 	    break;
// 	  }
