#include "Guard.h"
#include "ParserWrapper.h"
#include "CalcSubClasses.h"
#include <algorithm>
#include <sstream>
#include "Variable.h"
#include "Element.h"

//Implementation for class Guard
//default constructor
Guard::Guard (string s,Guard *fgg,Guard *fdd):var(NULL),succ(0) {
  if (s=="TRUE") {
    op = 'T';
    fg = fd = NULL;
  } else if (s=="FALSE") {
    op = 'F';
    fg = fd = NULL;
  } else if (s=="AND") {
    op = '.';
    if (fgg==NULL || fdd==NULL) 
      cerr << "AND operation with NULL son \n ";
    if (fgg->op == 'h' || fdd ->op == 'h') {
      op = 'h';
      fd = NULL;
      if ( fgg->op == 'h'  && fgg->fg == NULL)
	fg = new Guard(*fdd);
      else if ( fdd->op == 'h'  && fdd->fg == NULL)
	fg = new Guard(*fgg);
      else if ( fgg->op == 'h' && fdd ->op != 'h' ) 
	fg = new Guard("AND", new Guard(*fgg->fg) , new Guard(*fdd->fg));
      else if ( fgg->op == 'h' )
	fg = new Guard("AND", new Guard(*fgg->fg) , new Guard(*fdd));
      else
	fg = new Guard("AND", new Guard(*fgg) , new Guard(*fdd->fd));
    } else {
      fg = fgg ;
      fd = fdd ;
    }
  }  else if (s=="OR") {
    op = '+';
    if (fgg==NULL || fdd==NULL) 
      cerr << "OR operation with NULL son \n ";
    fg = fgg ;
    fd = fdd ;
  } else if (s=="NOT") {
    op = '-';
    if (fgg==NULL) 
      cerr << "NOT operation with NULL son \n ";
    fg = fgg ;
    fd = NULL ;
  } 
}

Guard::Guard (const string &head,const string &guardstr, const string &tname, bool isGSPN):var(NULL),succ(0) {
  if (head == "head") {
    op = 'h';
    var = NULL;
    if (isGSPN) {
      fg = ParserWrapper::ParseGuard(guardstr,tname,true);
    } else 
      fg = ParserWrapper::ParseGuard(guardstr);
    fd = NULL;
  }


}

Guard::Guard (Variable *v,int _succ): succ(_succ) {
  op = 'v';
  var = v;
  fg =fd = NULL;
}

Guard::Guard (Variable *v,Element * e,int _succ):succ(_succ) {
  if (!e) { 
    set = vector<Element> ();
    op = 'v';
  } else {
   set = vector<Element> (1);
   set[0] = * e;
   op = 'd';
  }
  var = v;
  fg =fd = NULL;
}

Guard::Guard (Variable *v,vector<Element> ve,int _succ):succ(_succ) {
  /*constructor by copy */
   set = ve ;
   op = 'd';
   var = v;
   fg =fd = NULL;
}

Guard::Guard (Variable *v,const string &s,Variable *v2,int succ1,int succ2):var(NULL),succ(0) {
  if (s== "=") op = '='; 
  else op='!';
  fg = new Guard (v,succ1);
  fd = new Guard (v2,succ2);
}

//constructor by copy
Guard::Guard (const Guard& g){
    var = g.var;
    op  = g.op;
    set = g.set;
    if (g.fg == NULL) fg=NULL;
    else fg = new Guard (*g.fg);
    if (g.fd == NULL) fd=NULL;
    else fd = new Guard (*g.fd);
    succ=g.succ;
}

//default destructor
Guard::~Guard (){}

// *******************basic operators

// clone by operator=
Guard& Guard::operator=(const Guard& g){
  if (this != &g) {
    var = g.var;
    op = g.op;
    set = g.set;
    if (g.fg == NULL) fg=NULL;
    else fg = new Guard (*g.fg);
    if (g.fd == NULL) fd=NULL;
    else fd = new Guard (*g.fd);
    succ = g.succ;
  }
  return *this;
}

// comparison by operator==
int Guard::operator==(const Guard& g){
  return (op == g.op && set == g.set && var == g.var && (fg == g.fg || *fg == *g.fg)  && (fd == g.fd || *fd == *g.fd) && (succ == g.succ) );
 
}




string  Guard::getCamiFormat() {
  ostringstream ost;
  ost << *this ;
  return ost.str();
}

void Guard::FindOrderedClasses () {
  if (var) if (succ)  var->PClass()->setOrdered(true);
  if (fg!= NULL) { fg->FindOrderedClasses(); }
  if (fd!= NULL) { fd->FindOrderedClasses(); }

}




