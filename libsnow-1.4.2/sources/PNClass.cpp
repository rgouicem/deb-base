#include <string>
#include "PNetIO.h"
#include <cstdio>
#include <algorithm>
#include "PNClass.h"


//Implementation for class PNClass
//default constructor
PNClass::PNClass (int id,classType type,const string &name) {
  this->id=id;
  this->type=type;
  this->name=name;
  parent = NULL;
  isOrdered = false;
}


//default destructor
PNClass::~PNClass (){}

// *******************basic operators


// comparison by operator==
bool operator==(const PNClass& a,const PNClass& b){
  return (a.id == b.id);
}

// comparison by operator<
bool operator<(const PNClass& a,const PNClass& b){
  return (a.id < b.id);
}

// printing with operator<<
ostream& operator<<(ostream& os,const PNClass& p){
  os << "Class "<<p.name<<" is ";
  switch (p.type) {
  case Integer:{ os << "INTEGER" ;break;}
  case Char:{ os << "CHAR" ;break;}
  case Parametre:{ os << "PARAMETRE" ;break;}
  case Intervalle:{ os << "INTERVAL" ;break;}
  case Enumere:{ os << "ENUMERE" ;break;}
  case Statique:
    { 
    os << "STATIC" << "(of parent " ; 
    if (p.parent) 
      os << p.parent->name << ")" ; 
    else 
      os << "0x0)" ;
    break;
    }
  case Dynamique:
    { 
    os << "DYNAMIC" << "(of parent " ; 
    if (p.parent) 
      os << p.parent->name << ")" ; 
    else 
      os << "0x0)" ;
    break;
    }
  }

  if (! p.subclasses.empty()) {
    os << " of subclasses : " ;
    list<PNClass*>::const_iterator it;
    
    for (it = p.subclasses.begin();it!=p.subclasses.end();it++) {
      os << (*it)->name ;
      if (it != --p.subclasses.end()) os << ",";
      else os << "  ;";
    }
  }
  os << "(id : "<<p.id<<")";
  os << "{";
  if (! p.elts.empty() ) {
    vector<Element>::const_iterator it(p.elts.begin());
    os << *it;
    for (it++;it!=p.elts.end();it++) {
      os << "," << *it ;
    }
  }
  os << "}";
  return os;
}


vector<Element> PNClass::getRange (const string &oprel,Element *borne) {
  
  vector<Element> set ;
  vector<Element>::iterator itb = find (elts.begin(),elts.end(),*borne);
  
 
  if (oprel == "<") {
    set.insert(set.begin(),elts.begin(),itb);
  } else if (oprel == "<=") {
    set.insert(set.begin(),elts.begin(),++itb);
  } else if (oprel == ">") {
    set.insert(set.begin(),++itb,elts.end());
  } else if (oprel == ">=") {
    set.insert(set.begin(),itb,elts.end());
  } 

  return set;
}

/* sorted insert */
int PNClass::AddElt (const Element &e) {
  vector<Element>::iterator it;
  it = upper_bound(elts.begin(),elts.end(),e);
  elts.insert(it,1,e);
  return 1;
}



PNClass::PNClass(int id,string &name,int min,int max,const string &prefix) {
  this->id=id;
  this->type=Intervalle;
  this->name=name;
  this->parent = NULL;
  this->prefix = prefix ;
  isOrdered = false;
  int i,j;
  char buff[64];
  for (i=0,j=min;j<=max;i++,j++) {
    sprintf(buff,"%s%d",prefix.c_str(),j);
    this->AddElt(Element(i,buff));
  } 
}

PNClass::PNClass(int id,string &name,PNClass* parent,bool addsubtoparent) {
  this->id=id;
  if (parent->type== Statique)
    this->type=Dynamique;
  else
    this->type=Statique;
  this->name=name;  
  this->parent = parent;
  isOrdered = false;
  if (parent->prefix.size()) { this->prefix=parent->prefix; }
  if (addsubtoparent) parent->addSub(this);
}

void PNClass::addSub(PNClass *sub) {
  subclasses.push_back(sub);
}

/* sorted insert */
int PNClass::AddEltToStatic (const string &s) {
  const Element *e=parent->Find(s);
  if (e==NULL) {
    cerr << "\n Element \""<<s<< "\"  of "<<*this<<" should be a member of parent class :\n"<< *(this->parent)<< ";\n Insert Failed!!";
    return -1;
  } 
  this->AddElt(*e);
  return 1;
}

list<vector<Element> > PNClass::getSubSets() {
  list<vector<Element> > l;
  list<PNClass*>::iterator it;

  for (it = subclasses.begin();it!=subclasses.end();it++) {
    l.push_back((*it)->elts);
  }
  return l;


}

list<vector<Element> > PNClass::getDynSubSets() {
  list<vector<Element> > l;
  list<PNClass*>::iterator it,jt;

  for (it = subclasses.begin();it!=subclasses.end();it++) {
    if (! (*it)->subclasses.empty())
      for (jt = (*it)->subclasses.begin();jt!=(*it)->subclasses.end();jt++) {
	l.push_back((*jt)->elts);
      }
    else
      l.push_back((*it)->elts);
  }
  return l;


}
PNClass *  PNClass::FindDynSubContaining (const vector<Element> &v) {
  list<PNClass*>::iterator it,jt;

  for (it = subclasses.begin();it!=subclasses.end();it++) {
    if (! (*it)->subclasses.empty())
      for (jt = (*it)->subclasses.begin();jt!=(*it)->subclasses.end();jt++) {
	if (v == (*jt)->elts) return *jt;
      }
    else
      if (v == (*it)->elts) return *it;
  }
  return NULL;
}

