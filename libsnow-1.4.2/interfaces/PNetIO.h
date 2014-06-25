#ifndef _PNETIO_H_
#define _PNETIO_H_

#include "PNet.h"
#include <string>
#include <iostream>
#include <fstream>
#include <set>
using namespace std;

// Load data structure from an open filestream in Matrix format .rdp_matrix, return pointer to PNet object
extern PNet * parseMatrix (ifstream &mff,PNet *PN = NULL);

// Load data structure from Matrix file .rdp_matrix, return pointer to PNet object
extern PNet * parseMatrix (const char *path = "./FK_MNGR/.rdp_matrix", PNet *PN = NULL);

// Load data structure from an open filestream in CAMI format .dec_model, return pointer to PNet object
extern PNet * parseCAMI (PNet *PN = NULL);
extern PNet * parseCAMI (const string &path);
// Load data structure from gspn .net .def format
extern PNet * parseGSPN (const string & modelname,PNet *PN = NULL);

// parse a tobs description file (textual format)
extern int  ParseTobs (const string &path, PNet *pPN=NULL) ;
// Set a filter for tobs file parsing
extern int SetTobsFilter (const set<string> & s);

// Formats a string(resp. an int) for cami output: %d:%s as in strlen(s):s  i.e : 11:declaration */ 
extern string CamiFormat (const string &s);
extern string CamiFormat (int i);



/* lit les anciennes valeurs aestetic et les redeverse en bloc dans le modele en creation */
/*  !! pas de controle d' erreur si objets non definis etc... !! */
extern void ReLoadOldAesteticValues (ostream &os);
extern void LoadCAMIAestetic(PNet *PN );


/******* Some primitive Function Objects used in calls to find *************/
class FindMacao  {
protected:
  long macaoNum;
public:
  FindMacao (long n) { macaoNum = n; }
  bool operator()(Place &p) { return p.NumeroMacao() == macaoNum; }
  bool operator()(Transition &t) { return t.NumeroMacao() == macaoNum; }
  bool operator()(const Arc &a) { return const_cast<Arc*>(&a)->NumeroMacao() == macaoNum;}
};

class FindName  {
protected:
  string name;
public:
  FindName (const string &n) { name = n; }
  bool operator()(Place &p) { return p.Name() == name; }
  bool operator()(Transition &t) { return t.Name() == name; }
  bool operator()(PNClass &c) { return c.Name() == name; }
  bool operator()(Domain &d) { return d.Name() == name; }

};




#endif
