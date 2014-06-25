#include "Domains.h"
#include <cstdio>
#include <algorithm>
#include "PNetIO.h" // for FindName functor class

//Implementation for class Domains
//default constructor
Domains::Domains () {
  NextFreeDomId=1;
  Domain d;
  Insert(d);
}


//default destructor
Domains::~Domains (){}

// *******************basic operators





// printing with operator<<
ostream& operator<<(ostream& os,const Domains& d){
  list<Domain>::const_iterator it;
  os<<"Domains: {"<<endl;
  for (it = d.lst.begin();it != d.lst.end();it++) {
    os<<*it<<endl; 
  }
  os<<"} End Domains"<<endl;
  return os; 
 
}


// **********Advanced ops
/* sorted insert */

Domain * Domains::Insert(Domain &d) {
  list<Domain>::iterator it;
  if ( (it =find(lst.begin(),lst.end(),d)) !=lst.end() ) 
    return &(*it);
  it = upper_bound(lst.begin(),lst.end(),d);
  lst.insert(it,1,d);
  it = find(lst.begin(),lst.end(),d);
  it->genName(NextFreeDomId);
  return &(*it);
}


Domain * Domains::FindName(const string & name) {
  list<Domain>::iterator it;
  class FindName fn(name);
  it = find_if(lst.begin(),lst.end(),fn);
  if (it!=lst.end()) return &(*it);
  else return NULL; 
}

Domain * Domains::FindNameList (const list<string> & l) {
  list<Domain>::iterator it;

  for (it = lst.begin() ; it != lst.end() ; it++) 
    if (it->IdentifyNameList(l))
      return &(*it);

  return NULL;
}



class filter_null_domain_t
{
public:
  inline bool operator() (const Domain& d)
  {
    return d.Name() != "null";
  }
};

int Domains::ExportToCami(ostream &os,int &numligne) {
  // Filter null domain :
  unsigned int size(count_if(this->lst.begin(), this->lst.end(), filter_null_domain_t()));
  //  if (lst.size() > 0) {
  if (size != 0) {
    os << "CM(11:declaration,1,"<<numligne++<<",1,6:Domain)\n";
    list<Domain>::iterator it;
    for (it = lst.begin();it!=lst.end();it++) {
      it->ExportToCami(os,numligne);
    }
  }
  return 0;
}