// printing with operator<<
ostream& operator<<(ostream& os,const Guard& g){
  switch (g.op) 
    {
    case '.' : { os << "(" << *g.fg << ") AND (" << *g.fd << ")"; break ; }
    case '+' : { os << "(" << *g.fg << ") OR (" << *g.fd << ")"; break ; }
    case '-' : { os << "NOT (" << *g.fg << ")" ; break ; }
    case '=' : { os << "(" << *g.fg << "=" << *g.fd << ")"; break ; }
    case '!' : { os << "(" << *g.fg << "<>" << *g.fd << ")"; break ; }
    case 'T' : { os << "TRUE"; break ; }
    case 'F' : { os << "FALSE"; break ; }
    case 'd' : 
      {
	PNClass *ppc = g.var->PClass()->FindSubContaining(g.set);
	os << g.var->Name();
      if (g.succ > 0) os << "++" << g.succ;
      if (g.succ < 0) os << "--" << (-g.succ);
	
      if (ppc) {
	  os <<" IN " ; 
	  os << ppc->Name();
	} else {
	  os << " IN {" ; 
	  if (g.set.size() !=  0 ) {
	    calcSub::print(os,g.set,false);
	  }
	  os << "} ";
	}
	break;
      }
    case 'v':
      {
	os << g.var->Name() ;
      if (g.succ > 0) os << "++" << g.succ;
      if (g.succ < 0) os << "--" << (-g.succ);
	
	break;
      }
    case 'h':
      {
	os << "[" << *g.fg << "]";
      }
    }
  return os;
}

int Guard::ExportToGSPN (ostream &os ) {
  
    switch (op) 
    {
    case '.' : { os << "(" ; fg->ExportToGSPN (os);os << ") and ("; fd->ExportToGSPN (os) ; os << ")"; break ; }
    case '+' : { os << "(" ; fg->ExportToGSPN (os);os  << ") or (" ; fd->ExportToGSPN (os) ; os << ")"; break ; }
    case '-' : { os << "! (" ; fg->ExportToGSPN (os);os  << ")" ; break ; }
    case '=' : { os << "(" ; fg->ExportToGSPN (os);os  << "="; fd->ExportToGSPN (os) ; os <<  ")"; break ; }
    case '!' : { os << "(" ; fg->ExportToGSPN (os);os  << "<>"; fd->ExportToGSPN (os) ; os  << ")"; break ; }
    case 'T' : { os << "TRUE"; break ; }
    case 'F' : { os << "FALSE"; break ; }
    case 'd' : 
      {
	os << "d(";
	if (succ > 0) os  << succ << "!" ;
	if (succ < 0) os << (-succ) << "^" ;
	os  << var->ExportToGSPN() << ")=" ; 
	/* name of static sub */
	PNClass * ppc =var->PClass()->FindSubContaining(set);
	if (ppc) {
	  os << ppc->Name();
	} else {
	  os << "{" ; 
	  if (set.size() !=  0 ) {
	    vector<Element>::const_iterator it(set.begin());
	    os << it->desc;
	    for (it++;it!=set.end();it++) {
	      os << "," << it->desc;
	    }
	  }
	  os << "} ";
	}
	break;
      }
    case 'v':
      {	
	if (succ > 0) os  << succ << "!" ;
	if (succ < 0) os << (-succ) << "^" ;
	os << var->ExportToGSPN() ;
	break;
      }
    case 'h':
      {
	RewritePredWithClasses ();
	os << "0" ;
	if (fg->op != 'T' && fg->op != 'F') {
 	  os << "   " ; // 0 gratos pour GSPN
	  os << "1.0 1.0   " ; // position of the guard tag
	  os << "[" ; fg->ExportToGSPN (os) ; os  << "]"  ;
	} 
	os << endl;
	
	break;
      }
    }
    return 0;

}


/** Find guards on a given variable */

list<Guard *>& Guard::FindPredOnVar (Variable &v,list<Guard*> *l ) {
  if (op == '=' || op == '!') return *l;
  if (fg != NULL) fg->FindPredOnVar(v,l);
  if (fd != NULL) fd->FindPredOnVar(v,l);
  if (op == 'd' && *var==v) {
    l->push_back(this);
  }

  return *l;
}

void Guard::EnsureUnicity () {
  list<Variable *> lvar = FindMentionedVars ();
  list<Variable *>::iterator it ;

  for (it = lvar.begin();it != lvar.end() ;it++ ) {
    EnsureUnicity (*it);
  }
}


