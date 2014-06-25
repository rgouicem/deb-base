
/* module de gestion des arbres */
#define	PRINTF_ALLOWED	1

#include <FKstdio.h>
#include <string.h>
#include "FKEnv.h"
#include "FKctype.h"		/* isalpha */
#include "ConstMatrice.h"
#include	<FKErrors.h>
#include	<FKMain.h>
#include "FKTypes.h"
#include <FKResults.h>
#include "FKModel.h"
#include        <FKCLSend.h>    /* kFk_CAMI_CommandSize */

#include "CListes.h"
#include "Arbres.h"
#include "ListeVariables.h"
#include "ListeArcs.h"
#include "ListeTransitions.h"
#include "ListePlaces.h"
#include "global.h"
#include "ListeClasses.h"
#include "ListeDomaines.h"
#include "ListeVariables.h"
#include "ExprArbre.h"
#include "RedDom.h"
#include "HashTypes.h"






 /*#define MYFREE(_cp) (free(_cp),_cp=0) */
#define MYFREE(_cp) (_cp=0)
/* bizzare problem related to garbage collecting, track it in Purify */

/* explores guard, rewriting preds on varid as references to elements of predParts */
static void RewritePredsAsParts (int varid, PTypePred  *predParts,int predPartsSize ,PTypePred guard) ;
/* returns 1 if varid is last var in numeric order for the domain it belongs to ; 
   used to delay call to RewritepredAsParts until we' ve reached last var (i.e. partition is finished for this class */
static int isLastVarOfDom (int varid) ;
/* returns 1 if at least one pred in guard concerns varid ; 
   used to cut short exploration if no mention of curvar in guard */
static int isMentioned (PTypePred guard, int varid) ;
/* Critical function used to insure the property of our data structure : no two preds on a same var have overlapping sets
   Explores Varsets  to decompose them into non overlapping sets 
   sets are decomposed in separate parts */
static void SeparatePreds ( PTypePred *VarSets ,int VarsSetSize, PTypePred guard,int curvar,PTypePred *predParts,int *PpredPartsSize) ;
/* Used to eliminate link nodes that were introduced in Camix def of expression trees ;
   These nodes were defined originally to obtain a lisp-like combed tree (arbre peign'e) */  
static PTypeNoeud ElimineLinks (PTypeNoeud guard);
/* The main function that does the coordinating of calls to primitives 
   Called after init, exited before release of globs */
static void GenerePredicatsDisjoints (void);  
/* Fills global vars Classes,doms and Vars with info directly parsed from the .dec_model file 
   Should really use the info as already parsed by matrix, but still partly parses .decmodel for now*/ 
static void LoadHashClasses (void) ;
/* Returns the dual operator of the entry op
   Used to insure an order in preds : var encountered on left ; i.e : 5>X -> X(NOTop(>))5 = X <= 5 */
static char * NOTop (char * op) ;
/* Returns the numeric varid of a var mentioned by name */
static int getVarId (char *name) ;
/* Constructs an equivalent PredTree from a NoeudTree ; translates the structure imported from Camix into our 
   native format; */
static PTypePred  GenereArbrePred (PTypeNoeud guard);
/* Returns 1 if comparison by op of val and borne returns TRUE ; 
   0 otherwise ; used to generate the acceptance set implied by a guard in comparison form */ 
static int testPred (int val,int borne,char *op) ;
/* Frees all global vars (duh !) */
static void freeAllglob (void) ;
/* Defines an order on sets ; returns 1 if a before b, 0 if a = b and -1 otherwise
   Order is defined by a < b iff there exists in "a" a minorant for elements of b 
   Makes the strong asumption that a and b have same length; should only be called from TriPreds */ 
static int TriSets (int * a,int *b);
/* Defines an order on preds, makes assumption that a and b are preds on the same varid
   Order is defined by a < b iff a->setsize < b->setsize or sets have same size and a->set < b->set (TriSets) */
static int TriPreds (PTypePred a,PTypePred b); 
/* explores guard, adding preds on curvar to VarSets in sorted order => any doubles are eliminated */
static void FindPredOnVar (PTypePred guard, PTypePred * VarSets, int numVar,PTypePred pere) ;
/* returns maximal class size in number of elements ;
   used for dynamic memory allocation according to  needs */ 
static int MaxClasseTaille (void) ;
/* Eliminates identical preds in guard ; tokeep replaces doublon in guard
   Used in iterations of separatePreds, that may create identical preds */
