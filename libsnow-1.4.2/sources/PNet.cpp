#include "PNet.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include "PNetIO.h"
#include "CalcSubClasses.h"
#include <unistd.h>

#ifdef FRAMEKIT_SUPPORT
// INCLUDES FOR MACAO
#include	<FKCLSend.h>
#include	<FKCLDecode.h>
#include	<FKModel.h>
#include	<FKResults.h>
#endif 

//Implementation for class PNet



// *******************basic operators




// printing with operator<<
ostream& operator<<(ostream& os,const PNet& p){
  os << p.LPlace << p.LTrans << p.LClasse << p.LVar << p.LDom << p.LArc <<endl;  
  return os;
}

// Adding an arc to the model
Arc * PNet::AddArc (int placeId,int transId,Direction dir,int id,const string &valuation) {
  Place *p=LPlace.Find(placeId);
  Transition *t=LTrans.Find(transId);
  Arc a = Arc (id,p,t,dir,valuation);
  Arc *pa= LArc.Insert(a);
  p->AddArc(pa);
  t->AddArc(pa);
  return pa;
}

void PNet::FindOrderedClasses () {

  LArc.FindOrderedClasses();
  LTrans.FindOrderedClasses();
}

// calculate static subclass info
void PNet::GenerateStaticSub (bool doDynamicSS) {
  
  list<Variable *> lvar ;
  list<Variable *>::iterator varit;
  list<PNClass>::iterator clit;
  list<PNClass> &lclass = LClasse.Lst() ;
  int i;
  int max = lclass.size();

  FindOrderedClasses();
  
  for (clit = lclass.begin(),i=0 ; clit != lclass.end() && i < max ; clit ++,i++) {
    list<vector<Element> > subclasses,subcforvar,subcforplace,subcforarc ;
    if ( clit->Type() == Statique ||clit->Type() == Dynamique ) continue ;
    lvar = LVar.Find(&(*clit)) ;
    // transitions assymetry
    for (varit = lvar.begin() ; varit != lvar.end() ; varit++ ) {
      subcforvar = LTrans.getAssymetry(*varit) ;
      subclasses.insert(subclasses.end(),subcforvar.begin(),subcforvar.end());
      subclasses  = calcSub::uniquePartition (subclasses);
      
      /* break if maximum separation reached */
      if (subclasses.size() == (*varit)->PClass()->Elts().size()) {
	break; }
    }
    
    // arcs assymetry
    subcforarc = LArc.getAssymetry(&(*clit)) ;
    subclasses.insert(subclasses.end(),subcforarc.begin(),subcforarc.end());
    subclasses  = calcSub::uniquePartition (subclasses);
     cerr << "subcstatic" ; calcSub::print(cerr,subclasses);
    // construct static subclasses
    LClasse.MakeStatic (subclasses,&(*clit));
 
    // places
    subcforplace = LPlace.getAssymetry(&(*clit)) ;
       cerr << "subcforplace" ; calcSub::print(cerr,subcforplace);
    subclasses.insert(subclasses.end(),subcforplace.begin(),subcforplace.end());
    subclasses  = calcSub::uniquePartition (subclasses);
     cerr << "subc unique" ; calcSub::print(cerr,subclasses);

     if (doDynamicSS) {
    // subclasses is now a partition of the partition into static subs : it gives the separation into dynamic subs
     LClasse.MakeDynamic (subclasses,&(*clit));
     } else {
       // no dynamic subclasses ver 
       LClasse.MakeStatic (subclasses,&(*clit));
     }
  }
  if (doDynamicSS) {
    LPlace.RewriteWithDynamicClasses () ; 
  } else {
    LPlace.RewriteWithStaticClasses () ; 
  }

  LTrans.RewriteWithStaticClasses();
  LArc.RewriteWithStaticClasses();
}

// Export data structure to CAMI FORMAT in file specified by path
int PNet::ExportToCami (const string &path) {
  ofstream ff(path.c_str());
  /*
  ff << "DB()\n";
  ff << "CN(3:net,1)\n";
  if (author != "") 
    ff << "CT(9:author(s),1," << CamiFormat(author) << ")\n" ;
  if (version != "") 
    ff << "CT(7:version,1," << CamiFormat(version) << ")\n" ;
  
  int numl = 1;
  reIndex();
  LClasse.ExportToCami(ff,numl);
  LDom.ExportToCami(ff,numl);
  LVar.ExportToCami(ff,numl);
  LPlace.ExportToCami(ff);
  LTrans.ExportToCami(ff);
  LArc.ExportToCami(ff);
//  ReLoadOldAesteticValues(ff);
  ff << "FB()" << endl;
  ff.flush();
  ff.close();
  return (0);
  */
  int result = this->ExportToCami(ff);
  ff.close();
  return result;
}

