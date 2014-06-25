#ifndef __PARSE_DEF_H__
#define __PARSE_DEF_H__

#include <utility>
#include <vector> 
#include <string> 
#include <iostream>
#include "Position.h"
using namespace std; 


enum DeclType {
  marking,
  class,
  function
};

class Declaration {
public : 
  DeclType type;
  string name;
  string decl;
  Position pos;
  int defline;

  Declaration (DeclType ttype,const string & nname, const string & ddecl, const Position &ppos, int ddefline) : type(ttype),name(nname),decl(ddecl),pos(ppos),defline(ddefline) {};
};

class DefFile {
  vector<Declaration> decls;
 public:
  DefFile (const char * path);


};


#define MAXSTRING 1024




#endif
