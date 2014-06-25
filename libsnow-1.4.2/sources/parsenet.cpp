#include "LineParser.h"
#include "parsedef.h"
#include "parsenet.h"
#include <cstdio>
#include <cstring>

using namespace std;




namespace parsenet {

  static const int INPUT_ARC = 0;
  static const int OUTPUT_ARC = 1;
  static const int INHIB_ARC = 2;
  
  PNet * pPN;
  DefFile *deff;
  

  /* Parse from net_fp, numarcs arcs around transition trans, of type arctype = any of the const above */
  void parseArcs (int numarcs, Transition *trans, FILE * net_fp, int arctype) {

    char tmp[MAXSTRING];

    for(int item_arc = 1; item_arc <= numarcs; item_arc++)
      { /* Lettura di ogni arco di ingresso */
	

	fgets( tmp, MAXSTRING - 1, net_fp );
		
	
	int multiplicity,n_inflex,plindex,char_read;
	sscanf(tmp,"%d %d %d %n",
	       &multiplicity, // multiplicity of the arc = color function B/W
	       &plindex,   // index of connected place
	       &n_inflex,  // number of inflexion points
	       &char_read);


	/* Set place (input or ouput);*/
	Place * p = pPN->LPlace.Find(plindex);
	if (!p) {
	  cerr << "ERROR : Incorrect place index encountered while parsing arc" <<endl << tmp<< endl;
	  cerr << "Fatal Error, could not recover, sorry." <<endl;
	  exit(1);
	}
	if (multiplicity <0) {
	  multiplicity = -multiplicity;
	  cerr << "WARNING : broken arc feature is not preserved by this tool." << endl;
	}
		
	string func;
	float funcxpos,funcypos;
	char *name_p;
	name_p = tmp + char_read;

	/* SKIP LAYER */
	int skip_layer ;
	do
	  {
            sscanf(name_p,"%d%n",&skip_layer,&char_read);
            name_p+=char_read;
	  }
	while (skip_layer);	
	



	switch(*name_p) {
	case '@'  : {
	  /************************************/
	  /*  Da implementare adattando la    */
	  /*  grammatica affinche'riconosca   */
	  /*  come color anche descrizioni    */
	  /*  di funzioni riempiendo una ta   */
	  /*  bella			      */
	  /************************************/
	  break;
	}
	case '\n' : {/* Nessuna funzione */
	  char bb[256];
	  sprintf(bb,"%d",multiplicity);
	  func = bb;
	  if (p->Dom()->Name() != "null") {
	    if (multiplicity != 1) {
	    cerr << "PARSE ERROR: No color function on arc between transition \n" << *trans << endl << " and place\n " << *p << endl << "but place domain is not uncolored." << endl;
	    cerr << "FATAL, sorry." << endl;
	    exit(1);
	    } else {
	      cerr << "WARNING : using implicit projection on default variable(s) as color function of arc linking \n" << trans->Name() << endl << " and place\n " << p->Name() << ". Assuming function :" ;
	      func = "<" ;
	      list<PNClass*>::iterator it;
	      for (it = p->Dom()->Elts().begin() ; it != p->Dom()->Elts().end() ; it++) {
		func += (*it)->Name() + "_defvar";
		if ( it != --p->Dom()->Elts().end()) func += "," ;
	      }
	      func += ">";
	      cerr << func << endl <<endl;
	    }
              }
	  break;
	}
	default : {/* Arco con funzione */
	  
	  int toskip;
	  sscanf(name_p,"%d %f %f %n",&toskip,&funcxpos,&funcypos,&char_read);
	  name_p = name_p + char_read;
	  /* set func to correct arc function, look in def file for function or use end of tmp line */
	  {
	    LineParser l (name_p);
	    l.parse(func);
	    
	    vector<Declaration>::const_iterator it;
	    for (it = deff->decls.begin() ; it != deff->decls.end() ; it++ ) {
	      if (func == it->name) {
		func = it->decl;
		break;
	      }
	    }
	    if (it == deff->decls.end()) 
	      func = name_p;
	  } /* end set func string */
	}
	} /* switch */



	/* create arc */
	Arc *pa =pPN->LArc.Insert( Arc(-1,p,trans,( arctype== OUTPUT_ARC )? Trans2Place : Place2Trans,func,-1,( arctype== INHIB_ARC ),true) );
	p->AddArc(pa);
	trans->AddArc(pa);

//	cerr << "ADDED ARC with function : " << pa->Valuation();

	/* SKIP inflexion point position lines */
	for(int item_skip = 0; item_skip < n_inflex ; item_skip++) {
	  float x,y;
	  fgets( tmp, MAXSTRING - 1, net_fp ); 
	  sscanf(tmp,"%f %f",&x,&y);
	  pa->addInflexion(Position(x,y));
	}


      }


  }