void Guard::RewritePredWithClasses () {
  list<Variable *> lvar = FindMentionedVars ();
  list<Variable *>::iterator it ;

//  Guard *g=this;

//   cerr << "This guard :";
//   cerr << *this ;
//  for (list<Variable *>::iterator vit=lvar.begin() ; vit != lvar.end();vit++) {

//    g = CanonizePredTree(this,*vit);

//  }
//  cerr << "g:" << g << endl;
//   cerr << *g <<endl ;
//  *this = *g;

//  cerr << "This guard :";
//  cerr << *this ;

  for (it = lvar.begin();it != lvar.end() ;it++ ) {
    list<Guard *> &l = FindPredOnVar (**it);
    list<Guard *>::iterator jt;
    list<vector<Element> > lsub = (*it)->PClass()->getSubSets() ;
    
//    cerr << "current subclass sets :";
//    calcSub::print(cerr,lsub);

    for (jt = l.begin();jt != l.end() ; jt++ )
      (*jt)->RewritePredWithPart (lsub);

  }
  
  Guard *g = SimplifierArbre(this);
  *this = *g;

}


void Guard::EnsureUnicity (Variable *v) {
  list<Guard *> &l = FindPredOnVar (*v);
  list<vector<Element> > lcall = list<vector<Element> > ();
  list<vector<Element> > lcallres;
  list<Guard *>::iterator it;
  
  /*
  cerr << "FindPredOnVar : " <<endl;
  for (it = l.begin();it != l.end() ; it++ )
    cerr << **it << ",";
  cerr <<endl;
  */

  for (it = l.begin();it != l.end() ; it++ )
    lcall.push_back( (*it) -> set);

  lcallres = calcSub::uniquePartition (lcall);
  
  //calcSub::print(cerr,lcallres) ;

  for (it = l.begin();it != l.end() ; it++ )
    (*it)->RewritePredWithPart (lcallres);
  
}


void Guard::RewritePredWithPart ( list<vector<Element> > & l ) {
  
  list<vector<Element> >::iterator vt;
  list<vector<Element> > lcall,lcallres;

//  cerr << "argument list:";
//  calcSub::print(cerr,l) ;
  
  for (vt = l.begin(); vt!= l.end() ; vt++) {
    lcallres = calcSub::intersect(set,*vt) ;

 //   cerr << "lcallres :";
  //  calcSub::print(cerr,lcallres) ;

    vector<Element> v1 = *lcallres.begin(); // S1 / S2
    vector<Element> v2 = *(++(lcallres.begin())); // S2 / S1
    vector<Element> v3 = *(++(++(lcallres.begin())));// S1 inter S2

//    cerr << "v1 =" ;
//    calcSub::print(cerr,v1);

    if (v1.size() == 0 && v2.size() ==0) {
      // finished
      return;
    } else if (v2.size() ==0) {
      //cerr << "yoyoy" ;
      fg = new Guard(var,v3) ;
      fd = new Guard(var,v1) ;
      fd->RewritePredWithPart (l);
      op = '+' ;
      var = NULL;
      set = vector<Element>();
      return;
    } else if (v3.size() == 0) {
      // no intersection continue with next vt
      continue;
    } else if (v1.size() == 0) {
      // should not happen
      cerr << "argument list:";
      calcSub::print(cerr,l) ;
      cerr << "set:";
      calcSub::print(cerr,set) ;
      
      
      
      cerr << "PROBLEM in Guard::RewritePredWithPart "<<endl ;
   //   exit(0);
    }
  }
}


Guard::PredPath::PredPath (Guard * pred,int depth) {
  this->pred = pred ;
  this->depth = depth;
}

Guard::PredPath * Guard::FindMisbehavedPred (Guard * guard,Variable *targetVar,int curdepth) {

  PredPath *ret;
  char retFrom=0;

  if (guard->op == 'h') 
    return FindMisbehavedPred(guard->fg,targetVar,0);

  if (guard==NULL) 
    return NULL;
  if ((guard->op == '+' && curdepth >0) || (guard->op == '.')) 
    curdepth ++;
    
  if (guard->op == 'd' && guard->var == targetVar && curdepth > 1) {
    ret = new PredPath (guard,curdepth);
      return ret;
  }  else if (guard->op == '+' || guard->op == '.' ) {
    if ((ret = FindMisbehavedPred(guard->fg,targetVar,curdepth)) != NULL) {
      retFrom = 'g';
    } else if  ((ret = FindMisbehavedPred(guard->fd,targetVar,curdepth)) != NULL) {
      retFrom = 'd';
    } else {
      return NULL;
    }
  } else {
    // any other variable or predicate i.e. a=b a!=b etc..
    return NULL;
  }
  
  if (retFrom == 'g')
    ret->addToPath (guard, guard->fd);
  else 
    ret->addToPath (guard, guard->fg);
  
  return ret;
  
}

