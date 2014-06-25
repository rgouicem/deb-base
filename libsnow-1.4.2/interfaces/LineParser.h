#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

class LineParser {
  char *b;
  int len;
  char *cp;
  int pos;
  char delim[13];
  int nbDelim;
public:
  // constructor
  LineParser(ifstream &);
  LineParser(const string & s);
  // destructor
  virtual ~LineParser(){free(b);};
  //
  /* parses a token into s , returns number of characters parsed;optional delimiter can be specified*/
  int parse(string &s,char='\0');
  /* parses a Cami format string  into s , returns number of characters parsed ;
   i.e. 11:declaration will put "declaration" in s and return 11 */
  int parseCami (string &s);

  /* parses an int into n , returns number of characters parsed*/
  int parse(int &n);
  /* skips ' ' and '\t' returns number of chars skipped */
  int skipWhite();
  /* parses next non white char if c=='\0' and returns it
     else attempts to parse character c, returns 1 on success*/
  char parse(char c='\0');
  /* moves the current position back one char <=> return a char to the parser
   !! Note that this operation is not reverse of parse(char c) since whitespace are not distinguished by
   !! ungetc()
   returns 1 on success, 0 if pos was already 0
  */
  int ungetc() ; 
  /* returns true if at end of line */
  bool  eol() ;

  /* homogeneous error handling */
  void parseError (const string);
  /* setting delims */
  void setDelim (const string &s="");
  /* printing for debug */
  friend ostream & operator<<(ostream &os,LineParser &l);

};

/* returns parse int from string */
extern int toint (const string &s);