static void EliminateDoublons (PTypePred guard, PTypePred tokeep, PTypePred doublon) ;
/* subtile. Looks for preds on targetVar at depth superior strictly to 1
   This search is recursive, and returns the path to the pred that is too deep
   or NULL if no preds are left to develop */
static PTypePath FindMisbehavedPred ( PTypePred guard, int targetVar, int curdepth ) ;
/* Canonizes guard in function of curVar, i.e. develops preds on curVar
   startVar is used as a finish test, as the canonize phase is recursively called on minterms 
   for each   var mentioned in guard, in the order defined by varid*/
static PTypePred CanonizePredTree (PTypePred guard,int curVar,int startVar) ;
/* Applies the rules to develop (raise in the guard) the pred pointed to by predPath
   uses the path returned by FindMisbehavedPred */
static void RemontePred (PTypePath predPath) ;
/* primitive to free a TypePath */
static void freeTypePath (PTypePath predPath) ;
/* Calculates the quotient of guard by diviseur
   The quotient F' of F by X= (x in Set) is defined by replacing
   any instance of X in F by TRUE
   any instance of the form X2 = X in Set2 by FALSE
   This operation is used to reduce the formula by X.F = X.F' */
static PTypePred ArbreQuotient (PTypePred guard,PTypePred diviseur) ;
/* Is used in REmontePred, after applying one rule to raise Pred
   the path remaining to reduce is shortened by this primitive */ 
static void EatPathHead (PTypePath predPath) ;
/* Allocates and initializes a new Leaf Pred with the info specified
   Could be optimized thru use of a hash */
static PTypePred newLeafPred (int varid,int *set,int setsize) ;
/* Allocates and initializes a new operator Pred (i.e. internal node) with the info specified
   Could be optimized thru use of a hash */
static PTypePred newOpPred (char op,PTypePred fg,PTypePred fd) ;
/* Allocates and initializes a new comparison Pred (i.e.X = Y)  with the info specified
   Could be optimized thru use of a hash */
static PTypePred newCompPred(char op,int varid1,int varid2);
/* Inserts minT in minTab in sorted order */
static void insereTrie (PMinTerm minT, PMinTermTab minTab) ;
/* Defines the order on Minterms for inesereTrie */
static int TriMinTerm (PMinTerm a, PMinTerm b) ;
/* Refines the order on Preds to compare Comparison Preds
   Order is based on lexicographic order */
static int TriCompPreds ( PTypePred a,PTypePred b) ;
/* Allocates and initializes a new Minterm structure with the info specified */
static PMinTerm newMinTerm (PTypePred  pred,  PTypePred pereET,  PTypePred minT) ;
/* Loads minTab with Minterm Structures on targetVar 
   as deduced from exploring guard. MinTab is returned in sorted order */
static void FindMinTerm (PTypePred guard, int targetVar, PMinTermTab minTab) ;
/* Calculates the quotient of guard by diviseur, then simplifies the resulting pred tree */
static PTypePred ArbreQuotientSimplifie (PTypePred guard,PTypePred diviseur) ;
/* Applies basic boolean algebra formulas to reduce the expression "guard"
   Among these are : 0.A=0 1.A=A A+A=A.A=A 0+A=A 1+A=1 ... 
   Unless the result is a single TRUE or FALSE pred, TRUE and  FALSE preds
   should be eliminated by this operation */
static PTypePred SimplifierArbre (PTypePred guard) ;
/* service function, pretty-prints set in buff.
   set is a -1 terminated array of int */
static void PrintSet (int *set,char *buff) ;
/* returns index in class->set of element described by string elt */
static int stringEltNum (PtabClasse class,char *elt) ;
/* service function, pretty-prints a pred formula in buff. */
static void PrintPredTree (PTypePred pred,char *buff) ;
/* service function, pretty-prints a pred formula in buff. 
   Protects against a depth of recursion superior to 32 which might indicate
   a cycle in pred formula */
static void PrintPredTreeNoCycle (PTypePred pred,int depth,int isStatic,char *buff) ;
/* service function, pretty-prints a pred formula in buff. 
   Prints leaf preds that are refs to static subclasses correctly */
static void PrintStaticPredTree (PTypePred pred,char *buff) ;
/* Inserts a pred in predTab, in sorted order */
static void InsertPred (PTypePred pred,PTypePred *predTab,int VarsSetSize) ; 
/* Recursively compares two pred trees returns 1 if they define equivalent formulas */   
static int IsIdenticalPred (PTypePred  a,PTypePred b) ;
/* Applies fusion rule to 2 preds on same variable
   i.e. a= X in Set1 , b  = X in Set2 ->X in Set1 U Set2*/
