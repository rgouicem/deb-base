#include "ParserCAMI.h"
#include "FKCLDecode.h"
#include "LineParser.h"
#include "ParserWrapper.h"
#include "PNetIO.h"
#include <cstdlib>
#include <strings.h>
#include <cstring>


#ifdef FRAMEKIT_SUPPORT
#include <FKCLSend.h>
#include <FKModel.h>
#else
#define kFk_CAMI_CommandSize 1024
#endif


using namespace std;

PNet * ParserCAMI::PN = NULL ;
int ParserCAMI::nextNumPlace = 0;
int ParserCAMI::nextNumTrans = 0;
int ParserCAMI::nextNumArc = 0;
map< long, map<string, string> > ParserCAMI::cms;
map<Place*, string> ParserCAMI::delayedMarks;

ParserCAMI::ParserCAMI (PNet *PNN, const string & path) {
  /*  if (!PNN) {
      PN = new PNet(); 
      } else  PN = PNN;*/
  PN = PNN;
  nextNumPlace=0; 
  nextNumTrans=0;

  string pathff(path);
  
#ifdef FRAMEKIT_SUPPORT
  if (path == "")
    pathff=FkGetModelFileName();
#endif


  // Search .model file for relevant info on classes/dom/vars
  {

    ifstream modelFF(pathff.c_str());
    if (!modelFF) {
      cerr << "\nCould not open file " <<  pathff<<" in CAMI parser function!Please check if the file exists and permissions are set to readable\n"<< endl;
#undef exit
      exit(1);
    }


    // Looking for something like this :
    /*
      CM(11:declaration,1,1,1,5:Class)
      CM(11:declaration,1,2,1,20:Utilisateur is 1..4;)
      CM(11:declaration,1,3,1,41:Etat is [NonEtablie,Etablie,Interrompue];)
      CM(11:declaration,1,4,1,6:Domain)
      CM(11:declaration,1,5,1,34:Comm is <Utilisateur,Utilisateur>;)
      CM(11:declaration,1,6,1,43:EtatComm is <Utilisateur,Utilisateur,Etat>;)
      CM(11:declaration,1,7,1,3:Var)
      CM(11:declaration,1,8,1,21:i,j,k in Utilisateur;)
    */
    /* state : 0 no decl encountered;
       1 : Class encountered;
       2 :  Domain encountered
       3 : Var encountered */
    int state = 0;  
    // used to number classes internally
    int classNumber = 0; 
    int varNumber =0;
    while (1 && !modelFF.eof()) { // while input exists
      LineParser l(modelFF);
      
      l.setDelim("(");
      string tok;
      l.parse(tok);
      if (tok == "CM") { // multi line attribute
	l.parse('(');
	l.setDelim(",");
	
	l.parseCami(tok);
	l.parse(',');
	int n;
	l.parse(n);
	if (tok == "declaration" && n==1 ){
	  /* looking good here: reached declaration node attached to object number 1 = Net */
	  l.parse(',');l.parse(n);l.parse(',');l.parse(n);l.parse(',');
	  l.parseCami(tok);
	  l.parse(')');
	  LineParser ll(tok); 
	  ll.setDelim(", ");

	  if (! ll.eol() ) // dangerous empty declaration lines 
	    /*********** begin actual analysis of declaration contents */
	    switch (state)  
	      {
	      case 0: { // state : 0 no decl encountered;
		ll.parse(tok);
		if ( !strcasecmp(tok.c_str(),"CLASS") ) {
		  state = 1;
		}
		if (ll.eol())
		  break; 
		// else fall through to case 1
	      }
	      case 1: { // 1 : Class encountered;
		string tab[32];
		ll.parse(tab[0]);
		/* check for state change */
		if ( ! strcasecmp(tab[0].c_str(),"DOMAIN") ) {
		  state = 2;
		  if (ll.eol())
		    break; 
		  // else fall through to case 2
		} else if (! strcasecmp(tab[0].c_str(),"VAR") ) {
		  state = 3;
		  if (ll.eol())
		    break; 
		} else {// parsing class declaration
		  // parse:  class1,class2,class3 IS ...
		  // store class names in tab, i is number of classes declared on this line
		  int i;
		  PNClass *newClass=NULL;
		
		  for (i=1;i<32;i++) {
		    char c = ll.parse();
		    if (c == ',') {
		      ll.parse(tab[i]);
		    } else {
		      ll.ungetc();
		      break;
		    }
		  }
		  // parse IS keyword
		  string is;
		  PNClass * ParClass = NULL ;
		  ll.setDelim(" \t;()");
		  ll.parse(is);
		  if (! strcasecmp(is.c_str(),"STATIC") ) {
		    char cc=ll.parse();
		    if (cc == '(') {
		      // looking ok : STATIC(parentClass) 
		      string pclass;
		      ll.parse(pclass);
		      ParClass = PN->LClasse.FindName (pclass);
		      if (!ParClass) 
			ll.parseError("Name of an already defined non static class in definition of parent class in STATIC(ParentClass) definition");
		      cc = ll.parse();
		      if (cc != ')') 
			ll.parseError("Closing paren after parent class name in STATIC(ParentClass) definition");
		    } else {
		      ll.parseError("Opening paren before parent class name in STATIC(ParentClass) definition");
		    }
		    ll.parse(is);
		  }
		  if ( strcasecmp(is.c_str(),"IS") ) {
		    ll.parseError("keyword IS after class name and parent declaration");
		  } else { // start parsing class description
		    char cc=ll.parse();
		    if (cc == '[') { // enumerated class
		      classType ct = ParClass?Statique:Enumere;
		      newClass = ParClass?new PNClass(classNumber++,tab[0],ParClass):new PNClass(classNumber++,ct,tab[0]);
		      ll.setDelim(", \t]");
		      if (!ParClass) { // NOT static subclass
			for (int num=0;ll.parse(tok);num++) {
			  Element elt(num,tok);
			  newClass->AddElt (elt);
			  if ( ll.parse() == ']' )
			    break;
			} 
		      }else { // static subclass
			for (int num=0;ll.parse(tok);num++) {
			  newClass->AddEltToStatic (tok);
			  if ( ll.parse() == ']' )
			    break;
			} 
		      }
		      PN->LClasse.Insert(*newClass);
		    } else { // Not enumerated class
		      ll.ungetc();
		      string type;
		      ll.parse(type);
		    
		      if (!strcasecmp(type.c_str(),"INTEGER")) {
			/****************** INTEGER TYPE CLASS ******************/
			classType ct = Integer;
			newClass = new PNClass(classNumber++,ct,tab[0]);
			PN->LClasse.Insert(*newClass);
		      
		      } else if (!strcasecmp(type.c_str(),"CHAR")) {
			/****************** CHAR TYPE CLASS ******************/
			classType ct = Char;
			newClass = new PNClass(classNumber++,ct,tab[0]);
			PN->LClasse.Insert(*newClass);
		      
		      } else {
			/***************** INTERVAL SPECIFIED CLASS ****************/
			string fintype;
			ll.setDelim(";");
			ll.parse(fintype);
			type += fintype;
			LineParser ll2(type);
			int min,max;
			string prefix;
			ll2.setDelim("0123456789.");
			int hasPrefix = ll2.parse(prefix);
			ll2.setDelim(". ");
			ll2.parse(min);
			ll2.parse('.');ll2.parse('.');
			if (hasPrefix) {
			  string prefix2;
			  ll2.setDelim("0123456789.");
			  ll2.parse(prefix2);
			  ll2.setDelim(". )");
			  if (prefix != prefix2) 
			    ll.parseError("same prefix:\""+prefix+"\" in interval class specification but found \""+prefix2+"\" ");
			}
			ll2.parse(max);
			if (hasPrefix) 
			  newClass = new PNClass(classNumber++,tab[0],min,max,prefix);
			else
			  newClass = new PNClass(classNumber++,tab[0],min,max);
			if (ParClass) {
			  ParClass->addSub(newClass);
			  newClass->Type() = Statique ;
			  newClass->setParent(ParClass);
			}
			PN->LClasse.Insert(*newClass);
		      } // end INTERVAL specified class
		    } // end Not enumerated class
		  } // end parsing class description
		  /* finshed creating class1 of class1,class2,class3 IS xxx
		     create classes (and simple domains) for class2 class3 etc from the template of class1 
		  */ 
		  Domain d(tab[0]);
		  PNClass * pClass = PN->LClasse.Find(classNumber-1);
		  d.AddElt(pClass);
		  PN->LDom.Insert(d);

		  for (int j=1; j<i;j++ ) {
		    PNClass c2(*newClass);
		    c2.Name(tab[j]);
		    c2.Id(classNumber++);
		    PN->LClasse.Insert(c2);

		    Domain d(tab[j]);
		    PNClass * pClass = PN->LClasse.Find(classNumber-1);
		    d.AddElt(pClass);
		    PN->LDom.Insert(d);
		  }
		  break ;
		} // end parsing class declaration
		// fall through for domain encountered and !eol
	      } // case 1 : class encountered
	      case 2: { // case 2 : domain encountered
		if (state == 2) { 
		  string tab[32],name;
		  ll.parse(name);
		  /* check for state change */
		  if ( ! strcasecmp(name.c_str(),"VAR") ) {
		    state = 3;
		    if (ll.eol())
		      break; 
		    // else fall through to case 3
		  } else {// parsing domain declaration
		    Domain d(name);
		    // parse IS keyword
		    string is;
		    ll.setDelim(" \t");
		    ll.parse(is);
		    if ( strcasecmp(is.c_str(),"IS") ) {
		      ll.parseError("keyword IS after domain name declaration");
		    } else { // start parsing domain contents
		      /* expecting something like : EtatComm is <Utilisateur,Utilisateur,Etat>; */
		 
		      ll.parse('<');
		      ll.setDelim(",> ");
		      ll.parse(tab[0]);
		      int i;
		      for (i=1;i<32;i++) {
			char c = ll.parse();
			if (c == ',') {
			  ll.parse(tab[i]);
			} else {
			  ll.ungetc();
			  break;
			}
		      }
		      ll.parse('>');
		      ll.parse(';');
		      PNClass *pClass;
		      for (int j=0;j<i;j++) {
			pClass = PN->LClasse.FindName(tab[j]);
			if (pClass) {
			  d.AddElt(pClass);
			} else {
			  string mess("element number ");
			  char tmp[10];
			  sprintf(tmp,"%d",j+1);
			  mess += tmp;
			  mess += " of domain " + name + " to be name of a class; class named "+tab[j]+" is undefined";
			  ll.parseError(mess);
			}
		      }
		      PN->LDom.Insert(d);
		    }
		  }
		  break;
		} // end if (state == 2)
		// fall thru 
	      } // end case 2 : domain encountered
	      case 3: { // case 3 : var encountered
		string tab[32];
		ll.parse(tab[0]);
		int i;
		for (i=1;i<32;i++) {
		  char c = ll.parse();
		  if (c == ',') {
		    ll.parse(tab[i]);
		  } else {
		    ll.ungetc();
		    break;
		  }
		}
		// parse IN keyword
		string in;
		ll.setDelim(" \t;");
		ll.parse(in);
		if ( strcasecmp(in.c_str(),"IN") ) {
		  ll.parseError("keyword IN after variable name declaration");
		} else { // start parsing variable domain description
		  string name;
		  ll.parse(name);
		  PNClass * pClass = PN->LClasse.FindName(name);
		  if (!pClass) 
		    ll.parseError("valid class name in variable domain definition");
		  for (int j=0;j<i;j++) {
		    Variable v(varNumber++,tab[j],pClass);
		    PN->LVar.Insert(v);
		  }
		}
		break;} // end case 3 : var encountered
	      default: break;
	      } // end switch(state)    
	} // end treatment for this line of declaration
      } // end got CM(xxxx....) 
      else if (tok == "FB") { // end of parse
	break;
      }
    } // end while (1)
    
  } // end looking for declarations in .dec_model file
  


  /********* Taken copy/paste from Matrice main *****************/
  int result,paramc;
  char **paramv;
  FILE * descCAMI;
  char commande[kFk_CAMI_CommandSize];
  if ( (descCAMI = fopen(pathff.c_str(),"r")) == NULL ) {
    perror ("Error attempting to access model file.  \n Skipping load old attribute names  phase.");
    //    FkExit(kFk_EndOnError);
  } else {
    // ************ Start actual parse of CAMI file
    FkClInitDecodeCami();
    ParserWrapper::setModel(PN);
    FkClChangerFonction(CAMI_CN,&(this->InterfaceTraiterCN));
    FkClChangerFonction(CAMI_CT,&(this->InterfaceTraiterCT));
    FkClChangerFonction(CAMI_CA,&(this->InterfaceTraiterCA));
    //  FkClChangerFonction(CAMI_FB,this->InterfaceTraiterFB);
    FkClChangerFonction(CAMI_CM,&(this->InterfaceTraiterCM));
    FkClChangerFonction(CAMI_PO,&(this->InterfaceTraiterPO));
    FkClChangerFonction(CAMI_PT,&(this->InterfaceTraiterPT));
    
    
    do 
      {  
	if (  fgets(commande, kFk_CAMI_CommandSize, descCAMI) == NULL  ) break;
	if (strlen(commande)>4)
	  commande[strlen(commande)-1]='\0';
	result=FkClDecodeCami((char *)commande,(int *)&paramc, &paramv);
      }
    while  (result!= CAMI_FB );
    TraiterCM();
    TraiterMarks();
    fclose(descCAMI);
  }


}

