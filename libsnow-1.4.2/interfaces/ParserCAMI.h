#ifndef _PARSERCAMI_H_
#define _PARSERCAMI_H_
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "PNet.h"

/* For interface with Framekit environment */
class ParserCAMI {
protected :
  static PNet *PN;
  static int nextNumPlace; 
  static int nextNumTrans;
  static int nextNumArc;
  static map< long, map<string, string> > cms;
  static map<Place*, string> delayedMarks;
  
  static void TraiterCM();
  static void TraiterMarks();

public :
  ParserCAMI(PNet *PNN,const string &path ="");
  virtual  ~ParserCAMI(){};

  static void InterfaceTraiterCN(int paramc, char *paramv[]);
  static void InterfaceTraiterCT(int paramc, char *paramv[]);
  static void InterfaceTraiterCA(int paramc, char *paramv[]);
  static void InterfaceTraiterCM(int paramc, char *paramv[]);
  static void InterfaceTraiterFB(int paramc, char *paramv[]);
  static void InterfaceTraiterPO(int paramc, char *paramv[]);
  static void InterfaceTraiterPT(int paramc, char *paramv[]);

  static void traiterPosition (const string &cmd,const string &tag,int num,int x,int y,const string &line);

};
#endif