static PTypePred FusionnePred (PTypePred a,PTypePred b) ;
/* Adds a pred to the garbage, delays deallocation of the pred
   until garbageCollect */
static int addGarbage (PTypePred pred);
/* empties the garbage by calling freeTypePred 
   on global array Garbage */
static void GarbageCollect (void);
/* Allocates a physical copy of guard */  
static PTypePred Clone (PTypePred guard) ;
/* Recursive part of the clone procedure */
static PTypePred CloneRec (PTypePred guard) ;
/* Exports basic class,dom and var info to the model in construction */
static void ExporteDomAndVars (void) ;

 

/********************* Le Main  **********************************/
extern void ReduceDomains (void) {
  
 fprintf (stderr,"redddddddddddddddddddd sTAAAAAAAART   !!!!! \n");
  
  ListeArcsDetecteSucc(ClassesTab,ClassesTabSize,VarsTab,VarsTabSize );
  /* L' action ************/
  GenerePredicatsDisjoints();

}
  
    


/**/
static void GenerePredicatsDisjoints (void) 
{
  int nb,gNum;
  PTypeNoeud *GuardsTab ;
  PtabClasse class;
  PTypePred *VarSets=NULL;
  int VarsSetSize;
  int curvar,i ;
  PTypePred allGuards;
 int ReducedVar;
 char mess [256];
   
  for (ReducedVar = 0;ReducedVar < VarsTabSize;ReducedVar++ ) {
    /* iterate over each guard */
    /* Shortcircuit the study if successor operators have been found on this class */
    /* This implies the only partition we can make is one that places an element in each subclass :( */
    /* NO GAIN from symbolic model checking here */
    if ( ClassesTab[VarsTab[ReducedVar]->domaine]->IsOrdered == 0) {
      for (gNum=0;gNum<nb;gNum++) {
	/* iterate over variables of the model */
	for (curvar=0;curvar<VarsTabSize;curvar++) {
      
	  class = ClassesTab[VarsTab[curvar]->domaine] ;
	  VarsSetSize = class->taille +1;    
	  /* initialize a tab for the current variable; maximum taille elts  */
	  VarSets = malloc(VarsSetSize*sizeof(PTypePred));
	  memset(VarSets,0,VarsSetSize*sizeof(PTypePred));
	  
	  
	  VarsSetSize = class->taille +1; 
	  
	  /* explore the tree looking for preds on curVar get them sorted (TriPred=> no identicals) in VarSets*/
	  FindPredOnVar(PredsTab[gNum],VarSets,curvar,NULL);
	  /* Make sure each var value is mentionned in at most one pred */
	  SeparatePreds(VarSets,VarsSetSize,PredsTab[gNum],curvar,NULL,0);
	}
	
/* 	if (ReducedVar == 0) { */
/* 	  sprintf(mess,"With Unique preds, guard %d= ",gNum); */
/* 	  FkSendTextResponse( mess); */
/* 	  *mess = 0; */
/* 	  PrintPredTree(PredsTab[gNum],mess); */
/* 	  strcat(mess,"\n"); */
/* 	  FkSendTextResponse(mess); */
/* 	} */
	PredsTab[gNum] = SimplifierArbre (PredsTab[gNum]);
/* 	if (ReducedVar == 0) { */
/* 	  sprintf(mess,"After Simplify, guard %d= ",gNum); */
/* 	  FkSendTextResponse( mess); */
/* 	  *mess = 0; */
/* 	  PrintPredTree(PredsTab[gNum],mess); */
/* 	  strcat(mess," "); */
/* 	  FkSendTextResponse(mess); */
	  
/* 	}  */
	if ( isMentioned(PredsTab[gNum],ReducedVar) )
	  PredsTab[gNum] = CanonizePredTree (PredsTab[gNum],ReducedVar,ReducedVar);
	
      }
    
    /* iterate over each guard */
/*     fprintf (stderr,"\nAtfer dev with respect to var : %s\n",VarsTab[ReducedVar]->nom); */
/*     for (gNum=0;gNum<nb;gNum++) { */
/*       if (isMentioned(PredsTab[gNum],ReducedVar)) { */
/* 	sprintf(mess," guard %d= ",gNum); */
/* 	FkSendTextResponse( mess); */
/* 	*mess = 0; */
/* 	PrintStaticPredTree(PredsTab[gNum],mess); */
/* 	strcat(mess," "); */
/* 	FkSendTextResponse( mess); */
/*       } */
/*     } */
/*     sprintf(mess," "); */
/*     FkSendTextResponse( mess); */
    
    {
      int nbTsurVar = 0;
      int gardesNum[nb];
      
      for (gNum=0;gNum<nb;gNum++) 
	if  (isMentioned(PredsTab[gNum],ReducedVar))  
	  {
	    gardesNum[nbTsurVar]=gNum;
	    nbTsurVar++;
	  }
      
      if (nbTsurVar) {
	if (nbTsurVar == 1)
	  allGuards = PredsTab[gardesNum[0]];
      
	else if (nbTsurVar >= 2) {
	  allGuards = newOpPred('+',PredsTab[gardesNum[1]],PredsTab[gardesNum[0]]);
	  for (gNum=2;gNum<nbTsurVar;gNum++) {
	    allGuards = newOpPred('+',PredsTab[gardesNum[gNum]],allGuards);
	  }
	}
	
	
	/*    fprintf(stderr,"After Combining guards, Allguards = \n"); */
	/*     PrintPredTree(allGuards); */
	/*     fprintf(stderr,"\n"); */
	
	class = ClassesTab[VarsTab[ReducedVar]->domaine] ;
	MYFREE(VarSets);
	
	VarsSetSize = class->taille +1; 
	
	/* initialize a tab for the current variable; maximum taille elts  */
	VarSets = malloc(VarsSetSize*sizeof(PTypePred));
	memset(VarSets,0,VarsSetSize*sizeof(PTypePred));
	
	
	allGuards = newOpPred('+', newLeafPred(ReducedVar,class->ens,class->taille),allGuards);
	/* explore the tree looking for preds on curVar get them sorted (TriPred=> no identicals) in VarSets*/
	FindPredOnVar(allGuards,VarSets,ReducedVar,NULL);
	
    
	/* Make sure each var value is mentionned in at most one pred */
	{
	  PTypePred  *predParts;
	  PTypePred *staticParts = StaticClass[class->id];
	  PTypePred dummyParts;
	  int predPartsSize = 0;
	  int staticPartsSize = StaticClassSize[class->id];
	  
	  
	  predParts = malloc(VarsSetSize*sizeof(PTypePred));
	  memset(predParts,0,VarsSetSize*sizeof(PTypePred));
   
	  
	  SeparatePreds (VarSets,VarsSetSize,allGuards,ReducedVar,predParts,&predPartsSize);
	  
	  
	  if (! staticParts ) {
	    StaticClass[class->id] = predParts;
	    StaticClassSize[class->id] = predPartsSize;
	  } else {
	    int numVar = ReducedVar;
	    /* 	for (numVar = ReducedVar;numVar >=0;numVar--) { */
	    
	    if (VarsTab[numVar]->domaine == class->id) {
	      if (staticPartsSize == 1)
		dummyParts = newLeafPred(numVar,staticParts[0]->set,staticParts[0]->setsize);
	      else if (staticPartsSize >= 2) {
		dummyParts = newOpPred('+',
				       newLeafPred(numVar,staticParts[0]->set,staticParts[0]->setsize),
				       newLeafPred(numVar,staticParts[1]->set,staticParts[1]->setsize));
		for (i=2;i<staticPartsSize;i++) 
		  dummyParts = newOpPred('+',
					 newLeafPred(numVar,staticParts[i]->set,staticParts[i]->setsize),
					 dummyParts);
	      }
	      
	      allGuards = newOpPred('+',dummyParts,allGuards);
	      /* explore the tree looking for preds on curVar get them sorted (TriPred=> no identicals) in VarSets*/
	      memset(VarSets,0,VarsSetSize*sizeof(PTypePred));
	      FindPredOnVar(allGuards,VarSets,numVar,NULL);
	      
	      memset(predParts,0,VarsSetSize*sizeof(PTypePred));
	      predPartsSize = 0;
	      SeparatePreds (VarSets,VarsSetSize,allGuards,numVar,predParts,&predPartsSize);
	      
	      StaticClass[class->id] = predParts;
	      StaticClassSize[class->id] = predPartsSize;
	    }
	    /* 	} */
	  }
	  
	  
	  if (isLastVarOfDom(ReducedVar)) {
	    FILE * Fich_out;

	    fprintf(stderr,"Variable %s Forces to partition class %s into :\n",VarsTab[ReducedVar]->nom,class->nom);
	    
	    Fich_out = fopen("./FK_MNGR/.staticClasses","a+");
	    fprintf(Fich_out,"CLASS %s has %d subclasses\n",class->nom,predPartsSize);
	    for (i=0;i<predPartsSize;i++) {
	      char buff[256] ;
	      char message[256] ;
	      buff[0] = 0;
	      message[0] = 0 ;
	      
	      PrintSet(predParts[i]->set,buff);
	      sprintf(message,"CM(7:project,1,%d,1,%d:%s%d STATIC(%s) is %s;)",curProjLine++,2*strlen(class->nom)+strlen(buff)+14+1,class->nom,i,class->nom,buff);
	      fprintf(Fich_out,"STATIC  %s%d of %s is %s\n",class->nom,i,class->nom,buff);
	      FkClSendChannelCami(NULL, message);
	      /*   fprintf(stderr,"%s%d STATIC(%s) is [%s]\n",class->nom,i,class->nom,buff); */
	      
	    }
	    fclose (Fich_out);
	    for (i=0;i<ReducedVar;i++) {
	      if (VarsTab[i]->domaine == VarsTab[ReducedVar]->domaine) {
		int ggNum;
		
		for (ggNum=0;ggNum<nb;ggNum++) {
		  RewritePredsAsParts(i, predParts,predPartsSize ,PredsTab[ggNum]);
		  
		}
	      }
	    }
	    for (i=0;i<predPartsSize;i++) {
	      predParts[i]->set [0] =i;
	      predParts[i]->set [1] =-1;
	      predParts[i]->setsize = 1;
	      predParts[i]->nbref = 1 << 16;
	    }
	  }
	  
	  
	}
	
      }
    }
      
      
  }
    
  }
  
  sprintf(mess,"With respect to static subclasses Guards can be expressed as :"); 
  FkSendTextResponse( mess); 
  /* iterate over each guard */
  for (gNum=0;gNum<nb;gNum++) {
    sprintf(mess,"Guard %d :",gNum);
    FkSendTextResponse( mess); 
    *mess = 0;
    PrintStaticPredTree(PredsTab[gNum],mess);
    fprintf(stderr,"\n");
    FkSendTextResponse( mess);
  }
    

  ExporteDomAndVars();
  
  
  /********** FREE ***********/
  
  GarbageCollect();
  for (i=0;i<nb;i++) {
    FreeExprArbre (GuardsTab[i]);
    /*  FreePredArbre (PredsTab[i]);  */
  }
  MYFREE(GuardsTab);
  
  MYFREE(PredsTab);
  for (i=0;VarSets != NULL &&  VarSets[i]!=NULL;i++) { 
    MYFREE(VarSets[i]->set); 
  } 
  MYFREE(VarSets);
  
 
  
}


