#include "LineParser.h"
#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstdlib>
#include <cstring>

#ifdef FRAMEKIT_SUPPORT
#include <FKCLSend.h>
#include <FKModel.h>
#include <FKCLDecode.h>
#include <FKEnv.h>
#include <FKMain.h>
#include <FKResults.h>
#endif

/* returns parse int from string */
extern int toint (const string &s) {
  int i;
  if (sscanf(s.c_str(),"%d",&i) ==1)
    return i;
  else
    cerr << "\ncouldn't convert "<<s<<" to integer value !!\n"<<endl;
  return -1;
}

/* homogeneous error handling */
void LineParser::parseError (const string s ) {
  ostringstream ost ;
  
  ost << "\n Error parsing file; At line :\n" << b << "\n expected "<< s << " around character number "<<pos <<endl;  
  cerr << ost.str() ;
#ifdef FRAMEKIT_SUPPORT
  FkResultSendOneMessage(kFk_ResultError, ost.str().c_str() , 1 , "declaration" , 0 , 0 ) ;
  FkExit(kFk_EndOnError);
#else
  cerr << "Sorry error is fatal."<<endl;
  exit(1);
#endif
}

/* returns true if at EOL */
bool  LineParser::eol() {
  skipWhite();
  return (pos == len) ;
}

// constructor
LineParser::LineParser(ifstream &mff) {
  string s;
  getline(mff,s,'\n');
  b = (char *) malloc ( (s.length()+1)*sizeof(char) );
  strcpy(b,s.c_str());
//  mff.getline(b,2048,'\n');
  len = strlen(b);
  memset(delim,0,13);
  cp=b;
  pos=0;
  nbDelim =0;
}

// constructor
LineParser::LineParser(const string &s) {
  b = (char *) malloc ( (s.length()+1)*sizeof(char) );
  strcpy(b,s.c_str());
//  mff.getline(b,2048,'\n');
  len = strlen(b);
  memset(delim,0,13);
  cp=b;
  pos=0;
  nbDelim =0;
}

/* skips ' ' and '\t' returns number of chars skipped */
int LineParser::skipWhite() {
 /* skip whitespaces */
  int i;
  for (i=0;pos<len && ( *cp == ' ' || *cp == '\t');pos ++,cp++,i++);
  return i;
}

/* parses a Cami format string  into s , returns number of characters parsed ;
   i.e. 11:declaration will put "declaration" in s and return 11 
*/
int LineParser::parseCami (string &s) {
  int n;
  char saveDelim[12];
  strncpy(saveDelim,delim,11);
  int nnbDelim = nbDelim;
  setDelim(":");
  parse(n);
  parse(':');
  char buff[n+1];
  for (int i=0;i<n;i++) {
    buff[i] = *cp;
    cp++;pos++;
  }
  buff[n] = '\0';
  s = buff;
  strncpy(delim,saveDelim,11);
  nbDelim = nnbDelim;
  return n;
}

/* parses a token into s , returns number of characters parsed ;optionally delimiter can be specified*/
int LineParser::parse (string &s,char c) {
  int i;
  s= string ("");
  /* skip whitespaces */
  skipWhite();
  
  /* break if encountering a character in delim list' ''\t''('')''|' ']' '[' '!'o */
  int ok;
  if (!c) 
    for (i=0,ok=1;
	 pos<len ;
	   // old version had hard coded delimiters
	   // && ( *cp != ' ' && *cp != '\t' && *cp != '(' && *cp != ')' && *cp != '|' && *cp != ':' && *cp != '[' && *cp != ']' && *cp != '!');
	 pos++,cp++,i++) {
      for (int j=0;j<nbDelim;j++) 
	// new version uses delim[] 
	if (*cp == delim[j]) 
	  {
	    ok=0;
	    break;
	  }
      if (ok)
	s+= *cp;
      else break;
    }
  else 
    for (i=0;
	 pos<len &&  *cp != c;
	 pos++,cp++,i++)
      s+= *cp;
// Unsure whether 0 chars read is an error ... can be caught externally if such is desired behavior
// permits constructs like while(xx.parse(token)) { do something... }
//  if (i==0) 
//    parseError("string token");
  return i;
}


/* parses next non white char if c=='\0' and returns it
     else attempts to parse character c, returns 1 on success*/
char  LineParser::parse (char c) {
  skipWhite();
  if (pos >= len) {
    string messout("character \'");
    messout += c;
    messout += "\' but reached EOL";
    parseError (messout);
  }
  if (c=='\0') {
    c = *cp;
    cp++;pos++;
    return c;
  } else if (*cp != c) {
      string s("character ");
      s+=c;
      parseError(s);
  } else {
    cp++;
    pos++;
  }
  return 1;
}

/* moves the current position back one char <=> return a char to the parser
   !! Note that this operation is not reverse of parse(char c) since whitespace are not distinguished by
   !! ungetc()
   returns 1 on success, 0 if pos was already 0
*/
int LineParser::ungetc() { 
  if (pos >0) {
    pos--;cp--;
    return 1;
  } else {
    return 0;
  }  
}

/* parses next int and assigns it, returns 1 on success 0 on error*/
int  LineParser::parse (int & c) {
  string s;
  if (this->parse(s)) {
    if ( (c = toint(s)) ==-1 ) return 0;
    return 1;
  }
  return 0;
}

/* setting delims */
void LineParser::setDelim (const string &s) {
  nbDelim = s.size()<12?s.size():12;
  for (int i=0;i<nbDelim;i++) 
    delim[i]=s[i];
}

/* printing for debug */
ostream & operator<<(ostream &os,LineParser &l) {
  os << "LineParser :" << endl << l.b << endl << "at char " << l.pos <<endl;
  return os;
}