void Guard::PredPath::addToPath (Guard * p,Guard * son) {
  path.push_back(p);
  otherSon.push_back(son);
}


Guard * Guard::ArbreQuotientSimplifie (Guard * guard,Guard * diviseur) {
  
  return SimplifierArbre(ArbreQuotient(guard,diviseur));
}

Guard * Guard::FusionnePred (Guard *a,Guard *b) {
  Guard *ret = new Guard (*a);
  list<vector<Element> > l;
  l = calcSub::intersect (a->set,b->set);
  // Add:  b.set \ a.Set to a.set
  ret->set.insert(ret->set.end(),(++(l.begin()))->begin(),(++(l.begin()))->end());

  sort(ret->set.begin(),ret->set.end());
  return ret;
}

void Guard::getOrTerms(Guard * guard,vector<Guard *> &lor) {
  if (guard->op == '+') {
    getOrTerms(guard->fg,lor);
    getOrTerms(guard->fd,lor);
//     if (find(lor.begin(),lor.end(),guard->fg) == lor.end() ) getOrTerms(guard->fg,lor);
//     else { guard->fg = new Guard("FALSE"); } 
//     if (find(lor.begin(),lor.end(),guard->fd) == lor.end() ) getOrTerms(guard->fd,lor);
//     else { guard->fd = new Guard("FALSE"); }  
  } else
    if (find(lor.begin(),lor.end(),guard) == lor.end()) lor.push_back(guard);
}

Guard * Guard::SimplifierArbre (Guard * guard) {

  if (guard==NULL) 
    return NULL;
  if (guard->op == 'h') guard->fg = SimplifierArbre (guard->fg);
  if (guard->op == '=' || guard->op == '!') return guard;
  if (guard->op == '+' || guard->op == '.') {
    if (guard->op == '.') {
      guard->fg = ArbreQuotientSimplifie (guard->fg,guard->fd);
      guard->fd = ArbreQuotientSimplifie (guard->fd,guard->fg);
    } else {

      guard->fg = SimplifierArbre (guard->fg);
      guard->fd = SimplifierArbre (guard->fd);

//      cerr << "b4 lor: " << *guard << endl; 
      vector<Guard *> lor;
      getOrTerms(guard->fg,lor);
      getOrTerms(guard->fd,lor);
      int i,j,max;
      max = lor.size();
//      for (i=0;i<max;i++) 
//	cout << "i:val" << i << ":" << *lor[i] <<endl;
      
      for (i=0;i<max-1;i++) {
	//      cout << "i:val" << i << ":" << *lor[i] <<endl;
	for (j=i+1;j<max;j++) {
	  if ( *lor[i] == *lor[j]) {
	    lor[j]->op = 'F';
	    lor[j]->fg = NULL;
	    lor[j]->fd = NULL;
	  }

//        // This is supposed to test if we have (x in E) OR (x in C / E) = TRUE
// 	  else if ( lor[i]->op == 'd' && lor[j]->op == 'd' 
// 		      && lor[i]->var ==  lor[j]->var 
// 		      && lor[i]->set.size() + lor[j]->set.size() == lor[i]->var->PClass()->size()
// 		    ) {
// 	    guard->op = 'T';
// 	    guard->fg = guard->fd = NULL;
// 	  }
	}
      }
      if (lor.size() == 1) {
	*guard = *lor[0];
      } else if (lor.size() == 2) {
	guard->fg = lor[0];
	guard->fd = lor[1];
      } else {
	int ii = lor.size();
	Guard * newG = new Guard("OR",lor[ii-2],lor[ii-1]);
	ii -= 2;
	for ( ; ii> 1 ; ii--) {
	  Guard * newGG = new Guard("OR",lor[ii-1],newG);
	  newG = newGG;
	}
	guard->fd = newG;
	guard->fg = lor[0];
      }
	
//      cout << *guard;

    } 



    }
    /*  1+x = 1 ; x+0 = x ; 
        0.x = 0 ; x.1 =x */ 
    if ( (guard->op == '+'  && (guard->fg->op == 'T' ||  guard->fd->op == 'F')) 
	 || (guard->op == '.'  && (guard->fg->op == 'F' ||  guard->fd->op == 'T')))  {

      return guard->fg ;
      /*  0+x = x ; x+1 = 1 ;
	 1.x = x ; x.0 = 0 */
    } else if  ( (guard->op == '+'  && (guard->fg->op == 'F' ||  guard->fd->op == 'T'))
		 || (guard->op == '.'  && (guard->fg->op == 'T' ||  guard->fd->op == 'F')))   { 

      return guard->fd ;
      /* x+x = x ;
	 x.x = x */
    } else if ( ( guard->op == '+'  ||  guard->op == '.' ) 
		&& (guard->fg == guard->fd   || *guard->fg == *guard->fd  ) ) {
      return guard->fd ;
    }
      /*DONT APPLY THIS : BREAKS ALL WE VE BEEN DOING !! We can't control the direction its applied
	Silly of me to try that one ..:>*/    
  /*  Distributivity of type :  (x.y)+(x.z) = x.(y+z)  */
/*     } else if ( guard->op == '+' && guard->fd->op == '.' && guard->fg->op == '.' ) { */
/*       if (guard->fd->fg == guard->fg->fg ) { */
/* 	 (x.y)+(x.z) = x.(y+z)  */
/* 	return newOpPred('.',guard->fd->fg,newOpPred('+',guard->fg->fd,guard->fd->fd)); */
/*       } else if (guard->fg->fg == guard->fd->fd ) { */
/* 	 (x.y)+(z.x) = x.(y+z)  */
/* 	return newOpPred('.',guard->fg->fg,newOpPred('+',guard->fg->fd,guard->fd->fg)); */
/*       } else if (guard->fg->fd == guard->fd->fg ) { */
/* 	 (y.x)+(x.z) = x.(y+z)  */
/* 	return newOpPred('.',guard->fd->fg,newOpPred('+',guard->fd->fd,guard->fg->fg)); */
/*       } else if (guard->fd->fd == guard->fg->fd ) { */
/* 	 (y.x)+(z.x) = x.(y+z)  */
/* 	return newOpPred('.',guard->fg->fd,newOpPred('+',guard->fg->fg,guard->fd->fg)); */
/*       } */
/*     } */


  return guard;
}





