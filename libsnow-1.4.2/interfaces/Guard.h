#ifndef _GUARD_H_
#define _GUARD_H_
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <list>


class Variable;
class Element;

using namespace std;

//Declaration header file for class Guard


class Guard {
protected:
  Variable *var;
  char op; // +=OR .=AND -=NOT v=(Var=Var) = =(Var=Var) ! =(Var!=Var)  d=dom h=head T=true F=False
  vector<Element> set;
  class Guard *fg;
  class Guard *fd;
  int nbref;
  int succ;
  // *******************Constructor/destructor 

 /* creates a plain variable with succ use in X = !Y and != leaves */
  Guard (Variable *v,int _succ);

  

  /* A internal class for our Canonization process, stores a trio of pointers into a tree structure*/
  class minTerm {
  public :
    Guard * pred;
    Guard * pereET;
    Guard * minT;
    
    minTerm(const minTerm &m): pred(m.pred),pereET(m.pereET),minT(m.minT) {};
    minTerm(Guard *pred=NULL,Guard * pereET=NULL,Guard *minT=NULL) ;
    minTerm & operator=(const minTerm& m);
    friend bool operator<(const minTerm &a,const minTerm &b) ;

  };
  
  /*  Internal class used to find deep predicates and float them up to the surface */
  class PredPath {
  public:
      Guard *pred;
      vector<Guard*> path;
      vector<Guard*> otherSon;
      int depth;
   
      PredPath (Guard * pred,int depth);
      void addToPath (Guard * p,Guard * son) ;
      void EatPathHead ();
  };


  /******** Simplification primitives used by the public SimplifierArbre and related functions*/
  /* calculates the quotient tree */
  static Guard * ArbreQuotient (Guard * guard,Guard * diviseur);
  /* Finds terms at the same OR level, ie A OR ( B OR C ) -> {A,B,C} */
  static void getOrTerms(Guard * guard,vector<Guard *> &lor);
  /* For internal recursion when looking for deep preds, constructs a path to the pred that needs 
     to be floated up */ 
  static class PredPath * FindMisbehavedPred (Guard * guard,Variable *targetVar,int curdepth);
  /* X in E OR X in E2 = X in (E union E2) */
  static Guard * FusionnePred (Guard *a,Guard *b) ;
  /* higher level primitive that wraps calls to quotient and SimplifierArbre */
  static Guard * ArbreQuotientSimplifie (Guard * guard,Guard * diviseur);
  /* Used to reconstruct the tree in the canonization phase */
  static void FindMinTerm (Guard *guard,Variable *targetVar, list<minTerm> &minTab) ;
  /* Searches for all preds on v, and partitions the mentionned sets to ensure mentionned sets are distinct*/
  void EnsureUnicity (Variable *v) ;
  /* Recursive call that finds the preds on a given var */
  list<class Guard *>& FindPredOnVar (Variable &v,list<Guard*> *l= new list<Guard*> () );
  /* splits predicates into the sets mentionned in l, used in the rewriting phase */ 
  void RewritePredWithPart ( list<vector<Element> > & l ) ;
  /* Floats a predicate up to a depth of at most 1 */
  static void RemontePred (PredPath * predPath);
  /* Applys a not operation */
  void Not();
public:

  //default constructor, explicit mention of operator and sons
  Guard (string s="TRUE",Guard *fgg=NULL,Guard *fdd=NULL);
  /* Creates a (var=e1) predicate */
  Guard (Variable *,Element *,int _succ=0);
  /* Creates a (var in S) predicate*/
  Guard (Variable *,vector<Element>,int _succ=0);
  /* Creates a V1=V2 or V1!=V2 predicate, the string defining the comparison op should be "=" 
  or it will be interpreted as "!=" */
  Guard (Variable *,const string &,Variable *,int succ1=0,int succ2=0);
 


  // parser constructor
  Guard (const string &head,const string &guardstr, const string & tname = "t", bool isGSPN = false);
  //constructor by copy
  Guard (const Guard&);
  //default destructor
  virtual ~Guard ();
  string  getCamiFormat();
  // *******************basic operators
  // clone by operator=
  Guard& operator=(const Guard&);
  // comparison by operator==
  int operator==(const Guard&);

  
  Variable* const getVar() const {return this->var;}
  void setVar(Variable* v) {this->var = v;}

  const char getOp() const {return this->op;}

  const vector<Element>& getSet() const {return this->set;}
  vector<Element>& getSet() {return this->set;}
  void setSet(vector<Element>& elts) {this->set = elts;}

  class Guard* const getFg() const {return this->fg;}
  class Guard* const getFd() const {return this->fd;}
  //  int nbref;
  const int getSucc() const {return this->succ;}
  void setSucc(const int s) {this->succ = s;}

  // Advanced ops
  static Guard * SimplifierArbre (Guard * guard);
  static Guard * CanonizePredTree (Guard * ,Variable * );
  void RewritePredWithClasses ();
  int ExportToGSPN (ostream & os) ;
  

  /* Applies any negation predicates to make them disappear */
  void ApplyNot();

  list<vector<Element> > getAssymetry (Variable *) ;

  void EnsureUnicity ();
  void FindOrderedClasses ();
  //printing with operator<<
  friend ostream& operator<<(ostream&,const Guard&);

  friend int guardparse();
  friend int gguardparse();
  friend bool operator<(const minTerm &a,const minTerm &b) ;

  static bool isMentioned(Guard *g,Variable *var) ;
  list<Variable*> FindMentionedVars () ;
  void  FindMentionedVars (list<Variable*> &l) ;
};



#endif