void ParserCAMI::TraiterCM()
{
  Place* p = NULL;
  Transition* t = NULL;
  Arc* a = NULL;
  Domain* dom = NULL;
  for (map< long, map<string, string> >::iterator i = cms.begin();
       i != cms.end();
       ++i)
    for (map<string, string>::iterator j = i->second.begin();
	 j != i->second.end();
	 ++j)
      {
	//	std::cout << j->first << " : " << j->second << std::endl;
	if (j->first == "marking")
	  {
	    if ((p = PN->LPlace.FindMacao(i->first)) != NULL)
	      p->setMarking(j->second);
	    else
	      cerr << "Unknown place with id=" << i->first << endl;
	  }
	else if (j->first == "guard")
	  {
	    if ((t = PN->LTrans.FindMacao(i->first)) != NULL)
	      t->setGuard(j->second);
	    else
	      cerr << "Unknown transition with id=" << i->first << endl;
	  }
	else if (j->first == "valuation")
	  {
	    if ((a = PN->LArc.FindMacao(i->first)) != NULL)
	      a->setValuation(j->second);
	    else
	      cerr << "Unknown arc with id=" << i->first << endl;
	  }
	else if (j->first == "name")
	  {
	    if ((p = PN->LPlace.FindMacao(i->first)) != NULL)
	      p->Name(j->second);
	    else if ((t = PN->LTrans.FindMacao(i->first)) != NULL)
	      t->Name(j->second);
	    else
	      cerr << "Unknown place or transition with id=" << i->first << endl;
	  }
	else if (j->first == "domain")
	  {
	    if ((p = PN->LPlace.FindMacao(i->first)) != NULL)
	      {
		if ((dom =  PN->LDom.FindName(j->second)) != NULL)
		  {
		    p->Dom(dom);
		    // Apply delayed marks :
		    map<Place*, string>::iterator m_i = delayedMarks.find(p);
		    if (m_i != delayedMarks.end())
		      {
			p->setMarking(m_i->second);
			delayedMarks.erase(m_i);
		      }
		  }
		else
		  cerr << "Unknown domain " << j->second << " specification for " << *p << endl;
	      }
	    else
	      cerr << "Unknown place with id=" << i->first << endl;
	  }
	else if (j->first == "priority")
	  {
	    if ((t = PN->LTrans.FindMacao(i->first)) != NULL)
	      {
		int prio;
		sscanf(j->second.c_str(),"%d",&prio);
		t->Priority(prio);
	      }
	    else
	      cerr << "Unknown transition with id=" << i->first << endl;	  
	  }
	else if (j->first == "valuation")
	  {
	    if ((a = PN->LArc.FindMacao(i->first)) != NULL)
	      a->setValuation(j->second);
	    else
	      cerr << "Unknown arc with id=" << i->first << endl;
	  }
	else if (j->first == "version")
	  PN->Version(j->second);
	else if (j->first == "author(s)")
	  PN->Author(j->second); 
	else if (j->first == "declaration")
	  ; // Nothing to do...
	else if (j->first == "comment")
	  ; // Nothing to do...
	else
	  cerr << "Unknown CM command: CM(" << j->first << ",...)" << endl;
      }
}