int PNet::ExportToCami (ostream& stream) {
  stream << "DB()\n";
  stream << "CN(3:net,1)\n";
  if (author != "") 
    stream << "CT(9:author(s),1," << CamiFormat(author) << ")\n" ;
  if (version != "") 
    stream << "CT(7:version,1," << CamiFormat(version) << ")\n" ;
  
  int numl = 1;
  reIndex();
  LClasse.ExportToCami(stream,numl);
  LDom.ExportToCami(stream,numl);
  LVar.ExportToCami(stream,numl);
  LPlace.ExportToCami(stream);
  LTrans.ExportToCami(stream);
  LArc.ExportToCami(stream);
//  ReLoadOldAesteticValues(stream);
  stream << "FB()" << endl;
  stream.flush();
  return (0);
}

#ifdef FRAMEKIT_SUPPORT
int PNet::ExportGMS () {
  PFkModel			pFkModel;
  FkAttributeList	fkAttributeList;
//  char					message[kFk_CAMI_CommandSize];

  FkClInitDecodeCami();   
  FkSetModelTrace(kFk_Model_TraceError /*+ kFk_Model_TraceRoutineCall*/ + kFk_Model_TraceCAMI);
  pFkModel=FkNewModel("ReachabilityGraph");
  
  fkAttributeList=FkNewAttributeList();
  
  chdir("./FK_MNGR");
  execl("/home/thierry/GreatSPN/i686_R2.4.18-6mdk/WNSRG","model","-o",NULL);
  execl("/home/thierry/AMINET/GSPN2GMS/Linux/gspn2gms","model.srgC3",NULL);

  FkClSendFile("./FK_MNGR/model.gms",kFk_DoNotDeleteIt);  //  kFk_DeleteIt  not implemented
  
  FkSendModel(pFkModel);

  return 0;
}
#endif  


void PNet::reIndex () {
  int curMacaoId = 2;
  curMacaoId = LPlace.reIndex(curMacaoId);
  curMacaoId = LTrans.reIndex(curMacaoId);
  curMacaoId = LArc.reIndex(curMacaoId);
}

#ifdef FRAMEKIT_SUPPORT
/* Export into a model for the Macao interface */
int PNet::ExportToMacao () {
  PFkModel			pFkModel;
  FkAttributeList	fkAttributeList;
//  char					message[kFk_CAMI_CommandSize];

  FkClInitDecodeCami();   
  FkSetModelTrace(kFk_Model_TraceError /*+ kFk_Model_TraceRoutineCall*/ + kFk_Model_TraceCAMI);
  pFkModel=FkNewModel("AMI-Net");
  
  fkAttributeList=FkNewAttributeList();
  ExportToCami("./FK_MNGR/.dec_model2");
  

  
/*  while (ff.getline(message,kFk_CAMI_CommandSize)) {
     FkClSendChannelCami(NULL, message);
   }
*/

  FkClSendFile("./FK_MNGR/.dec_model2",kFk_DoNotDeleteIt);  //  kFk_DeleteIt  not implemented
  
  FkSendModel(pFkModel);

  return 0;
}
#endif



/* Export to .def and .net files */
int PNet::ExportToGSPN (const string &dir) {
  int numligne = 0;
  string pathnet = dir + "model.net" ;
  string pathdef = dir + "model.def" ;
  ofstream netff(pathnet.c_str());
  ofstream deff(pathdef.c_str());
  
  netff.precision(4);
  deff.precision(4);

  deff << "|256\n%\n|\n" ; // header for def files
    
  netff << "|0|\n|\nf   0   " ; // number of mrking parameters
  netff << LPlace.size() << "   ";
  netff << "0   "; // number of rate parameters
  netff << LTrans.size() << "   ";
  netff << "0   "; // number of groups
  netff << "0   "; // free 0 !!! gratos !! "&@`#  GSPN
  netff << "0" << endl ; // number of layers seems to be optional

  

  // export classes
  LClasse.ExportToGSPN(deff) ;

  numligne = LClasse.size() ;
  // export markings + places
  LPlace.ExportToGSPN(deff,netff,numligne);
  LTrans.renameTrans ();
  LTrans.ExportToGSPN(netff,deff,numligne);

  return 0;
}



// Export to Smart format in file specified by path. Note that this only works for GSPN (uncolored nets)
int PNet::ExportToSmart (const string &path ) {
  ofstream os (path.c_str());
  
  // header
  os << "//  Model generated by GSPN2Smart \n\n" ;
  os << "spn model := {" << endl ; 
  
  LPlace.ExportToSmart (os);
  LTrans.renameTrans ();
  LTrans.ExportToSmart (os);


  // to indicate we want state space size
  os << "    bigint m1 := num_states(false);" << endl ;
  // footer
  os << "};" << endl ; // close spn description 
  // set (best) options
  os << "# GarbageCollection LAZY \n";
  os << "# StateStorage MDD_SATURATION \n";
  // set maximum verbosity for performance measures
  os << "# Report true \n";
  os << "# Verbose true \n";
  // compute state space size and exit
  os << "\n\nmodel.m1; \n\n";
  return 0;
}


// Export to Smart format in file specified by path. Note that this only works for GSPN (uncolored nets)
int PNet::ExportToPnddd (const string &path ) {
  ofstream os (path.c_str());
  
  // header
  // None for pnddd
  
  LPlace.ExportToPnddd (os);
  LTrans.renameTrans ();
  LTrans.ExportToPnddd (os);

  return 0;
}