Guard * Guard::ArbreQuotient (Guard * guard,Guard * diviseur) {
  Guard * ret=NULL;
  
  if (guard == NULL) 
    return NULL;
  if (diviseur == NULL || diviseur->op != 'd')
    return guard;
  
  
  if (*guard == *diviseur) {
    ret = new Guard ("TRUE");
  } else if (guard->var == diviseur->var) {
    ret = new Guard("FALSE");
  } else if (guard->op  == '+' || guard->op  == '.' ) {
    if (guard->op  == '+') ret = new Guard ("OR", ArbreQuotient(guard->fg,diviseur), ArbreQuotient(guard->fd,diviseur));
    else ret = new Guard ("AND", ArbreQuotient(guard->fg,diviseur), ArbreQuotient(guard->fd,diviseur));
  } else if (guard->op == 'h') { 
    guard->fg = ArbreQuotient(guard->fg,diviseur) ;
  } else {
    /* this is a leaf predicate on a differnet variable */
    return new Guard(*guard);
  }
  return ret;
}


void Guard::RemontePred (PredPath * predPath) {

   Guard *A,*B,*C=NULL,*X;
   Guard *P1,*P2,*P3=NULL;
 
 /* shorthands... we have the structure : 

               P3
              /  \ 
            P2    C
           /  \
          P1   B
         / \   
        X   A
	
	X is the target pred, A,B & C are Expression trees, P1,P2,P3 are operator nodes + or .

	C and P3 are not always defined, we sometimes only need
	P1,P2 and A,B to push X up

	!DANGER! There are strong assumptions on what Path represents (i.e : pathlenght >= 2). If called 
	on a path resulting from FindMisbehavedPred or by internal recursion, things should be fine though
   */

  A=predPath->otherSon[0];
  B=predPath->otherSon[1];
  if (predPath->otherSon.size() >2) {
    C=predPath->otherSon[2];
    P3=predPath->path[2];
  }
  P1=predPath->path[0];
  P2=predPath->path[1];
  X= predPath->pred;

  if (P1->op == '+') {
    if (P2->op == '+') {
      /* (X+A)+B = (A+B) + X */
      P1->fg = A;
      P1->fd = B;
      P2->fg = X;
      P2->fd = P1;
      
      predPath->EatPathHead();
      /* depth still > 1 or we wouldn't be here */
      RemontePred(predPath);
    } else {
      /* (X+A).B = (X.B') + (A+B) */
      Guard * B2;
      Guard * newP = new Guard("AND",B,A);

      B2 = ArbreQuotientSimplifie(B,X);
      
      P1->op = '.';
      P1->fg = X;
      P1->fd = B2;

      P2->op = '+';
      P2->fg = P1;
      P2->fd = newP;

      if (predPath->depth > 2) {
	predPath->otherSon[1] = newP;
	predPath->otherSon[0] = B2;
	
	RemontePred(predPath);
      }
    }
  } else {
    /* X.(something) */
    if (P2->op == '.') {
      /* (X.A).B = (A'.B').X */
      Guard * A2,*B2;
      
      B2 = ArbreQuotientSimplifie(B,X);
      A2 = ArbreQuotientSimplifie(A,X);
      
      P1->fg=A2;
      P1->fd=B2;
      
      P2->fg = X;
      P2->fd = P1;
      
      predPath->EatPathHead();
      if (predPath->depth  > 1) {
	RemontePred(predPath);
      }
    } else {
      /* This case is more complex : we must disinguish cases on P3->op */
      if (P3->op == '.') {
	/*  C.(B+(X.A)) = (C.B)+(X.(A'.C')) */
	Guard * A2,*C2;
	Guard * newP=new Guard ("AND",B,C);

	C2 = ArbreQuotientSimplifie(C,X);
	A2 = ArbreQuotientSimplifie(A,X);

	P1->fg = A2;
	P1->fd = C2;

	P2->fg = X;
	P2->fd = P1;
	P2->op = '.';
	
	P3->op = '+';
	P3->fg = P2 ;
	P3->fd = newP;
	
	if (predPath->depth  > 3) {
	  predPath->EatPathHead();
	  
	  predPath->otherSon[0] = P1;
	  predPath->otherSon[1] = newP;

	  RemontePred(predPath);
	}
      } else {
	Guard * A2;
	/* ((X.A)+B)+C = (X.A') + (B+C) */
	A2 = ArbreQuotientSimplifie(A,X);

	P1->op = '+';
	P1->fg = B;
	P1->fd = C;
	
	P2->op = '.';
	P2->fg=X;
	P2->fd=A2;

	P3->fd=P1;
	P3->fg=P2;

	/* depth still > 1 or we wouldn't be here */
	predPath->EatPathHead();
	  
	predPath->otherSon[0] = A2;
	predPath->otherSon[1] = P1;
	
	RemontePred(predPath);
      }
    }	
    
  }
}