void ParserCAMI::TraiterMarks()
{
  // Création du domaine "null" s'il n'existe pas...
  string null_domain_str = "null";
  list<Domain>& domains = PN->LDom.Lst();
  Domain* null_domain = NULL;
  for (list<Domain>::iterator i = domains.begin();
       i != domains.end();
       ++i)
    if (i->Name() == null_domain_str)
      {
	null_domain = &(*i);
	break;
      }
  if (null_domain == NULL)
    {
      Domain null_domain_temp(null_domain_str);
      null_domain = PN->LDom.Insert(null_domain_temp);
    }
  // Mise à jour des places marquées...
  for (map<Place*, string>::iterator i = delayedMarks.begin();
       i != delayedMarks.end();
       ++i)
    {
      if (i->first->Dom() == NULL)
	i->first->Dom(null_domain);
      i->first->setMarking(i->second);
    }
  delayedMarks.clear();
}

void ParserCAMI::InterfaceTraiterCM(int paramc, char* paramv[])
{
  long num;
  sscanf(paramv[2],"%ld",&num);
  string& s = cms[num][paramv[1]];
  s += " ";
  s += paramv[5];
}


/* creation d'une nouvelle place ou transition a la reception d'une commande CAMI CN */
void ParserCAMI::InterfaceTraiterCN(int paramc, char *paramv[])
{

  long num;

  sscanf(paramv[2],"%ld",&num);
 
  if ( ! strcmp(paramv[1],"transition") ) {
    Transition t (nextNumTrans++,num);
    PN->LTrans.Insert(t);
  } else  if (! strcmp(paramv[1],"place") ) {
    Place p (nextNumPlace++,num);
    PN->LPlace.Insert(p);
  } else  if (! strcmp(paramv[1],"immediate transition") ) {
    Transition t (nextNumTrans++,num,""/*,NULL*/,1);
    PN->LTrans.Insert(t);
  } else if ( ! strcmp(paramv[1],"net") ) {
    // This declares the net itself which we have already created implicitly
    // skip this declaration
  } else   {
    cerr << "Unknown CN command: CN(" ;
    for (int i=1;i<paramc;i++) {
      cerr <<  paramv[i] ; 
      i<paramc-1?cerr << ",":cerr << ")" <<endl;
    }
  }
}