static void RewritePredsAsParts (int varid, PTypePred  *predParts,int predPartsSize ,PTypePred guard) {
  int i;

  if (guard == NULL) 
    return ;
  RewritePredsAsParts (varid, predParts,predPartsSize ,guard->fg);
  RewritePredsAsParts (varid, predParts,predPartsSize ,guard->fd);
  if (guard->varid == varid && guard->nbref < 1<<15 && guard->setsize!=0) {
    for (i=0;i<predPartsSize; i++) {
      if (predParts[i]->set[0] == guard->set[0]) {
	guard->set[0] = i;
	guard->set[1] = -1;
	guard->setsize = 1;
	guard->nbref = 1 << 16;
	break;
      }
    }
  }
}


static PTypePred CanonizePredTree (PTypePred guard,int curVar,int startVar) {
  PTypePath predPath;
  PTypePred newP;
  PMinTermTab minTab;
  int i;
 
  
/*     fprintf(stderr," Running Canonize on variable %d (startvar %d) with guard = \n",curVar, startVar); */
/*     PrintPredTree(guard); */
/*     fprintf(stderr,"\n"); */
  
  while ((predPath =FindMisbehavedPred(guard,curVar,0)) != NULL) { 
     RemontePred(predPath);
     freeTypePath(predPath);
  }  
  
  guard = SimplifierArbre (guard);
  
/*    fprintf(stderr,"After REmontePred, guard = \n");  */
/*    PrintPredTree(guard);  */
/*    fprintf(stderr,"\n");  */

  minTab = malloc(sizeof(minTermTab));
  memset (minTab,0,sizeof(minTermTab));
  minTab->minT = malloc(sizeof(PMinTerm));
  minTab->minT[0] = NULL;
  minTab->size = 1;
  
  FindMinTerm(guard,curVar,minTab);
  

  if ( (curVar+ 1) % VarsTabSize != startVar ) { 
    for (i=0;i<minTab->size - 1;i++) {
      minTab->minT[i]->minTerm = CanonizePredTree (minTab->minT[i]->minTerm, (curVar + 1) % VarsTabSize, startVar);
     }
  }


  /* factorisation/fusion des pred qui ont meme MinTerm */
  if (curVar==startVar) {
    PMinTerm tmp[minTab->size];
    int j;
    for (i=0; i < minTab->size -2; i++)
      for (j=i+1; j < minTab->size - 1 ; ) {
	if ( IsIdenticalPred(minTab->minT[i]->minTerm,minTab->minT[j]->minTerm )) {
	  minTab->minT[i]->pred = FusionnePred (minTab->minT[i]->pred,minTab->minT[j]->pred);
	  minTab->minT[i]->pereET->fg =  minTab->minT[i]->pred;
	  memcpy(tmp,minTab->minT+j+1,(minTab->size-j-1) * sizeof(PTypePred));
	  memcpy(minTab->minT+j,tmp,(minTab->size-j-1) * sizeof(PTypePred));
	  minTab->size --;
	  /*we've  already shifted ... */
	} else j++;
      }
  }


  if (minTab->size == 1) {
    guard= NULL;
  } else if (minTab->size == 2) {
    guard = minTab->minT[0]->pereET;
  } else {
    /* build the tree from last on up */
    PTypePred lastElt = minTab->minT[minTab->size -2]->pereET;
    PTypePred B4lastElt = minTab->minT[minTab->size -3]->pereET;
    newP = newOpPred('+',B4lastElt,lastElt);
    for (i=minTab->size-4;i>=0;i--) {
      newP = newOpPred('+',minTab->minT[i]->pereET,newP);
    }
    guard = newP;
  }

  /*   if (curVar == startVar) {   */
/*     fprintf(stderr,"Obtained expression : variable %d (startvar %d) with guard = \n",curVar, startVar); */
/*     PrintPredTree(guard); */
/*     fprintf(stderr,"\n\n"); */
/*     }   */
  return guard;

}