void Guard::PredPath::EatPathHead () {
  
  vector<Guard*> newPath = vector<Guard*> ();
  vector<Guard*> newotherSon=  vector<Guard*> ();
  vector<Guard*>::iterator it;
  it = path.begin();it++; // bizarre warning under gcc < 3.0
  newPath.insert( newPath.begin(),it,path.end());
  it = otherSon.begin();it++; // bizarre warning under gcc < 3.0
  newotherSon.insert(newotherSon.begin(),it,otherSon.end());

  
  
  path = newPath;
  otherSon = newotherSon ;
  depth--;
} 


Guard::minTerm::minTerm (Guard *pred,Guard * pereET,Guard *minT) {
  
  if (pereET != NULL) {
    pereET->fg = pred;
    pereET->fd = minT; 
  } else {
    /*pereET is NULL if this minTerm has no pred on targetVar or no minterm*/
  }
  this->pred = pred;
  this->minT = minT;
  this->pereET = pereET;

}

void Guard::FindMinTerm (Guard *guard,Variable *targetVar, list<minTerm> &minTab) {
  list<minTerm>::iterator it;
 
  /* debug */
//   cerr << "minTab : [" << endl ;
//   for (it = minTab.begin() ; it != minTab.end() ; it ++) {
//     cerr << "{ pred:" << it->pred ;
//     if (it->pred) cerr << "->" << *it->pred ;
//     cerr << ", pereET : " << it->pereET << ", minT: "<< it->minT << "}" << endl; 
//   }
//   cerr << "]" << endl ;

  if (guard == NULL)
    return;

  

  if (!isMentioned(guard,targetVar)) {
    /* this minterm has no pred on targetVar */
      minTerm minT =  minTerm (NULL,NULL,guard);
      it = upper_bound(minTab.begin(),minTab.end(),minT);
      minTab.insert(it,minT);
      return;
  }

   //  cerr << "\nin FindMinTerm" << *guard <<endl;
  if (guard->op == '+') {
      FindMinTerm (guard->fg,targetVar,minTab);
      FindMinTerm (guard->fd,targetVar,minTab);
  } else if  (guard->op == '.') {
    if (guard->fg->var == targetVar) {
      minTerm minT =  minTerm (guard->fg,guard,guard->fd);
      it = upper_bound(minTab.begin(),minTab.end(),minT);
      minTab.insert(it,minT);
    } else if (guard->fd->var == targetVar) {
      minTerm minT =  minTerm (guard->fd,guard,guard->fg);
      it = upper_bound(minTab.begin(),minTab.end(),minT);
      minTab.insert(it,minT);
    } else {
      /* this minterm has no pred on targetVar */
      minTerm minT =  minTerm (NULL,NULL,guard);
      it = upper_bound(minTab.begin(),minTab.end(),minT);
      minTab.insert(it,minT);
    }
  } else {
    if (guard->var == targetVar) {
      minTerm minT =  minTerm (guard,NULL,NULL);
      it = upper_bound(minTab.begin(),minTab.end(),minT);
      minTab.insert(it,minT);
    } else if (guard->op == 'h') {
      FindMinTerm (guard->fg,targetVar,minTab);
    } else {
      /* this minterm has no pred on targetVar */
      minTerm minT =  minTerm (NULL,NULL,guard);
      it = upper_bound(minTab.begin(),minTab.end(),minT);
      minTab.insert(it,minT);
    }
  }

  /* debug */
//   cerr << "minTab : [" << endl ;
//   for (it = minTab.begin() ; it != minTab.end() ; it ++) {
//     cerr << "{ pred:" << it->pred ;
//     if (it->pred) cerr << "->" << *it->pred ;
//     cerr << ", pereET : " << it->pereET << ", minT: "<< it->minT << "}" << endl; 
//   }
//   cerr << "]" << endl ;



}