/* creation d'un nouvel arc a la reception d'une commande CAMI CA */
void ParserCAMI::InterfaceTraiterCA(int paramc, char *paramv[])
{

  long num,numori,numdest;
  bool isInhibitor = false ;
  
  sscanf(paramv[2],"%ld",&num);
  sscanf(paramv[3],"%ld",&numori);
  sscanf(paramv[4],"%ld",&numdest);
  
  
  if (! strcmp(paramv[1],"arc") || ( ! strcmp(paramv[1],"inhibitor arc")  && (isInhibitor = true ) ) ) {
    Transition *t = PN->LTrans.FindMacao(numori);
    Place *p = PN->LPlace.FindMacao(numori);
    Direction dir;
    if (!t) { 
      t = PN->LTrans.FindMacao(numdest); 
      dir = Place2Trans;
    } else {
      p = PN->LPlace.FindMacao(numdest);
      dir = Trans2Place;
    }
 
    Arc a (nextNumArc++,p,t,dir,"1",-1,isInhibitor);
    a.NumeroMacao(num);
    Arc *pa=PN->LArc.Insert(a);
    p->AddArc(pa);
    t->AddArc(pa);

  } else {
    cerr << "Unknown CA command: CA(" ;
    for (int i=1;i<paramc;i++) {
      cerr <<  paramv[i] ; 
      i<paramc-1?cerr << ",":cerr << ")" <<endl;
    }
  }

}


