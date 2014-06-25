#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "PNetIO.h"
#include "LineParser.h"
#include "ParserCAMI.h"
#include "parsedef.h"
#include "parsenet.h"
#include "ParserWrapper.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef FRAMEKIT_SUPPORT
#include <FKCLSend.h>
#include <FKModel.h>
#endif

/* internal function prototypes */
/* loads a sequence of : strlen string strlen string .... into a string vector */
static int read_str (vector<string>& v,istream& is) ;



int  ParseTobs (const string &path, PNet *pPN) {
  return ParserWrapper::ParseTobs(path,pPN);
}

int SetTobsFilter (const set<string> & s) {
  return ParserWrapper::SetTobsFilter(s);
}

// Load data structure from an open filestream in Matrix format .rdp_matrix, return pointer to PNet object
extern PNet * parseMatrix (ifstream &mff,PNet *PN){
  if (!PN) {
    PN = new PNet();
  }
  
  // ************ Start actual parse of matrix file
  int NbPlaces,NbTrans,NbClasses,NbVar;

  // get number of places, trans, classes and vars
  mff >> NbPlaces >> NbTrans >> NbClasses >> NbVar;
  // get macao numbers of the objects
  vector<int> MacaoPlace(NbPlaces,0);
  vector<int> MacaoTrans(NbTrans,0);
  for (unsigned int i=0;i<MacaoPlace.size() && mff>>MacaoPlace[i];i++) ;
  for (unsigned int i=0;i<MacaoTrans.size() && mff>>MacaoTrans[i];i++) ;
  
  // get object names
  vector<string> PlaceName(NbPlaces);
  vector<string> TransName(NbTrans);
  vector<string> ClassName(NbClasses);
  vector<string> VarName(NbVar);
  read_str (PlaceName,mff);
  read_str (TransName,mff);
  read_str (ClassName,mff);
  read_str (VarName,mff);
  
  
  for (int i=0;i<NbPlaces;i++) {
    Place p (i,MacaoPlace[i],PlaceName[i]);
    PN->LPlace.Insert(p);
  }
  for (int i=0;i<NbTrans;i++) {
    Transition t (i,MacaoTrans[i],TransName[i]);
    PN->LTrans.Insert(t);
  }
  
  vector<classType> ClassType(NbClasses);

  
  /** Begin parse of class description line */
  {
    {
      char buff[8];
      // clear remaining \n
      mff.getline(buff,8,'\n');
    }
    // load class description line into buff
    LineParser l(mff);
    int i;
    for (i=0;i<NbClasses;) {
      switch (l.parse()) 
	{
	case '1': {
	  ClassType[i] = Integer;
	  l.parse(':');
	  l.parse('(');
	  l.parse(')');
	  PNClass c(i,ClassType[i],ClassName[i]);
	  PN->LClasse.Insert(c);
	  i++;
	  break;
	} 
	case '2': {
	  ClassType[i] = Char;
	  l.parse(':');
	  l.parse('(');
	  l.parse(')');
	  PNClass c(i,ClassType[i],ClassName[i]);
	  PN->LClasse.Insert(c);
	  i++;
	  break;
	}
	case '3': {
	  ClassType[i] = Parametre;
	  string sname;
	  l.parse(':');
	  l.parse('(');
	  l.parse('|');
	  l.setDelim("| \t");
	  l.parse(sname);
	  if (sname != ClassName[i]) {
	    string err("expected same name as class name ");
	    err+= ClassName[i];
	    err+=" in PARAMETRE class description";
	    l.parseError(err);
	  }
	  l.parse('|');
	  l.parse(')');
	  
	  PNClass c(i,ClassType[i],ClassName[i]);
	  PN->LClasse.Insert(c);
	  i++;
	  break;
	}
	  
	case '4': {
	  ClassType[i] = Intervalle;
	  l.parse(':');
	  l.parse('(');
	  
	  string t1,t2,t3;
	  l.setDelim(" \t)");
	  if ( !l.parse(t1) || !l.parse(t2) )
	    l.parseError("string token");
	  if ( l.parse(t3) ) {
	    // this means interval has a prefix specified, t2 and t3 are min and max of interval 
	    PNClass c(i,ClassName[i],toint(t2),toint(t3),t1);
	    PN->LClasse.Insert(c);
	  } else {
	    // no prefix, t1 and t2 are min and max of interval 
	    PNClass c(i,ClassName[i],toint(t1),toint(t2));
	    PN->LClasse.Insert(c);
	  }
	  l.parse(')');
	  i++;
	  break;	
	}
	case '5': {
	  ClassType[i] = Enumere;
	  l.parse(':');
	  l.parse('(');
	  PNClass c(i,ClassType[i],ClassName[i]);
	  string tok;
	  l.setDelim(" \t)");
	  for (int num=0;l.parse(tok);num++) {
	    Element elt(num,tok);
	    c.AddElt (elt);
	  }
	  l.parse(')');
	  PN->LClasse.Insert(c);
	  i++;
	  break;
	}
	case '6': {
	  ClassType[i] = Statique;
	  l.parse(':');
	  l.parse('(');
	  string t;
	  int parentId;
	  l.setDelim(" \t)");
	  l.parse(t); 
	  parentId=toint(t);
	  PNClass* pClass;  
	  // parentId-1 because my indexes start from 0 whereas matrix numbers elements from 1 on
	  pClass=PN->LClasse.Find(parentId-1); 
	  if (!pClass) {
	    cerr << "Incorrect parent class id for static subclass "<<ClassName[i]<<"!\n";
	    l.parseError("a valid class number");
	  }
	  PNClass c(i,ClassName[i],pClass);
	  string tok;
	  for (int num=0;l.parse(tok);num++) {
	    if (c.AddEltToStatic (tok)==-1) l.parseError("element of parent class");
	  }
	  l.parse(')');
	  PN->LClasse.Insert(c);
	  i++;
	  break;
	}
	default:
	  l.parseError("a valid class type (1 to 6) identifier");
	  break;
	}
    }
  } // end parse of class descriptions
  
    /** Parse Variable domain line */
  { 
    // load class description line into l
    LineParser l(mff);
    int i;
    for (i=0;i<NbVar;i++) {
      int n;
      l.setDelim(" \t");
      l.parse(n);
	Variable v(i,VarName[i],PN->LClasse.Find(n-1)); // n-1 because my index start from 0
	PN->LVar.Insert(v);
    }
  } // end parse variable domain description line
  
    /** Parse Arc lines  */
  // used to assign unique ids to arcs
  int numArc=0,numArcForMacao=0;
  
  // parse both pre and post matrix, same format...
  for (int cnt = 0;cnt <2;cnt++)
  {  
    int i;
    for (i=0;i<NbPlaces && !mff.eof();i++) {
      LineParser l(mff);
      
      // Parse place domain
      l.parse('[');
      int n;
      l.setDelim(" \t]");
      l.parse(n);
      Domain d("");
      for (int ii=0;ii < n;ii++) {
	int id;
	if ( l.parse(id) ) {
	  PNClass * p= PN->LClasse.Find(id-1);
	  if (p)
	    d.AddElt(p);
	  else
	    l.parseError("Id of a class as element of domain");
	} 
      }
      Domain *pD = PN->LDom.Insert(d);
      PN->LPlace.Find(i)->Dom(pD);
      l.parse(']');

      // Parse 
      int j;
      for (j=0;j<NbTrans;j++) {
	string val;
	l.parse('[');
	if (l.parse(val,']') && val != "<>") {
	  if (cnt==0) PN->AddArc(i,j,Place2Trans,numArc++,val);
	  else PN->AddArc(i,j,Trans2Place,numArc++,val);
	}
	l.parse(']');
      }
    }
    /******** Parsed arc descriptions */
    /* now parse Macao numbers ... */
    for (i=0;i<NbPlaces;i++) {
      LineParser l(mff);
      l.parse('!');
      int j;
      l.setDelim(" \t!");
      for (j=0;j<NbTrans;j++) {
	string tok;
	int nb;
	l.parse(tok);
	nb=toint(tok);
	if (nb == -1) {
	  l.parseError("Int value for macao number of arc");
	}
	if ( nb != 0) { 
	  (PN->LArc.Find(numArcForMacao++))->NumeroMacao(nb);
	}

      }
      l.parse('!');      
    }

  } /** end parse arc post-matrix lines */
  
  /** Parse Transition guards */
  {
    int j;
    for (j=0;j<NbTrans;j++) {
      LineParser l(mff);
      l.setDelim(" \t}");
      l.parse('{');
      
      string tok,guard;;
      /* test if there is a guard */
      if ( l.parse(tok) ) {
	int n,nn;
	/* Parse number of variables in predicate*/
	if ((n=toint(tok))!=-1) {
	  /* consume the listing of the domain of these variables, no treatment/effect on model */
	  /* This information is false in current matrix version as of today 13/02/2002 */
	  for (int i=0;i<n;i++) {
	    l.parse(nn);
	  }
	  /* get the actual predicate in raw form */
	  l.parse(guard);
	} else {
	  guard = tok;
	}
	(PN->LTrans.Find(j))->setGuard(guard);
      }
      l.parse('}');
    }
  } /* end parse guards */

  /* parse marking */
  {
    LineParser l(mff);
    int i;
    string tok;
    for (i=0;i<NbPlaces;i++) {
      l.parse('[');
      l.parse(tok,']');
      if ( tok !="<>" )
	   (PN->LPlace.Find(i))->setMarking(tok);
      l.parse(']');
    }
  }
  /* end parse marking */
  return PN;
}





