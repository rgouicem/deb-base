#include "Domain.h"
#include "PNetIO.h"
#include <cstdio>

//Implementation for class Domain

//default destructor
Domain::~Domain (){}

// *******************basic operators

// comparison by operator==
int operator==(const Domain& a,const Domain& b){
  list<class PNClass *>::const_iterator ita,itb;
  if (a.name != b.name) return 0;
  if (a.elts.size() != b.elts.size() ) return 0;
  for (ita=a.elts.begin(),itb=b.elts.begin(); ita !=a.elts.end() && itb != b.elts.end(); ita++,itb++) 
    if (*ita != *itb) return 0;
  return 1;
}

// comparison by operator<
int operator<(const Domain& a,const Domain& b){
  if (a.elts.size() < b.elts.size() ) return 1;
  if (a.elts.size() > b.elts.size() ) return 0;
  list<class PNClass *>::const_iterator ita,itb;
  for (ita=a.elts.begin(),itb=b.elts.begin(); ita !=a.elts.end() && itb != b.elts.end(); ita++,itb++) {
    if (**ita < **itb) return 1;
    else if (!(**ita == **itb)) return 0;
  }
  if (a.name < b.name) return 1;
  return 0;
}

// printing with operator<<
ostream& operator<<(ostream& os,const Domain& d){
  list<class PNClass *>::const_iterator it;
  os << "Domain "<<d.name<<": <";
  for (it=d.elts.begin();it!=d.elts.end();) {
    os << (*it)->Name();
    if (++it != d.elts.end()) os <<",";
  }
  os << ">";
  return os;
}



int Domain::AddElt(PNClass *pC) {
  this->elts.push_back(pC);
  return 1;
}

int Domain::ExportToCami(ostream &os,int &numligne) {
  if (elts.size() > 1) {
    os << "CM(11:declaration,1," << numligne++<<",1,";
    string out(name);
    out += " IS <";
    list<PNClass*>::iterator it;
    unsigned int i;
    for (it=elts.begin(),i=0;it!=elts.end();it++,i++) {
	out += (*it)->Name();
	if (i< (elts.size()-1)) 
	  out += ",";
    }
    out+=">;";
    os <<CamiFormat(out)<<")\n";
  } 
  return 0;
} 

int Domain::ExportToGSPN (ostream &os) {
  unsigned int i;
  list<PNClass*>::iterator it;

  for (it=elts.begin(),i=0;it!=elts.end();it++,i++) {
    os  << (*it)->Name();
    if (i< (elts.size()-1)) 
      os <<  ",";
  } 
  return 0;
}


/* generates a name, either null if no classes in dom, the name of the class if single, or domXX where XX is nextNum */
void Domain::genName(int &nextNum) { 
  if (name == "") {
    if (elts.size() ==0) {
      name = string("null");
    } else if (elts.size() ==1) {
      list<class PNClass *>::const_iterator it=elts.begin() ;
      name = (*it)->Name() ;
    } else {
      string out("dom");
      char buff[16] ;
      sprintf (buff,"%d",nextNum++);  
      out += buff;
      Name(out);
    }
  }
}



/* identify a domain by a list of names of its classes */
bool Domain::IdentifyNameList(const list<string> & l) {
  if (elts.size() != l.size()) 
    return false;

  if (l.size() == 0)
    return true;

  list<class PNClass *>::const_iterator it ;
  list<string>::const_iterator jt;

  for (it= elts.begin(), jt= l.begin(); it!= elts.end() ; it++,jt++) {
    if ((*it)->Name() != *jt)
      return false;
  }
  return true;


}