void ParserCAMI::InterfaceTraiterCT(int paramc, char *paramv[]) {

  long num;

  Place *p = NULL;
  Transition *t = NULL;
  Arc *a=NULL;

  sscanf(paramv[2],"%ld",&num);

  /* load correct object refs */
  if ( ! (p = PN->LPlace.FindMacao(num)) )
    if ( ! (t = PN->LTrans.FindMacao(num)) )
      a = PN->LArc.FindMacao(num);

 
  /* different cases */ 
  /* NAME */
  if (!strcmp(paramv[1],"name")) { 
    if (p!=NULL)
      p->Name(paramv[3]);	
    else if (t!=NULL)
      t->Name(paramv[3]);
 
    /* DOMAIN */
  } else if ((!strcmp(paramv[1],"domain")) && p) {
    Domain *pDom =  PN->LDom.FindName(paramv[3]); 
    if (!pDom) {
      cerr << "Unknown domain " << paramv[3] << " specification for " << *p << endl ;
    }
    p->Dom(pDom);

    /* slight problem with marks tag encountered before domain tag */
    map<Place*, string>::iterator m_i = delayedMarks.find(p);
    if (m_i != delayedMarks.end())
      {
	p->setMarking(m_i->second);
	delayedMarks.erase(m_i);       
      }
    /*
      if ( ! delayedMarks.empty() ) {
      for (it = delayedMarks.begin() ; it != delayedMarks.end() ; it++) {
      if ( it->first == p ) {
      p->setMarking( it->second );
      delayedMarks.erase(it);
      break;
      }
      }
      }
    */
    /* GUARD */
  } else if ((!strcmp(paramv[1],"guard")) && t) {
    t->setGuard(paramv[3]);
   
    /* PRIORITY */
  } else if ((!strcmp(paramv[1],"priority")) && t) {
    int p;
    sscanf(paramv[3],"%d",&p);
    t->Priority(p);

    /* MARKING */
  } else if ((!strcmp(paramv[1],"marking")) && p) {
    if (p->Dom())
      p->setMarking(paramv[3]);
    else
      delayedMarks[p] = paramv[3];
    //     delayedMarks.push_back(make_pair(p,paramv[3]));
    /* VALUATION */
  } else if ((!strcmp(paramv[1],"valuation")) && a) {
    a->setValuation(paramv[3]);

    /* Attributes of Net */
  } else if (num == 1) {
    /* AUTHOR(S) */
    if ( !strcmp(paramv[1],"author(s)") ) {
      PN->Author(paramv[3]); 

      /* VERSION */
    } else if ( !strcmp(paramv[1],"version") ) {
      PN->Version(paramv[3]); 
    }
    /* DEFAULT => ERROR */
  } else if (!strcmp(paramv[1],"comment")) {
    /* Do nothing for comments... */
  } else {
    cerr << "Unknown CT command or problem with objects referenced : CT(" ;
    for (int i=1;i<paramc;i++) {
      cerr <<  paramv[i] ; 
      i<paramc-1?cerr << ",":cerr << ")" <<endl;
    }
  }

}
void ParserCAMI::InterfaceTraiterPO(int paramc, char *paramv[]) {
  int num = atoi (paramv[1]);
  int x = atoi (paramv[2]);
  int y = atoi (paramv[3]);
  
  traiterPosition("PO","",num,x,y,paramv[0]);
}