  void parsenet(const string & path, DefFile * ddeff, PNet * ppPN) {
    pPN = ppPN;
    deff = ddeff;
    FILE * net_fp = fopen(path.c_str(),"r");
    if (! net_fp) {
      cerr << "\nERROR : Could not open file " <<  path<<". Please check if the file exists and permissions are set to readable\n"<< endl;
      exit(1);
    }

    char tmp[MAXSTRING];
    char type;
    int el[6];
    vector<int> mark_param;

    while( true) {
      /* Scanning .net file */
      if( fgets( tmp, MAXSTRING - 1, net_fp ) == NULL )
	if( feof( net_fp ) )
	  break;
      /* skip comment lines until NOOBJS line */
      if(tmp[0]=='|' && tmp[1]=='\n')
	{/* Parsing della prima riga */
	  fscanf(net_fp,"%c %d %d %d %d %d %d",&type,&el[0],&el[1],&el[2],&el[4],&el[3],&el[5]);
	  /* skip whitespace */
	  while(getc(net_fp)!='\n');
	  break;
	}/* Parsing della prima riga */
    }/* Scanning .net file */
    /*********** MARKING PARAMETERS ***********/
    if(el[0] > 0)
      {/* Ci sono marking parameters */
// 	cerr << "ERROR : Marking parameters are not yet supported by this tool. Mail yann.thierry-mieg@lip6.fr if this is a feature you would like to see supported in new versions."<<endl<<endl;
// 	exit(1);
	int int_val;
	char read_name[MAXSTRING];
	float xcoord1,ycoord1;
	xcoord1 =ycoord1 = 1;

	    for(int item = 1; item <= el[0]; item++)
	      {/* Lettura dei Marking Parameters */
		fgets( tmp, MAXSTRING - 1, net_fp );
		sscanf(tmp,"%s %d %f %f 0",read_name,&int_val,&xcoord1,&ycoord1);
		mark_param.push_back(int_val);
	      }
      }  
    /*********** PLACES ***********/
    if(el[1] > 0)
      {
	/* Ci sono posti */
	char *name_p;
	char read_name[MAXSTRING];
	int int_val,char_read;
	float xcoord1,ycoord1,xcoord2,ycoord2,xcoord3,ycoord3;
	xcoord1 =ycoord1 = 	xcoord2 =ycoord2 = 	xcoord3 =ycoord3 = 1; 

	Domain * pdom =NULL;
	list<string> domdesc;
	    
	for(int item = 1; item <= el[1]; item++)
	  {
	    /* Lettura dei Posti */
	    fgets( tmp, MAXSTRING - 1, net_fp );
	    sscanf(tmp,"%s %d %f %f %f %f %n",read_name,&int_val,&xcoord1,&ycoord1,&xcoord2,&ycoord2,&char_read);
	    
	    /* Skip layer information */
	    int skip_layer;
	    name_p = tmp + char_read ;
	    do
	      {
		sscanf(name_p,"%d%n",&skip_layer,&char_read);
		name_p+=char_read;
	      }
	    while (skip_layer);
	
	    pdom = NULL;
	    domdesc = list<string>();
	    /* Parse domain description */
	    switch(*(name_p))
	      {/* Tipo di dominio del posto */
	      case '@'  : {
		/************************************/
		/*  Da implementare adattando la    */
		/*  grammatica affinche'riconosca   */
		/*  come color anche descrizioni    */
		/*  di domini riempiendo una tabella*/
		/************************************/
		break;
	      }
	      case '\n' : {/* Dominio neutro */
		
		break;
	      }/* Dominio neutro*/
	      default   : {/*Dominio colorato da parsificare*/
		sscanf(name_p,"%f %f %n",&xcoord3,&ycoord3,&char_read);
		name_p+=char_read;
		/* kill ending \n */
		*( name_p + strlen(name_p) - 1 ) = '\0' ;
		LineParser l (name_p);
		l.setDelim(",\n");
		string s;
		while (! l.eol() ) {
		  l.parse(s);
		  if (! l.eol()) l.parse(',');
		  domdesc.push_back(s);
		}
	      }		
	      } /* end parse domain definition */
	    
	    pdom = pPN->LDom.FindNameList(domdesc);
	    if (pdom == NULL) {
	      Domain d;
	      PNClass *pC;
	      list<string>::iterator it;
	      for (it=domdesc.begin(); it != domdesc.end() ; it++ ) {
		if ( (pC = pPN->LClasse.FindName(*it)) == NULL ) {
		  cerr << "ERROR : Unknown class " << *it << " encountered in definition of domain of place " << read_name << endl;
		  exit(1);
		}
		d.AddElt(pC);
	      }
	      int dummy = item;
	      d.genName(dummy); // genName is configured to increase arg in some cases

	      pdom = pPN->LDom.Insert(d);
	    }
	    
	    /* Construct a place */
	    Place p(item,-1,read_name,pdom);
	    
	    /* Add marking */
	    if (int_val > 0) {
	      Marking *m = new Marking();
	      m->Insert(Mark(int_val));
	      p.setMarking(m);
	    } else if (int_val < -9800) {
	      int_val = -int_val;
	      int_val -= 10000;
	      /* index start from 1 !! */
	      int_val -= 1 ;
	      if (deff->decls[int_val].type != marking) {
		cerr << "ERROR :  marking for place "<<read_name << " references a line in .def file that is not a marking definition " <<endl;
		exit(1);
	      }
	      p.setMarking(deff->decls[int_val].decl,true);
	    } else if (int_val < 0) {
	      /* This should be a marking parameter defined earlier in the .net file */
	      /* only valid for B/W places */
	      Marking *m = new Marking();
	      int index = -int_val-1;
	      if (index < 0 || index >= int(mark_param.size())) {
		cerr << "There seems to be a problem in your input file. Marking for place " << read_name << " references a marking parameter that is undefined. \n Exiting, sorry. ";
		exit(1);
	      }
	      m->Insert(Mark(mark_param[index]));
	      p.setMarking(m);
	    } else {
	      /* int_val == 0 implies marking is empty */
	    }
	    p.setPos(Position(xcoord1,ycoord1));
	    p.namepos = Position(xcoord2,ycoord2);
	    p.dompos = Position(xcoord3,ycoord3);
	    pPN->LPlace.Insert(p);
	  } /* lettura dei posti */
      } /* if num places > 0 */
    /*********** RATE PARAMETERS ***********/
    if(el[2] > 0)
      {/* Ci sono rate parameters */
	cerr << "ERROR : Rate parameters are not yet supported by this tool. Mail yann.thierry-mieg@lip6.fr if this is a feature you would like to see supported in new versions."<<endl<<endl;
	exit(1);
      }
    /*********** PRIORITY GROUPS ***********/
    if(el[3] > 0)
      {/* Ci sono groups */
	cerr << "WARNING : SKIPPING : Priority Groups are not yet supported by this tool. Mail yann.thierry-mieg@lip6.fr if this is a feature you would like to see supported in new versions."<<endl<<endl;
	for(int item = 1; item <= el[3]; item++)
	  {/* Lettura dei Groups */
	    fgets( tmp, MAXSTRING - 1, net_fp );
	  }
      }
    /*********** TRANSITIONS ***********/
//    cerr << pPN->LPlace ;
    
    if(el[4] > 0)
      {
	/* Ci sono transizioni */
        for(int item = 1; item <= el[4]; item++)
	  {/* Lettura delle transizioni */
	    fgets( tmp, MAXSTRING - 1, net_fp );

	    char read_name[MAXSTRING],*name_p;
	    int nserv,tkind,ninput,rot,char_read;
	    float xcoord1,ycoord1,xcoord2,ycoord2,xcoord3,ycoord3;
	    double rate;

	    string guard;

	    sscanf(tmp,"%s %lg %d %d %d %d %f %f %f %f %f %f %n",
		   read_name,
		   &rate,  // rate of the transition
		   &nserv,// number of servers
		   &tkind,// Stochastic transition kind  : 0 = exponential ; 127 = deterministic ; 0 < p < 127 = relative priority of trans
		   &ninput,// number of input arcs
		   &rot,// rotation coef of the trans  in graphical interface
		   &xcoord1,&ycoord1,// position of trans
		   &xcoord2,&ycoord2,// position of name tag
		   &xcoord3,&ycoord3,// position of ??
		   &char_read);
	    
	    if (!(rate == 1 || rate == 0)) {
	      cerr << "WARNING : SKIPPING : sorry transition rate are not yet supported by this tool. Mail yann.thierry-mieg@lip6.fr if this is a feature you would like to see supported in new versions."<<endl;
	      cerr << "Ignoring rate parameter for transition " <<read_name <<endl;
	      
	    }
	    Transition t(item,-1,read_name,tkind);
	    t.setPos(Position(xcoord1,ycoord1));
	    t.namepos= Position(xcoord2,ycoord2);
	    if (nserv != 1) {
	      t.Nserv(nserv);
	    }
	    name_p = tmp + char_read;
	    int skip_layer;
	    do
	      {
		sscanf(name_p,"%d%n",&skip_layer,&char_read);
		name_p+=char_read;
	      }
	    while (skip_layer);
	    
	    switch(*(name_p))
	      {/* Tipo di dominio della transizione */
	      case '@'  : {
		/************************************/
		/*  Da implementare adattando la	   */
		/*  grammatica affinche'riconosca   */
		/*  come color anche descrizioni    */
		/*  di domini riempiendo una tabella*/
		/************************************/
		break;
	      }
	      case '\n' : {/* Dominio neutro*/
		break;
	      }/* Dominio neutro*/
	      default   : {/*Dominio colorato da parsificare*/
		while((*name_p)!='[' && (*name_p)!='#' && !isalpha(*name_p))
		  *name_p += 1;
		/* look for corresponding def file line */
		vector<Declaration>::iterator it;
		for (it = deff->decls.begin() ; it != deff->decls.end() ; it++) {
		  if (! strcmp(name_p,it->name.c_str()))
		    break;
		}
		
		if (it == deff->decls.end())
		  guard = name_p;
		else
		  guard = it->decl;
		/* Parse guard after arcs */
	      }
	      } /* switch*/

	    Transition * pt = pPN->LTrans.Insert(t);
	    /*********** INPUT ARCS *************/
	   
	    parseArcs(ninput, pt, net_fp,INPUT_ARC);

	    /*********** OUTPUT ARCS *************/
	    fgets( tmp, MAXSTRING - 1, net_fp ); 
	    int noutput;
	    sscanf(tmp,"%d",&noutput);
	    
	    parseArcs(noutput, pt, net_fp,OUTPUT_ARC);
	    
	    
	    /*********** INHIBITOR ARCS *************/
	    fgets( tmp, MAXSTRING - 1, net_fp ); 
	    int ninhib;
	    sscanf(tmp,"%d",&ninhib);
	    
	    parseArcs(ninhib, pt, net_fp,INHIB_ARC);
 
	    /* PARSE TRANSITION GUARD */
	    if (guard != "" )
	      pt->setGuard(guard,true);

	  } /* read one trans */
      } /* transitions*/

    pPN->reIndex();
  } /* parsenet function */


} /* parsenet namespace */
