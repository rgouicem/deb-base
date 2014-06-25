#ifndef __PARSE_DEF_H__
#define __PARSE_DEF_H__

#include <utility>
#include <vector> 
#include <string> 
#include <iostream>
#include "Position.h"
#include "PNet.h"
using namespace std; 



enum DeclType {
  marking,
  function,
  color
};

class Declaration {
public : 
  DeclType type;
  string name;
  string decl;
  Position pos;
  int defline;

  Declaration (DeclType ttype,const string & nname, const string & ddecl, const Position &ppos, int ddefline) : type(ttype),name(nname),decl(ddecl),pos(ppos),defline(ddefline) {};
  friend ostream & operator<<(ostream & os,const Declaration &d);
};

class DefFile {
public:
  vector<Declaration> decls; 

  DefFile (const string & path,PNet * pPN);
  DefFile() {};
  friend ostream & operator<< (ostream & os,const DefFile &d);
};


#define MAXSTRING 1024




#endif
