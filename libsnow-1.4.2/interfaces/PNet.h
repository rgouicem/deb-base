#ifndef _PNET_H_
#define _PNET_H_
#include <iostream>
#include <fstream>
 
#include "Places.h"
#include "Transitions.h"
#include "Arcs.h"
#include "Variables.h"
#include "PNClasses.h"
#include "Domains.h"

//Declaration header file for class PNet


class PNet {
public:
  Places LPlace;
  Transitions LTrans;
  Variables LVar;
  PNClasses LClasse;
  Domains LDom;

  Arcs LArc;
 protected:
  string author;
  string version;
  void FindOrderedClasses();

public:
  // *******************Constructor/destructor 
  //default constructor
  PNet () {};
  //constructor by copy
  // undefined use compiler generated

  //default destructor
  virtual ~PNet (){};

  // *******************basic operators
  // clone by operator=
  // undefined use compiler version

  // comparison by operator== (not used)
  // friend int operator==(const PNet&,const PNet&);

  //printing with operator<<
  friend ostream& operator<<(ostream&,const PNet&);

  // ****************** convenience accessors
  const int NbPlaces() const { return LPlace.size(); }
  const int NbTrans() const { return LTrans.size(); }
  const int NbClasses() const { return LClasse.size(); }
  const int NbVar() const { return LVar.size(); }
  // Accessors (basi)

  const string Author() const {return author;}
  void Author(const string& s) {author = s;}

  const string Version() const {return version;}
  void Version(const string& s) {version = s;}

  // ***********************Advanced methods
  // Load data structure from an open filestream in Matrix format .rdp_matrix, return pointer to PNet object
  friend PNet * parseMatrix (ifstream&,PNet *);
  // Load data structure from Matrix file .rdp_matrix, return pointer to PNet object
  friend PNet * parseMatrix (const char *, PNet *);
  friend class ParserCAMI;


  // Create unique id and Macao Object number 
  void reIndex () ;
  // Export data structure to CAMI FORMAT in file specified by path
  int ExportToCami (const string &path="FK_MNGR/.dec_model2");
  int ExportToCami (ostream& strean);

  /* Export to .def and .net files to dir in model.net and model.def files*/
  int ExportToGSPN (const string &dir="FK_MNGR/");

  // Export to Smart format in file specified by path. Note that this only works for GSPN (uncolored nets)
  int ExportToSmart (const string &path="model.sm");

  // Export to PNDDD format in file specified by path. Note that this only works for GSPN (uncolored nets)
  int ExportToPnddd (const string &path="model.pd3");

  /***************  ONLY AVAILABLE WITH libPNet_FrameKit.a *********/
  // Export SRG to MAcao (this is broken )
  int ExportGMS ();
  // Export data structure to Macao platform based on ExportToCami
  int ExportToMacao ();
  /*********************END FRAMEKIT_SUPPORT *************************/

  // calculate static subclass info
  void GenerateStaticSub (bool doDynamicSS=true);

  // Adding an arc to the model
  Arc* AddArc (int placeId,int transId,Direction dir=Trans2Place,int id=-1,const string &valuation="") ;

  
};
#endif

