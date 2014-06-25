#include "Transition.h"
#include "PNetIO.h"

//Implementation for class Transition
//default constructor
Transition::Transition (int id,long NumMac,string name,/*Domain *dom,*/int priority ) {
  this->id = id;
  this->numeroMacao=NumMac;
  this->name=name; 
//  this->dom=dom;
  guard =new Guard("head","true");
  this->priority = priority;
  this->nserv = 1;
}

Transition::Transition (const Transition & t): id(t.id),numeroMacao(t.numeroMacao),name(t.name),priority(t.priority),nserv(t.nserv),arcIn(t.arcIn),arcOut(t.arcOut) {
  guard =new Guard(*t.guard);
}


//default destructor
Transition::~Transition (){}

// *******************basic operators


// comparison by operator==
bool operator==(const Transition& a,const Transition& b){
  return (a.id==b.id);
}

// comparison by operator<
bool operator<(const Transition& a,const Transition& b){
  return (a.id<b.id);
}


// printing with operator<<
ostream& operator<<(ostream& os,const Transition& t){
  os << "Transition "<<t.name<<" internal id="<<t.id<<" MacaoNum="<<t.numeroMacao;
 // if (t.guard != "") os<<" guard :\""<<t.guard<<"\"";
  if (t.priority != 0) os << " Immediate of priority " << t.priority ;
  os << " guard :\""<<*t.guard<<"\"";
/*  
  list<Arc *>::const_iterator it ;
  os << "Arcs In :";
  for (it = t.arcIn.begin() ; it != t.arcIn.end() ;it ++) {
    os << (**it) << " ,"  ;
  }
  os << "\nArcs out : " ;
  for (it = t.arcOut.begin() ; it != t.arcOut.end() ;it ++) {
    os << **it << " ," ;
  }
*/
  return os;
}

// Add an arc 
int Transition::AddArc (Arc *pa) {
  if (pa->dir == Trans2Place) 
    arcOut.push_back(pa);
  else 
    arcIn.push_back(pa);
  return 1;
}

void Transition::FindOrderedClasses () {
  guard->FindOrderedClasses ();
}

void Transition::RewriteWithStaticClasses () {
  guard->RewritePredWithClasses ();
}


Guard *  Transition::setGuard (const string &s, bool isGSPN ) 
{ 
//  cerr << "attempting parse of: "<<s <<endl;
  guard=new Guard("head",s,name,isGSPN); 
  return guard;
}



int Transition::ExportToCami (ostream &os) {
  if (priority==0) 
    os << "CN(10:transition," << numeroMacao << ")\n";
  else
    {
      os << "CN(20:immediate transition," << numeroMacao << ")\n";
//      os << "CT(8:priority,"<< numeroMacao << ","<< priority << ")\n";
    }
  os << pos.getCamiPO(numeroMacao) << endl;
  if (name != "") {
    os << "CT(4:name," << numeroMacao << ","<< CamiFormat(name) << ")\n";
    os << namepos.getCamiPT(numeroMacao,"name") << endl;
  }
  string g = guard->getCamiFormat();
  if (g != "[TRUE]") {
    os << "CT(5:guard," << numeroMacao << ","<< CamiFormat(guard->getCamiFormat()) << ")\n";
    os << guardpos.getCamiPT(numeroMacao,"guard") << endl;
  }
  if (priority)
    os << "CT(8:priority," << numeroMacao << ","<< CamiFormat(priority) << ")\n";

  return 0;
}


int Transition::ExportToGSPN (ostream &os,ostream &deff,int & numligne) {
  if (name != "")
    os << name ;
  else 
    os << "T_" << id ;
  os << "  " ;
  os << "1.0   " ; // rate of the transition
  os << nserv << "   ";  // number of servers
  os << priority << "   " ; // Stochastic transition kind  : 0 = exponential ; 127 = deterministic ; 0 < p < 127 = relative priority of trans
  list<Arc *>::iterator it ;
  list<Arc *> arcInhibitor ;
  for (it = arcIn.begin() ; it != arcIn.end() ;it ++) {
    if ( (*it)->IsInhibitor() ) arcInhibitor.push_back(*it);
  }
  os << arcIn.size()- arcInhibitor.size() << " ";
  os << "0 " ; // rotation coef of the trans  in graphical interface
  os << pos.getGspn() << " " ; // position of trans
  os << namepos.getGspn() << " " ; // position of name tag
  os << "1.0 1.0 " ; // position of ??
  // Definition not clear on order of pos  ??? cimer GSPN
  guard->ExportToGSPN(os);  


  
  for (it = arcIn.begin() ; it != arcIn.end() ;it ++) {
    if ( ! (*it)->IsInhibitor() ) (*it)->ExportToGSPN(os,deff,numligne);
  }
  os << "   " << arcOut.size() << endl ;
  for (it = arcOut.begin() ; it != arcOut.end() ;it ++) {
    (*it)->ExportToGSPN(os,deff,numligne);
  }
  os << "   " << arcInhibitor.size() << endl ;
  for (it = arcInhibitor.begin() ; it != arcInhibitor.end() ;it ++) {
    (*it)->ExportToGSPN(os,deff,numligne);
  }


  return 0;
}

int Transition::ExportToSmart (ostream &os ) {
  // trans declaration
  os << "     trans " << name << " ; \n";
  // firing rate 
  os << "     firing (" << name << ":expo(1.0)) ;"  << endl ;
  // arcs
  os << "     arcs (";
  list<Arc *>::iterator it ;
  list<Arc *> arcInhibitor ;
  bool first = true ;
  for (it = arcIn.begin() ; it != arcIn.end() ; it ++ ) {
    if ( (*it)->IsInhibitor() ) 
      arcInhibitor.push_back(*it);
    else  {
      if (! first)
	os << ", " ;
      else 
	first = false;
      (*it)->ExportToSmart(os) ;    
    }
  }
  for (it = arcOut.begin() ; it != arcOut.end() ; it ++ ) {
    if (! first)
      os << ", " ;
    else 
      first = false;
    (*it)->ExportToSmart(os);
  }
  os << "  ); \n";
  
  first = true ;
  // inhibitor arcs
  if (! arcInhibitor.empty()) {
    os << " inhibit(";    
    for (it = arcInhibitor.begin() ; it != arcInhibitor.end() ; it ++) {
      if (! first)
	os << ", " ;
      else 
	first = false;
      (*it)->ExportToSmart(os);
    }
    os << "  ); \n";
  }
  
  return 0;
}


int Transition::ExportToPnddd (ostream &os ) {
  // trans declaration
  os << "#trans " << name << "\n";
  // firing rate : unsupported
  // os << "     firing (" << name << ":expo(1.0)) ;"  << endl ;

  // arcs
  if (! arcIn.empty()) {
    os << "in {";
    list<Arc *>::iterator it ;
    for (it = arcIn.begin() ; it != arcIn.end() ; it ++ ) {     
	(*it)->ExportToPnddd(os) ;          
    }
    os << "}\n";
  }
  if (! arcOut.empty()) {
     os << "out {";
     list<Arc *>::iterator it ;
     for (it = arcOut.begin() ; it != arcOut.end() ; it ++ ) {
       (*it)->ExportToPnddd(os);
     }
     os << "}\n";
  }

  os << "#endtr\n" << endl;
  return 0;
}