// Load data structure from Matrix file .rdp_matrix, return pointer to PNet object
extern PNet * parseMatrix (const char *path , PNet *PN ) {
  ifstream matrixff(path);
  if (!matrixff) {
    cerr << "\nCould not open file " <<  path<<"in parseMatrix function!Please check if the file exists and permissions are set to readable\n"<< endl;
  }
  return parseMatrix(matrixff,PN);
}


/* loads a sequence of : strlen string strlen string .... into a string vector */
int read_str (vector<string>& v,istream& is=cin) {
  int size;
  unsigned int i;
  for (i=0; i<v.size(); i++) {
    is>>size;
    if (size) is>>v[i];
    else v[i]="";
//    cout << v[i] << endl;
  }
  return i;
}



/* format a string for cami output: %d:%s as in strlen(s):s  i.e : 11:declaration */
string CamiFormat (const string &s) { 
  char buff[s.length()+4];
  sprintf(buff,"%zu:%s",s.length(),s.c_str());
  return buff;
}

string CamiFormat (int i) { 
  char buff[16];
  sprintf(buff,"%d",i);
  return CamiFormat(buff);
}




void LoadCAMIAestetic(PNet *PN ){

  FILE * aesteticFF ;
  char line[256] ;
  
  memset(line,0,256);

#ifdef FRAMEKIT_SUPPORT
   if ( (aesteticFF = fopen(FkAMINetGetAesteticFileName(),"r")) == NULL ) {
#else
   if ( (aesteticFF = fopen("FK_MNGR/.aestetic","r")) == NULL ) {
#endif
    perror ("Error attempting to access aestetic model info file. \n Skipping load old aestetic values phase.");
    return;
   }



  while ( (fgets(line,256,aesteticFF) != NULL) ) {
    LineParser l(line);
    string cmd,tag;
    int num,x,y;
    l.setDelim("(,)");
    l.parse(cmd);
    if (cmd == "PO") {
      l.parse('(');
      l.parse(num);
      l.parse(',');
      l.parse(x);
      l.parse(',');
      l.parse(y);
      l.parse(')');
    } else if (cmd == "PT") {
      l.parse('(');
      l.parse(num);
      l.parse(',');
      l.parseCami(tag);
      l.parse(',');
      l.parse(x);
      l.parse(',');
      l.parse(y);
      l.parse(')');
    } else {
      cerr << "Unknown cami position command \'" << cmd << "\' while parsing aestetic description file" <<endl;
      cerr << "Skipping line " << endl <<  line << endl;
      continue;
    }

    ParserCAMI::traiterPosition (cmd,tag,num,x,y,line);

  }


  fclose (aesteticFF);
}
  

// Load data structure from an open filestream in CAMI format .model, return pointer to PNet object
extern PNet * parseCAMI (const string & path){
  PNet *PN = new PNet();
  ParserCAMI p (PN,path);
//  LoadCAMIAestetic(PN);
  PN->LPlace.renamePlaces();
  PN->LTrans.renameTrans();
  return PN;
}

extern PNet * parseCAMI (PNet *PN ){

  if (!PN) {
    PN = new PNet();
  }
  ParserCAMI p (PN);
  LoadCAMIAestetic(PN);
  PN->LPlace.renamePlaces(); 
  PN->LTrans.renameTrans();
  return PN;


}

extern PNet * parseGSPN (const string & modelname, PNet * PN) {


  if (!PN) {
    PN = new PNet();
  }
  DefFile d (modelname+".def",PN);

 // cerr << d;
 // cerr << *PN ;
  
  parsenet::parsenet (modelname+".net",&d,PN);

  return PN;


}



/*-----------------------------------------------------------------------------------*/
/* lit les anciennes valeurs aestetic et les redeverse en bloc dans le modele en creation */
/*  !! pas de controle d' erreur si objets non definis etc... !! Changer pour parser des attributs de pos sur les objets */
extern void ReLoadOldAesteticValues (ostream &os)
{
  FILE * aesteticFF ;
  char line[256] ;
  
  memset(line,0,256);
 
#ifdef FRAMEKIT_SUPPORT
   if ( (aesteticFF = fopen(FkAMINetGetAesteticFileName(),"r")) == NULL ) {
#else
   if ( (aesteticFF = fopen("FK_MNGR/.aestetic","r")) == NULL ) {
#endif
    perror ("Error attempting to access aestetic model info file. \n Skipping load old aestetic values phase.");
    return;
  } else {
    while ( (fgets(line,256,aesteticFF) != NULL) ) {
      os << line;
    } 
  }
  fclose (aesteticFF);
  
}