PNClass *  PNClass::FindSubContaining (const vector<Element> &v) {
  if (type==Dynamique) return NULL;
  list<PNClass*>::iterator it;

  for (it = subclasses.begin();it!=subclasses.end();it++) {
    if (v == (*it)->elts) return *it;
  }
  return NULL;

}

Element * PNClass::Find (const string& name) {
  vector<Element>::iterator it;
  for (it=elts.begin();it!=elts.end();it++) 
    if (it->desc == name) return &(*it);
  return NULL;
}

int PNClass::ExportToCami(ostream &os,int &numligne) {

  //  skip export for dynamic subclasses 
  if (type == Dynamique)
    return 0;

  os << "CM(11:declaration,1," << numligne++<<",1,";
  string out(name);
  out += " IS ";
  switch (type) 
    {
    case Integer: {
      out += "INTEGER";
    break;
    }
    case Char: {
      out += "CHAR";
      break;
    }
    case Parametre: {
      out += "PARAMETRE";
      break;
    }
    case Intervalle: {
      elts.front().ExportToCami(out);
      out += "..";
      elts.back().ExportToCami(out);
      break;
    }
    case Statique: {
      out = name;
      out += " STATIC(" + parent->Name() + ")";
      out += " IS "; 
      if (parent->type == Intervalle) {
	int min,max,cur;
	bool hole = false;
	vector<Element>::iterator it;

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

	if (elts.size() < 3) {
	  hole = true;
	}

	if (!hole) {
	  elts.front().ExportToCami(out);
	  out += "..";
	  elts.back().ExportToCami(out);
	  break;
	}


	
      }
      // FALL THRU TO ENUMERATION OF ELTS
    } 
    case Enumere: {
      out += "[";
      vector<Element>::iterator it;
      unsigned int i;
      int len = 0;
      for (it=elts.begin(),i=0;it!=elts.end();it++,i++) {
	len += it->ExportToCami(out) + 1;
	if (i< (elts.size()-1)) 
	  out += ",";
	if (len > 130) {
	  os <<CamiFormat(out)<<")\n";
	  os << "CM(11:declaration,1," << numligne++<<",1,";
	  out = "";
	  len = 0;
	}
      }
      out += "]";
      break;
    }
      // just for compiler warning switch doesn't treat Dynamique
    case Dynamique :
      break;
  }
  out+=";";
  os <<CamiFormat(out)<<")\n";
  return (0);
}

void  PNClass::ExportToGSPN (ostream &os) {
  /*
  string name;
  if (this->name.length() > 10) {
    char buff[12];
    strcpy (buff,this->name.c_str() + this->name.length() - 10);
    name = buff;
  }
  */
  if (name == "S") 
    name = "class_S";
  os << "(" << name ;
  if (type == Dynamique )
    os    << " m " ;
  else
    os    << " c " ;

  /* insert positions as floats */
  os << "1.0 1.0 (@" ;
  if (type == Dynamique )
    os    << "m\n" ;
  else
    os    << "c\n" ;
  string out;
  if (type == Statique ) {
    switch (parent->type) 
      {
      case Integer: {
	cerr << " Your model uses integer classes which are not compatible with GSPN. Sorry. " << endl;
	exit(1);
	break;
      }
      case Char: {
	cerr << " Your model uses CHAR classes which are not compatible with GSPN. Sorry. " << endl;
	exit(1);
	
	break;
      }
      case Parametre: {
	cerr << " Your model uses PARAMETRE  classes which are not yet supported by this tool. Sorry. " << endl;
	exit(1);
	
	break;
      }
      case Intervalle: {
	int min,max,cur;
	bool hole = false;
	vector<Element>::iterator it;

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
	  if (prefix.size() == 0) out += "num" ;
	  else out += prefix ;
	  
	  out += "{";
	  
	  {
	    string::iterator it ;
	    it =  elts.begin()->desc.begin() ;
	    it += prefix.size() ;
	    string suff;
	    suff.insert(suff.begin() , it , elts.begin()->desc.end() );
	    out += suff ;
	  }
	  out += "-";
	  {
	    string::iterator it ;
	    it =  elts.back().desc.begin() ;
	    it += prefix.size() ;
	    string suff;
	    suff.insert(suff.begin() , it , elts.back().desc.end() );
	    out += suff ;
	  }
	  out += "}";
	  
	  break;
	}
	
	// holed => fall thru to enum case
      }
      case Enumere: {
	out += "{";
	vector<Element>::iterator it;
	unsigned int i;
	for (it=elts.begin(),i=0;it!=elts.end();it++,i++) {
	  it->ExportToGSPN(out);
	  if (i< (elts.size()-1)) 
	    out += ",";
	}
	out += "}";
	break;
      }
      case Statique: {
	cerr << " Your static subclass has a subclass ??? Fatal error. Sorry. " << endl;
	exit(1);

      }
      case Dynamique : break; // for compiler warning
      }
    os << out ;
  } else if (type == Dynamique) {
    os << "(" << parent->name << ":" << elts.size() << ")"  ; 
  } else {
    // main level color class
    if (isOrdered) os << "o" ; else os << "u" ;
    os << " ";
    list<PNClass *>::iterator it ;
    for (it=subclasses.begin() ;it!=subclasses.end();it++) {
      os << (*it)->name ;
      if (*it != subclasses.back()) os << "," ;
    }
   
  }

  os << endl << "))" << endl ;

}