Guard::minTerm & Guard::minTerm::operator= (const Guard::minTerm & m) {
  if (this != &m) {
    pred = m.pred;
    pereET = m.pereET;
    minT = m.minT;
  }
  return *this;

}

bool operator<(const Guard::minTerm &a, const Guard::minTerm &b) {

  if (&a == &b)  return 0;
   
  if (a.pred == b.pred) return 0;
  if (a.pred == NULL) return 0;
  if (b.pred == NULL) return 1;
  return ( a.pred->set[0] < b.pred->set[0] ) ;
}

list<Variable*> Guard::FindMentionedVars () {
  list<Variable*> l = list<Variable*> ();
  FindMentionedVars (l) ;
  return l;
}

void  Guard::FindMentionedVars (list<Variable*> &l) {
  if (fg != NULL) fg->FindMentionedVars(l);
  if (fd != NULL) fd->FindMentionedVars(l);

  if (op == 'd') {
    list<Variable*>::iterator it;
    it = upper_bound(l.begin(),l.end(),var);
    if (it==l.begin() || *(--it) != var) l.insert(++it,var);
  }
 
}

bool Guard::isMentioned(Guard *g,Variable *var) {
  if (g == NULL) 
    return 0;
  if (g->op == '=' || g->op == '!')
    return 0;
  if (g->var == var) 
    return 1;
  return (isMentioned(g->fg,var) || isMentioned(g->fd,var));
 
}


