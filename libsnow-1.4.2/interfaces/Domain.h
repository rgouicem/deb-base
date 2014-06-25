#ifndef _DOMAIN_H_
#define _DOMAIN_H_
#include <iostream>
#include <list>

class PNClass;
using namespace std;
//Declaration header file for class Domain


class Domain {
protected:
  list<class PNClass*> elts;
  string name;
public:
  // *******************Constructor/destructor 
  //default constructor
  Domain (const string &nname=""):name(nname){};

  //default destructor
  virtual ~Domain ();

  // *******************basic operators
  // comparison by operator==
  friend int operator==(const Domain&,const Domain&);
  // comparison by operator<
  friend int operator<(const Domain&,const Domain&);
  //printing with operator<<
  friend ostream& operator<<(ostream&,const Domain&);

  //accessors
  inline unsigned int Size() const {return elts.size(); }

  const string Name() const {return name;}
  void Name(const string& s) {name = s;}
  //  string Name (const string &s="I WANT GET") { if (s!="I WANT GET") name=s;return name;}

  const list<PNClass*>& Elts() const {return elts;}
  list<PNClass*>& Elts () {return elts;}

  /* insert sorted */
  int AddElt(PNClass *);
  
  // exports to a filestream, numligne is line declaration number in 
  // attribute declaration of object net (macao object number 1)
  int ExportToCami(ostream &os,int &numligne);

  int ExportToGSPN (ostream &os);
  
  /* generates a name if none defined , either null if no classes in dom, the name 
     of the class if single, or domXX where XX is nextNum */
  void genName(int &nextNum);


  /* identify a domain by a list of names of its classes */
  bool IdentifyNameList(const list<string> & l);

};
#endif

