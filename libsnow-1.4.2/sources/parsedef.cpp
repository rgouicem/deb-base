#include "parsedef.h"
#include <cstdio>
#include <cstdlib>



DefFile::DefFile (const string & path,PNet * pPN) {

  FILE * def_fp = fopen(path.c_str(),"r");
  if (! def_fp) {
    cerr << "\nCould not open file " <<  path<<". Please check if the file exists and permissions are set to readable\n"<< endl;
    exit(1);
  }
  
  bool start_parse=false;
  char tmp[MAXSTRING];
  char read_name[MAXSTRING];
  int curline = 0;



  ParserWrapper::setModel(pPN);
  
  while( true )
    {/* Scanning .def file */
      if( fgets( tmp, MAXSTRING - 1, def_fp ) == NULL )
	if( feof( def_fp ) )
	  break;
      if(tmp[0]=='|' )
	{/* Rate MD o delimitatore */
	  if(tmp[1]=='2' && tmp[2]=='5' && tmp[3]=='6' && tmp[4]=='\n')
	    {/* Delimitatore |256 */
	      do
		{
		  fgets( tmp, MAXSTRING - 1, def_fp );
		}
	      while(tmp[0]!='|' || tmp[1]!='\n');
	      start_parse = true;
	    }/* Delimitatore |256 */
	  else
	    {/* Definizioni di rate MD */
	      /* SEE  gspn/greatspn/SOURCES/WN/SOURCE/READNET/read_DEF.c for more details */
	      /* skip MD_RATE definitions */
	      continue;
	    }/* Definizioni di rate MD */
	}/* Rate MD o delimitatore */
      else if (start_parse)
       {/* Superato l'inizio file */
	 if(tmp[0]=='(')
	   {/* Definition of colors, markings , functions */
	     char *name_p =  tmp + 1;
	     float xcoord,ycoord;
	     char type;
	     char trash[4];
	     sscanf(name_p,"%s %c %f %f %c",read_name,&type,&xcoord,&ycoord,(char *) &trash);
	     
	     /* Lettura della definizione */
	     fgets( tmp, MAXSTRING - 1, def_fp );
	     curline ++;
	     /* Tipo di definizione e in tmp c'e' la definizione */
	     switch(type)
	       {
	       case 'c' : 
		 {
		   decls.push_back( Declaration(color,read_name,tmp,Position (xcoord,ycoord),curline));
		   ParserWrapper::gspnParseColor (tmp,read_name);
		   break;
		 }
	       case 'm' : if(tmp[0]=='(')
		 {/* Definizione di sottoclasse dinamica */
		   cerr << " Dynamic subclass definitions are not yet supported by this tool. Mail yann.thierry-mieg@lip6.fr if this is a feature you would like to see supported in new versions."<<endl;
		   exit(1);
		 } else {
		   /* Definizione di marcatura */
		   decls.push_back( Declaration(marking,read_name,tmp,Position (xcoord,ycoord),curline));

		 }
		 break;
	       case 'f' :
		 decls.push_back( Declaration(function,read_name,tmp,Position (xcoord,ycoord),curline));
		 break;
	       }
	     fgets( tmp, MAXSTRING - 1, def_fp );
	   }/* Definizioni di colori, marcature o funzioni */
       }/* Superato l'inizio file */
    }/* Scanning .def file */
 
  fclose (def_fp);

//  cerr << pPN->LClasse << endl <<endl ;

  pPN->LClasse.Reindex();

//  cerr << pPN->LClasse << endl <<endl ;


 
}



ostream & operator<< (ostream & os,const DefFile &d) 
{
  vector<Declaration>::const_iterator it;
  int i; 
  for ( it = d.decls.begin(), i=0 ; it != d.decls.end() ; it++,i++) 
    os << "line " <<i << ": " <<endl << *it ;
  return os ;
}


ostream & operator<<(ostream & os,const Declaration &d)
{
  os << "Type :" ;
  switch (d.type) {
  case marking : 
    os << "marking";
    break;
  case function : 
    os << "function";
    break;
  case color : 
    os << "color";
    break;
  }
  os << endl << "name : " <<d.name << endl << "decl :"<< d.decl << endl;
  return os;

}