Guard * Guard::CanonizePredTree (Guard * guard,Variable * curVar) {

  PredPath * predPath;
  list<minTerm>  minTab ;
  list<minTerm>::iterator it,jt;
 

//  if (lvar.empty()) return guard;

  if (guard == NULL) return NULL;
  if (guard->op == 'h') {
    guard->fg = CanonizePredTree (guard->fg,curVar); 
    return guard;
  }

//  cerr << "guard b4 simplif " << *guard<<endl;
//  guard = SimplifierArbre (guard);
 // cout << "guard after simplif" <<*guard << endl;
  
//      fprintf(stderr," Running Canonize on variable %d (startvar %d) with guard = \n",curVar, startVar); 
//      PrintPredTree(guard); 
//      fprintf(stderr,"\n"); 

//   cerr << "before find mibehaved with var =" << *curVar << ":" << *guard << endl ;
  while ((predPath =FindMisbehavedPred(guard,curVar,0)) != NULL) { 
    RemontePred(predPath);
    guard = SimplifierArbre (guard);
 
  }  
//  cerr << "after : " << *guard << endl ; 
  
  Guard::FindMinTerm(guard,curVar,minTab);

  /* debug display */
//   cerr << "in Canon minTab : [" << endl ;
//   for (it = minTab.begin() ; it != minTab.end() ; it ++) {
//     cerr << "{ pred:" << it->pred ;
//     if (it->pred) cerr << "->" << *it->pred ;
//     cerr << ", pereET : " << it->pereET << ", minT: "<< it->minT << "}" << endl; 
//   }
//   cerr << "]" << endl ;
  
    
  for (it= minTab.begin();it != minTab.end() ;it++) {
    if ( it->minT ) 
      if (! it->minT->FindMentionedVars().empty() ) 
	it->minT = CanonizePredTree ( it->minT, *it->minT->FindMentionedVars().begin() );
  }
  
  /* debug display */
//     cerr << "it :" << "{ pred:" << it->pred ;
//     if (it->pred) cerr << "->" << *it->pred ;
//     cerr << ", pereET : " << it->pereET << ", minT: "<< it->minT << "}" << endl; 
    

//  cerr << "b4 fusion" << *guard << endl;
  /* factorisation/fusion des pred qui ont meme MinTerm */
 
  for ( it= minTab.begin(); it != minTab.end() ; it++) {
    for (jt= ++it,it--; jt != minTab.end() ; ) {
      if (  (!it->minT  && !jt->minT)   || (it->minT  && jt->minT && *it->minT == *jt->minT )) {
	it->pred = FusionnePred (it->pred,jt->pred);
	if ( it->pereET) it->pereET->fg =  it->pred;
	jt = minTab.erase(jt);
	/*we've  already shifted ... */
      } else jt++;
    }
  }
  
//  cerr << "b4 minT" << *guard << endl;
  
   if (minTab.empty()) {
     guard= NULL;
   } else if (minTab.size() == 1) {
     it =  minTab.begin();
     guard = (it->pereET)?it->pereET:((it->pred)?it->pred:it->minT) ;
   } else {
     /* build the tree from last on up */
     list<minTerm>::reverse_iterator rit; 
     rit =  minTab.rbegin() ;
     Guard * lastElt = (rit->pereET)?rit->pereET:((rit->pred)?rit->pred:rit->minT) ;
     it++;
     Guard * B4lastElt = (rit->pereET)?rit->pereET:((rit->pred)?rit->pred:rit->minT) ;
     Guard * newP = new Guard("OR",B4lastElt,lastElt);
     for ( ;rit != minTab.rend() ;rit ++) {
       Guard * elt =  (rit->pereET)?rit->pereET:((rit->pred)?rit->pred:rit->minT) ;
       newP = new Guard ("OR",elt,newP);
     }
     guard = newP;
   }

//   cerr << "after mint " << *guard <<endl;

//    if (curVar == startVar) {   
//      fprintf(stderr,"Obtained expression : variable %d (startvar %d) with guard = \n",curVar, startVar); 
//      PrintPredTree(guard); 
//      fprintf(stderr,"\n\n"); 
//    }  
//    cerr << "guard:" << guard <<endl;

//    cerr << "guardSimple:" << *SimplifierArbre(guard) <<endl;
  return SimplifierArbre(guard);

}

list<vector<Element> > Guard::getAssymetry (Variable *v) {

  EnsureUnicity();
  list<vector<Element> > lcall ;
  list<vector<Element> > lcallres;
  list<Guard *>::iterator it;
  list<Guard *> &l = this->FindPredOnVar (*v);  

  if (! l.empty() ) {
    Guard * g = CanonizePredTree (this,v);
    l = g->FindPredOnVar (*v); 
    for (it = l.begin();it != l.end() ; it++ )
      lcall.push_back( (*it) -> set);
  }
  lcall.push_back(v->PClass()->Elts());

  lcallres = calcSub::uniquePartition (lcall);

  
//  for (it = l.begin();it != l.end() ; it++ )
//    (*it)->RewritePredWithPart (lcallres);
  
//  cerr << endl << "Get Assymetry over variable " << *v << " for guard g=" << *this << endl;
//  calcSub::print(cerr,lcallres);
  return lcallres;

}






void Guard::Not () {
  switch (op) 
    {
    case '.' : { op = '+' ; fg->Not(); fd->Not(); break;}
    case '+' : { op = '.' ; fg->Not(); fd->Not(); break;}
    case '-' : { *this = *fg ; break ;}
    case '=' : { op = '!' ; break; }
    case '!' : { op = '=' ; break; }
    case 'T' : { op = 'F' ; break ; }
    case 'F' : { op = 'T' ; break ; }
    case 'd' : 
      {
	vector<Element> v = var->PClass()->Elts() ;
	list<vector<Element> > l;
	l = calcSub::intersect (v,set);
	set = *l.begin();
	break;
      }
    case 'v':
      {
	break;
      }
    case 'h':
      {
	fg->Not();
      }
    }

}

void Guard::ApplyNot () {
  if (op == '-') { fg->Not() ; *this = *fg ;}
  if (fg!= NULL) { fg->ApplyNot(); }
  if (fd!= NULL) { fd->ApplyNot(); }


}




