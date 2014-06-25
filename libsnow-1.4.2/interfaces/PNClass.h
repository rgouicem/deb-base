#ifndef _PNCLASS_H_
#define _PNCLASS_H_
#include <iostream>
#include <string>
#include <vector>
#include <list>


//#include "Element.h" 
#include "ParserWrapper.h"

// in reference to matrix format defining class type
enum classType {
  Integer, // sorry INTEGER conflicts with bison macro
  Char,
  Parametre,
  Intervalle,
  Enumere,
  Statique,
  Dynamique
};

  

//Declaration header file for class PNClass
class PNClass {
 protected:
  string name;
  int id;
  classType type;
  vector<class Element> elts;
  list<class PNClass*> subclasses;
  PNClass *parent;
  string prefix ;
  bool isOrdered ;
 public:
  // *******************Constructor/destructor 
  //default constructor
  // for INT,CHAR, PARAMETRE ANd ENUMERATED CLASSES*/
  PNClass (int id=-1,classType type=Integer,const string& name="");
  // for Interval classes
  PNClass(int id,string &name,int min,int max,const string &prefix=""); 
  // for  subclasses
  PNClass(int id,string &name,PNClass* parent,bool addsubtoparent=true);
  //default destructor
  virtual ~PNClass ();
  
  // *******************basic operators
  // comparison by operator==
  friend bool operator==(const PNClass&,const PNClass&);
  // comparison by operator<
  friend bool operator<(const PNClass &,const PNClass &);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const PNClass&);
 
 

  ////////// Used for constructing a class
  // add an element 
  int AddElt (const class Element&);
  // adds to subclass , checks existence of element in parent class
  int AddEltToStatic (const string &s) ;
  // adds a subclass to a primary class
  void addSub(PNClass *sub) ;
  

  //accessors
  list<PNClass *> & SubClasses () {return subclasses ;};

  const vector<class Element>& Elts() const {return elts;}
  vector<class Element>& Elts() {return elts;}

  PNClass* const Parent () const { return parent; }
  void setParent(PNClass *par) { parent = par ;}
  void setOrdered (bool o) {isOrdered = o;}

  const string Name() const {return name;}
  void Name(const string& s) {name = s;}
  //  string Name (const string &s="I WANT GET") { if (s != "I WANT GET") name=s;return name;}

  const int Id() const {return id;}
  void Id(const int n) {id = n;}
  //  int Id (int ii=-1) { if (ii!=-1) id=ii;return id;}

  classType & Type () {return type; }
  const classType& Type() const {return this->type;}
  // <list> like access primitives
  class Element * Find (const string& name);

  const unsigned int size() const {return elts.size();}

  
 
  // returns the sets of the subclasses of a parent class
  list<vector<class Element> > getSubSets() ;
  // returns the sets of the DYNAMIC subclasses of a parent class
  list<vector<class Element> > getDynSubSets() ;
  // returns a range of elements defined by a comparison predicate
  vector<class Element> getRange (const string &oprel,class Element *borne) ;
  // Returns the subclass that contains a given set of elements or NULL if mismatch
  PNClass * FindSubContaining (const vector<Element> &v);
  PNClass * FindDynSubContaining (const vector<Element> &v);


  // exports to a filestream, numligne is line declaration number in 
  // attribute declaration of object net (macao object number 1)
  int ExportToCami(ostream &os,int &numligne);
  void ExportToGSPN (ostream &os) ;

  // Allow direct manipulation from the Container class PNClasses
  friend class PNClasses;
  friend int gcolparse ();
};


#endif

