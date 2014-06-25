#include "Variables.h"
#include "PNetIO.h"
#include <algorithm>

//Implementation for class Variables
//default constructor
Variables::Variables () {
  
}


//default destructor
Variables::~Variables (){}

// *******************basic operators




// printing with operator<<
ostream& operator<<(ostream& os,const Variables& v){
  list<Variable>::const_iterator it;
  os<<"Variables: {"<<endl;
  for (it = v.lst.begin();it != v.lst.end();it++) {
    os<<*it<<","<<endl; 
  }
  os<<"} End Variables"<<endl;
  return os; 
}

/* sorted insert */
Variable* Variables::Insert (const Variable &p) {
  list<Variable>::iterator it;
  it = upper_bound(lst.begin(),lst.end(),p);
  lst.insert(it,p);
  it--;
  return &(*it);
}

list<Variable *> Variables::Find(PNClass * pclass) {
  list<Variable>::iterator it;
  list<Variable * > lret;
  for (it=lst.begin() ; it != lst.end() ; it ++) {
    if (it->PClass() == pclass) lret.push_back(&(*it)) ;
  }
  return lret ;
}

Variable* Variables::Find(int id2find) {
  list<Variable>::iterator it;
  it=find(lst.begin(),lst.end(),Variable(id2find));
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}

Variable* Variables::FindName (const string &name2find) {
  list<Variable>::iterator it;
  it=find(lst.begin(),lst.end(),Variable(-1,name2find));
  if (it!=lst.end()) return &(*it);
  else return NULL; 

}

int Variables::ExportToCami(ostream &os,int &numligne) {
  if (! this->lst.empty())
    {
      os << "CM(11:declaration,1," << numligne++ << ",1,3:Var)\n";
      // classes are classes that have variables in their domain
      list <PNClass*> classes;
      list<Variable>::iterator it;
      // it: travels member variable list
      for (it = lst.begin();it!=lst.end();it++) {
	// if class hasn't been encountered yet
	if ( find(classes.begin(),classes.end(),it->PClass()) == classes.end() )
	  // add it in classes
	  classes.push_back(it->PClass());
      }
      list<PNClass*>::iterator jt;
      
      // jt: travels classes list we ve just constructed
      for (jt = classes.begin();jt!=classes.end();jt++) {
	string out; 
	// it: travels member variable list
	int first=1;
	for (it = lst.begin();it!=lst.end();it++) {
	  if (it->PClass()  == *jt) {
	    if (first) first=0; else out+=",";
	    out += it->Name();
	  }
	}
	out += " in "+ (*jt)->Name() +";";
	os << "CM(11:declaration,1,"<<numligne++<<",1,";
	os <<CamiFormat(out)<<")\n";
      }
    }
  return 0;
}