static void EliminateDoublons (PTypePred guard, PTypePred tokeep, PTypePred doublon) {
  if (guard == NULL) 
    return ;
  if (guard->fg == doublon) {
    guard->fg = tokeep;
  } 
  if (guard->fd == doublon) {
    guard->fd = tokeep;
  } 
  EliminateDoublons(guard->fg,tokeep,doublon);
  EliminateDoublons(guard->fd,tokeep,doublon);  

}

static int testPred (int val,int borne,char *op) {
  
  if (strcmp(op,"=") == 0) 
    return ( (val==borne)?1:0 );
  if (strcmp(op,"<>") == 0) 
    return ( (val!=borne)?1:0 );
  if (strcmp(op,"<") == 0) 
    return ( (val<borne)?1:0 );
  if (strcmp(op,"<=") == 0) 
    return ( (val<=borne)?1:0 );
  if (strcmp(op,">") == 0) 
    return ( (val>borne)?1:0 );
  if (strcmp(op,">=") == 0) 
    return ( (val>=borne)?1:0 );
  
  return -1;
}



static char * NOTop (char * op) {
  char *ret;
  if (strcmp(op,">")==0) {
    ret=malloc(strlen("<=")+1);
    strcpy(ret,"<=");
    MYFREE(op);
    return ret;
  } else if (strcmp(op,"<")==0) {
    ret=malloc(strlen(">=")+1);
    strcpy(ret,">=");
    MYFREE(op);
    return ret;
  } else if (strcmp(op,"=")==0) {
    return op;
  } else if (strcmp(op,"<=")==0) {
    ret=malloc(strlen(">")+1);
    strcpy(ret,">");
    MYFREE(op);
    return ret;
  } else if (strcmp(op,">=")==0) {
    ret=malloc(strlen("<")+1);
    strcpy(ret,"<");
    MYFREE(op);
    return ret;
  } else {
    fprintf(stderr," Unknown comparison operator %s!!\n",op);
    FkExit(kFk_EndOnError);
    return NULL; /*pour le compilo */
  }
}
