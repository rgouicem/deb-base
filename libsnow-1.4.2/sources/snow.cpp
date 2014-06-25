#include <PNet.h>
#include <PNetIO.h>
#include <LineParser.h>
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

void usage() {
  cerr << "This tool computes static subclasses for GreatSpn. Input is a CAMI model file. Options :" << endl;
  cerr<<  "    -m path : specifies the path to cami model " <<endl;
  cerr<<  "    -p path : specifies the path to properties file" <<endl;
  cerr<<  "    -d directory : specifies the directory in which to generate .def .net files (default \"./FK_MNGR\")" <<endl;
  cerr<<  "    -f filter: specifies the transitions to parse in the properties file \n";
  cerr<<  "               filter is a list of comma separated atomic property names" << endl;
  cerr<<  "    -show   : exports Cami model for viewing (non standalone mode)" <<endl; 
  cerr<<  "    -nodynamic : force to use only static subclasses in expression of initial marking (default is to use compute and use dynamic subclasses)"<<endl;
  cerr<<  "    -v : verbose mode outputs debug model before and after for control" <<endl;
  cerr<<  "    --help,-h : display this (very helpful) helping help text"<<endl;
  cerr<<  "Problems ? Comments ? contact Yann.Thierry-Mieg@lip6.fr " <<endl;
}

int main (int argc, char* argv[]) {
  string pathcamiff;
  string pathltlff;
  string pathtobsff;
  bool doshow= false;
  bool doverbose= false;
  bool doDynamicSS=true;
  string dirgspn="./";
  for (int i=1;i < argc; i++) {
    if ( ! strcmp(argv[i],"-m") ) {
      if (++i > argc) {
        cerr << "give argument value please after " << argv[i-1] << endl;
        usage();
        exit (1);
      }
      pathcamiff = argv[i];
      dirgspn="./";
    } else if (! strcmp(argv[i],"-p") ) {
      if (++i > argc) {
        cerr << "give argument value please after " << argv[i-1] << endl;
        usage();
        exit (1);
      }
      pathtobsff = argv[i]; 
    } else if (! strcmp(argv[i],"-f") ) {
      if (++i > argc) {
        cerr << "give argument value please after " << argv[i-1] << endl;
        usage();
        exit (1);
      }
      char delim[2] ;
      strcpy(delim,","); 
      set<string> s;
      char * name;
      name = strtok(argv[i],delim);
      s.insert(name);
      while ( (name = strtok(NULL,delim)) )
        s.insert(name);
      SetTobsFilter(s);
    } else if (! strcmp(argv[i],"-show") ) {
      doshow= true;
    } else if (! strcmp(argv[i],"-nodynamic") ) {
      doDynamicSS=false;
    } else if (! strcmp(argv[i],"-v") ) {
      doverbose= true;
    } else if (! strcmp(argv[i],"--help") || ! strcmp(argv[i],"-h")  ) {
      usage(); exit(0);
    } else if (! strcmp(argv[i],"-d") ) {
      if (++i > argc) {
        cerr << "give argument value please after " << argv[i-1] << endl;
        usage();
        exit (1);
      }
      dirgspn = argv[i]; 
    } else {
      cerr << "Error : incorrect Argument : " << argv[i] << endl;
      usage();
      exit(1);
    }
  }

  PNet * PN = parseCAMI (pathcamiff);
  LoadCAMIAestetic(PN);
  if (pathtobsff != "")
    ParseTobs (pathtobsff,PN);
  if (doverbose)
    cout << *PN;
  PN->GenerateStaticSub (doDynamicSS) ;
  if (doverbose)
    cout << *PN;
  PN->ExportToGSPN(dirgspn);
}
