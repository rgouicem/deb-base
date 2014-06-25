This tool allows to parse observation transitions from a text based format ;

This tool computes static subclasses for GreatSpn. Input is a CAMI model file. Options :
    -m path : specifies the path to cami model
    -p path : specifies the path to properties file
    -f filter: specifies the transitions to parse in the properties file
               filter is a list of comma separated atomic property names
    -show   : exports Cami model for viewing (non standalone mode)
    --help,-h : display this (very helpful) helping help text
Problems ? Comments ? contact Yann.Thierry-Mieg@lip6.fr

The 
-p option specifies a path to a tobs file in the format defined below
-f tname1,...,tnameN specifies a list of transitions to filter through 
          (i.e. only those mentionned in the formula to be checked) 
          This allows to reuse the properties file for more than one formula


The format for the text file is the following (see parsers/tobsLexer.lex parsers/tobsParser.yacc for more details) :

#trans TNAME
#inhib 
PNAME "ARCVAL"
PNAME2 "ARCVAL2"
#pre
PNAME "ARCVAL"
#guard "GUARD"
#endtrans
#trans TNAME2
... etc
#endtrans

All fields (inhibitor,pre,guard) are optional. The transition name is mandatory and should be unique. The order inhib/pre/guard is enforced. 

PNAME must be a valid place name in PN, and ARCVAL a CAMI arc function expression (in correct extended CAMI syntax) w.r.t. the domain of PNAME

An arbitrary number of arcs may be added (to pre or inhib)  but no two arcs should reference the same place.
At least one and as many transitions as desired can be put in a single file. 

Arc functions and guards should be enclosed in double quotes " ".

All names should be legal C identifiers :  (_|alpha)(_|alpha|numeric)*

whitespace are ignored, including newlines, however
keywords (#trans,#inhibitor ...)  should be placed after a newline


Example :

#trans P1ThinksNotEat
#inhibitor
Eating "<x>"
#pre 
Thinking "<x>"
#guard "[x = 1]"
#endtrans

Will be true in any state such that :
** there is at least a philosopher x in place Thinking, 
** that same x is not Eating as well,
** this philosopher x is the philosopher 1. (Class Philo is 1..N_PHILO;)

Please note that the syntax accepted is extended AMI syntax for WN, it allows reference to static subclasses if they exist.
Variables have to be declared in the original model.

The atomic properties will have an impact on the state space generated, by changing the static/dynamic subclass declarations.


Implementation, API in SNOW :
The call (header PNetIO.h)
int  ParseTobs (const string &path, PNet *pPN=NULL) ;
adds the transitions defined in the file "path" to the model *pPN

An additional filter can be specified by setting :
void SetTobsFilter (const set<string> & s);
Once this function has been called, only transitions with names in the set s will be added to the model by a subsequent call to ParseTobs(). By default (or if SetTobsFilter is not called) all transitions in the file are parsed.