void ParserCAMI::InterfaceTraiterPT(int paramc, char *paramv[]) {
  int num = atoi (paramv[1]);
  int x = atoi (paramv[3]);
  int y = atoi (paramv[4]);

  traiterPosition ("PT",paramv[2],num,x,y,paramv[0]);
}

void ParserCAMI::traiterPosition (const string &cmd,const string &tag,int num,int x,int y, const string & line) {
     Place *p = NULL;
     Transition *t = NULL;
     Arc * a = NULL;


     /* load correct object refs */
     if ( ! (p = PN->LPlace.FindMacao(num)) )
       if ( ! (t = PN->LTrans.FindMacao(num)) ) 
	 if (! (a = PN->LArc.FindMacao(num)) ) 
	 {
	 cerr << " Error while parsing position information, unknown node " << num << " ; skipping line " << endl <<  line << endl;
	 return;
	 }

     if (num == 1) {
       cerr << " Skipping declaration position for node 1 ..." << line << endl;
       return;
     }

     if (cmd == "PO") {
       p?p->setPos(Position(x,y)):t->setPos(Position(x,y));
     } else if (cmd == "PT") {
       if (tag == "name") {
	 if (p)  p->namepos = Position(x,y); 
	 else t->namepos = Position(x,y) ;
       } else if (tag == "guard") {
	 if (t) t->namepos = Position(x,y); 
	 else cerr<< " Error while parsing guard tag position, object "<<num<<" is not a transition ; skipping line " << endl <<  line << endl;
       } else if (tag == "domain") {
	 if (p) p->dompos = Position(x,y); 
	 else cerr<< " Error while parsing domain tag position, object "<<num<<" is not a place ; skipping line " << endl <<  line << endl;
       }else if (tag == "marking") {
	 if (p) p->markpos = Position(x,y); 
	 else cerr<< " Error while parsing domain tag position, object "<<num<<" is not a place ; skipping line " << endl <<  line << endl;
       } else if (tag == "valuation") {
	 if (a) a->valpos = Position(x,y); 
	 else cerr<< " Error while parsing valuation tag position, object "<<num<<" is not an arc ; skipping line " << endl <<  line << endl;
       } else {
	 cerr<< " Error while parsing tag position, unknown tag "<< tag << "  ; skipping line " << endl <<  line << endl;
       }
     }


}
