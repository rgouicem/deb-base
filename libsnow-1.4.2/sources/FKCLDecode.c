/****************************************************************************************
 *
 *	Module 		: TraiteCami.c -> TraiteCamiANSI.c -> FKCLDecode.c
 *	Auteur		: Bonnaire Xavier
 *	Date		: 08/04/92
 *	Sujet		: Decoder et traiter les commandes CAMI
 *	Date		: 08/07/94
 *						Renomme TraiteCamiANSI.c Ajout de nouvelles commandes CAMI bonnaire
 *	Date		: 27/02/96
 *						Renomme FKCLDecode.c  transformations pour FrameKit jlm
 *
 ****************************************************************************************/
#ifndef _OLD_AMI_INTERFACE_
#define _OLD_AMI_INTERFACE_
#endif

#include "FKCLDecode.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


#define kFk_CAMI_CommandSize 1024
#define UNKNOWN_CAMI	-2

#define SEPARATORS	",:)"





struct IdentCAMI {
	const char *commande;
	int code;
	TraiteC fonction;
	};
	
struct ErrorCAMI {
	int code;
	char texte[80];
	};

/* Les variables globales */

struct IdentCAMI commandeCAMI[]= {
			{"DUMY", CAMI_OK, NULL},
			{"DB",	CAMI_DB, NULL},
			{"CN",	CAMI_CN, NULL},
			{"CT",	CAMI_CT, NULL},
			{"CM",	CAMI_CM, NULL},
			{"CA",	CAMI_CA, NULL},
			{"FB",	CAMI_FB, NULL},
			{"CS",	CAMI_CS, NULL},
			{"AS",	CAMI_AS, NULL},
			{"DT",	CAMI_DT, NULL},
			{"FT",	CAMI_FT, NULL},
			{"DR",	CAMI_DR, NULL},
			{"DE",	CAMI_DE, NULL},
			{"FR",	CAMI_FR, NULL},
			{"RO",	CAMI_RO, NULL},
			{"XA",	CAMI_XA, NULL},
			{"FE",	CAMI_FE, NULL},
			{"RT",	CAMI_RT, NULL},
			{"TR",	CAMI_TR, NULL},
			{"DP",	CAMI_DP, NULL},
			{"RD",	CAMI_RD, NULL},
			{"DS", 	CAMI_DS, NULL},
			{"DN", 	CAMI_DN, NULL},
			{"TQ",	CAMI_TQ, NULL},
			{"SU",	CAMI_SU, NULL},
			{"GA",	CAMI_GA, NULL},
			{"Ok",	CAMI_Ok, NULL},
			{"Vs", 	CAMI_VS, NULL},	/* Demande de validation semantique de Desir vers Sandrine */
			{"En",	CAMI_En, NULL},	/* End dans les protocole Desir */
			{"FP",	CAMI_FP, NULL},
			{"CP",	CAMI_CP, NULL},
			{"AP", 	CAMI_AP, NULL},
			{"CB", 	CAMI_CB, NULL},
			{"CD",	CAMI_CD, NULL},
			{"Ex",	CAMI_Ex, NULL},
			{"QO",	CAMI_QO, NULL},
			{"QT",	CAMI_QT, NULL},
			{"ST",	CAMI_ST, NULL},
			{"PO", 	CAMI_PO, NULL},
			{"PT", 	CAMI_PT, NULL},
			{"Rn",	CAMI_Rn, NULL},
			{"Er",	CAMI_Er, NULL},
			{"MI",	CAMI_MI, NULL},
			{"MU",	CAMI_MU, NULL},
			{"WN",	CAMI_WN, NULL},
			{NULL,	-1 , NULL}};
			
struct ErrorCAMI erreurCAMI[]= {
			{-1000,		"Unknown Error"},
			{-1000,		"Unknown Error"},
			{CAMI_ERR,	"Erreur CAMI"},
			{CAMI_CM_OPER,	"Operation inconnue dans une commande CM"},
			{CAMI_UNKNOWN,	"Commande CAMI inconnue"},
			{0,		""/*NULL*/ }};
			
static	int 		g_fkParamc;
static	char 	 *g_fkParamCAMI[ MAX_CAMI_PARAM ];

static	char   *g_fkIndexGet=NULL;
static	char  	g_fkBufferText[kFk_CAMI_CommandSize];


/* Les fonctions de service */
#ifdef FRAMEKIT_SUPPORT
#include	"FKEnv.h"			/* FkOldSVimpression */
#include	"FKCLSend.h"	/* kFk_CAMI_CommandSize */
#else
void FkOldSVimpression(const char * c) { printf("%s\n",c); }
void FkTrace(const char * c) { printf("%s\n",c); }
#endif

/*============================================================================================
	Initialisation du module de decodage des comandes CAMI 
============================================================================================*/

void FkClInitDecodeCami( void )
{
 int i;
 
 for ( i=1; i<MAX_CAMI_PARAM; i++ ) g_fkParamCAMI[i] = NULL;
 g_fkParamCAMI[0] = (char *)malloc(5);
 g_fkParamc = CAMI_OK;
 }
 
/*============================================================================================
	Initialisation des fonctions de traitement des commandes par l'application
============================================================================================*/

void FkClInitialiseFonctions( TraiteC f[] )
{
 int i;
 
 for ( i=CAMI_OK + 1; i < CAMI_FIN; i++ ) commandeCAMI[i].fonction = f[i];
 }
 
/*============================================================================================
	Changement d'une des fonctions de traitement 
============================================================================================*/
 
void FkClChangerFonction( int cmd, TraiteC f )
{
 commandeCAMI[cmd].fonction = f;
 }

/*======= Decodage d'une chaine de caracteres dans une commande ===============*/

static char * LocalGetString( char *chaine )
{
 char *ch;
 int l,val;
 
 if ( chaine == NULL ) 
   ch = strtok( g_fkIndexGet, ":,");
 else ch = strtok( chaine, ":," );
 
 if ( ch != NULL )
 	{
    if ((ch[0] == ',') || (ch[0] == ')') ) 
			{
 			g_fkIndexGet = ch + 1;
			return( NULL );
			}
   }
   else {
 	 g_fkIndexGet = ch + 1;
 	 return( NULL );
 	 }
 if (ch[0] == '0' )
 	{
 	g_fkBufferText[0]='\0';
 	return (g_fkBufferText);	/* au lieu d'un return(""); */
 	}
 l = strlen(ch);
 val = atoi(ch);
 ch[l]=':';
 strncpy( g_fkBufferText, ch+l+1, val);
 g_fkBufferText[val]='\0';
 g_fkIndexGet = ch+l+val+2;
 return( g_fkBufferText );
 } 

/*====== Decode un entier dans une commande =============================*/

static char *GetInteger( char *chaine )
{
 char *ch;
 
 if ( chaine == NULL ) ch = strtok( g_fkIndexGet, ",)" );
  else ch = strtok( chaine, ",)" );
 if ( ch != NULL )
 	{
    if ((ch[0] == ',') || (ch[0] == ')') ) 
			{
			g_fkIndexGet = ch + 1;
			return( NULL );
			}
  }
   else {
 	 g_fkIndexGet = ch + 1;
 	 return( NULL );
 	 }
 g_fkIndexGet = ch + strlen(ch)+1; 
 return( ch );
 }

/*============================================================================================
	Decodage de la commande Debut Question
============================================================================================*/

static int DecoderCommandeDT( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "DT" );
 return( 0 );
 }
 
/*============================================================================================
	Decodage de la commande Fin Question
 ============================================================================================*/

static int DecoderCommandeFT( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "FT" );
 return( 0 );
}

/*============================================================================================
	Decodage de la commande Fin Reponse Dialogue
 ============================================================================================*/

static int DecoderCommandeFP( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "FT" );
 return( 0 );
} 
/*============================================================================================
	Decodage de la commande Debut Batch
============================================================================================*/

static int DecoderCommandeDB( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "DB" );
 return( 0 );
 }

/*============================================================================================
	Decodage de la commande Creer Noeud
============================================================================================*/

static int DecoderCommandeCN( char *commande )
{
 char *chaine;
 char *chTemp;
 
 chaine = commande + 3;   /* pas tres beau mais efficace */
 
 chTemp = LocalGetString( chaine );
 if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
 if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "CN" ); 
 return( 2 );
 }
 
/*====== Decodage de la commande ST ===================================================*/

static int DecoderCommandeST( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;   /* pas tres beau mais efficace */
 
 chTemp = LocalGetString( chaine );
 if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 	  
 chTemp = GetInteger( NULL );
 if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
  
 strcpy( g_fkParamCAMI[0], "ST" ); 
 return( 2 );
}
 
/*============================================================================================
	Decodage de la commande Creer Texte
============================================================================================*/
 
static int DecoderCommandeCT( char *commande )
 {
  char *chTemp, *chaine;
  
  chaine = commande + 3;	/* pas tres beau mais efficace */
  
  chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
  
 chTemp = GetInteger( NULL );
 if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
  
  chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else { 
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
 	  
  
  
  strcpy( g_fkParamCAMI[0], "CT" );
  return( 3 );
 }


 
static int DecoderCommandePT( char *commande )
 {
  char *chTemp, *chaine;
  
  chaine = commande + 3;	/* pas tres beau mais efficace */

 chTemp = GetInteger( chaine );
 if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
  
  chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else { 
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
  
 chTemp = GetInteger( NULL );
 if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
  
  chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[4] = NULL;
    else { 
 	  g_fkParamCAMI[4] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[4], chTemp );
 	  }
 	  
  
  
  strcpy( g_fkParamCAMI[0], "CT" );
  return( 3 );
 }
 
/*============================================================================================
	Decodage de la commande Creer Arc
============================================================================================*/
 
static int DecoderCommandeCA( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
  chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }  
 
  chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
  
  chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[4] = NULL;
    else {
 	  g_fkParamCAMI[4] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[4], chTemp );
 	  }
  
  strcpy( g_fkParamCAMI[0], "CA" );
 
  return( 4 );
 }
 
/*============================================================================================
	Decodage de la commande Fin Batch
============================================================================================*/
 
static int DecoderCommandeFB( char *commande )
 {
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
  strcpy( g_fkParamCAMI[0], "FB" );
  return( 0 );
  }
  
/*============================================================================================
	Decodage de la commande Creer Multiple
============================================================================================*/
  
static int DecoderCommandeCM( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;		/* pas beau mais efficace */
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[4] = NULL;
    else {
 	  g_fkParamCAMI[4] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[4], chTemp );
 	  }
  
/* action = atoi( chTemp );*/
/*
 switch( atoi( chTemp ) )
 	{
 	 case add		: 
 	 case insert		: 
 	 			  chTemp = LocalGetString( NULL );
  				    if ( chTemp == NULL ) g_fkParamCAMI[5] = NULL;
    					else { 
 	  					g_fkParamCAMI[5] = (char *)malloc( strlen(chTemp)+1 );
 	  					strcpy( g_fkParamCAMI[5], chTemp );
 	  					}
 	 			  
 	 			  strcpy( g_fkParamCAMI[0], "CM" );
 	 			  return(5);
 	 			  break;
 	 			  
 	 case supress		: g_fkParamCAMI[5] = (char *)malloc( 1 );
 	 			  g_fkParamCAMI[5][0]='\0';
 	 			  strcpy( g_fkParamCAMI[0], "CM" );
 	 			  return(5);
 	 			  break;
 	 			  
 	 default 		: return( CAMI_CM_OPER );
 	 			  break;
 	 }
*/
  chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[5] = NULL;
  else { 
    g_fkParamCAMI[5] = (char *)malloc( strlen(chTemp)+1 );
    strcpy( g_fkParamCAMI[5], chTemp );
  }
  strcpy( g_fkParamCAMI[0], "CM" );
  return(5);
 }
 
/*============================================================================================
	Decodage de la commande Creer Sommet
============================================================================================*/
 
static int DecoderCommandeCS( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;		/* pas beau mais efficace */
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
 
 chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[4] = NULL;
    else { 
 	  g_fkParamCAMI[4] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[4], chTemp );
 	  }
 	  
 strcpy(g_fkParamCAMI[0], "CS");
 	  
 return(4);
 	
 }
 
/*============================================================================================
	Decodage de la commande Associer Sommet
============================================================================================*/
 
static int DecoderCommandeAS( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;		/* pas beau mais efficace */
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
 
 chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[4] = NULL;
    else {
 	  g_fkParamCAMI[4] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[4], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "AS" );
 return( 4 );
 }

/*====== decodage de la commande DR ===================================================*/

static int DecoderCommandeDR( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "DR" );
 return( 0 );
 }
 
/*====== decodage de la commande DE ====================================================*/

static int DecoderCommandeDE( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "DE" );
 return( 0 );
 }

/*====== decodage de la commande FE ================================================*/

static int DecoderCommandeFE( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "FE" );
 return( 0 );
 }

/*====== decodage de la commande FR ============================================*/

static int DecoderCommandeFR( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "FR" );
 return( 0 );
 }
 
/*====== Decodage de la commande RO ============================================*/

static int DecoderCommandeRO( char *commande )
{
 char *chTemp, *chaine;
 
 chaine = commande + 3;
 
 
  chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 strcpy( g_fkParamCAMI[0], "RO" );
 return( 1 );
 }
 
/*====== decodage de la commande XA ================================================*/

static int DecoderCommandeXA( char *commande )
{
 char *chTemp, *chaine;
 
 chaine = commande + 3;
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else { 
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }

chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else { 
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
 	  
strcpy( g_fkParamCAMI[0], "XA" );
return( 3 );
}
 
/*===== decodage de RT ===========================================*/

static int DecoderCommandeRT( char *commande )
{
 char *chaine;
 char *chTemp;
 
 chaine = commande + 3;
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 	  
 strcpy( g_fkParamCAMI[0], "RT" );
 return( 1 );
 }
 
/*====== decodage de la commande TR =======================================*/

static int DecoderCommandeTR( char *commande )
{
 char *chaine;
 char *chTemp;
 
 chaine = commande + 3;
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 	  
 strcpy( g_fkParamCAMI[0], "TR" );
 return( 1 );
 }
 
/*====== Decodage de la commande DP =============================================*/

static int DecoderCommandeDP( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "DP" );
 return( 0 );
 }
 
/*====== Decodage de la commande RD ===========================================*/

static int DecoderCommandeRD( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
 
 chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[4] = NULL;
    else { 
 	  g_fkParamCAMI[4] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[4], chTemp );
 	  }
 	  
 return(4);
   }
  
/*====== Decodage de la commande DS =================================================*/

static int DecoderCommandeDS( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else { 
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 	  
  strcpy( g_fkParamCAMI[0], "DS" );
  return( 2 );
  }
  
/*====== Decodage de la commande DN ===============================================*/

static int DecoderCommandeDN( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "DN" );
 return( 2 );
 }
 
/*====== Decodage de la commande SU ================================================*/

static int DecoderCommandeSU( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "SU" );
 return( 1 );
 }
 
/*====== Decodage de la commande TQ =================================================*/ 

static int DecoderCommandeTQ( char *commande )
{
 char *chaine, *chTemp ;
 
 chaine = commande + 3;
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
    	 
  chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else { 
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 	  
  chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
   
  chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[4] = NULL;
    else { 
 	  g_fkParamCAMI[4] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[4], chTemp );
 	  }
    	 
   strcpy( g_fkParamCAMI[0], "TQ" );
   return( 4 );
   }
   
/*====== Decodage de la commande Ok ==========================================*/

static int DecoderCommandeOk( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "Ok" );
 return( 0 );
 }
 
/*====== Decodage de la commande En =========================================*/

static int DecoderCommandeEn( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "En" );
 return( 0 );
 }
 
/*====== Decodage de la commande GA =======================================*/

static int DecoderCommandeGA( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
  chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "GA" );
 return(2);
 }
/*====== Decodage de la commande CP =====================================*/
#if 0
static int DecoderCommandeCP( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "CP" );
 return(2);
 }
#endif
 /*====== Decodage de la commande AP =====================================*/
#if 0
static int DecoderCommandeAP( char *commande )
{
char *chaine, *chTemp;

chaine = commande + 3;

chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }

chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }

chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }

strcpy( g_fkParamCAMI[0], "AP" );
return( 3 );
}
#endif
/*====== Decodage de la commande CB ======================================*/
#if 0
static int DecoderCommandeCB( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "CB" );
 return( 3 );
 }
#endif 
/*====== Decodage de la commande CD =================================*/

static int DecoderCommandeCD( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "CD" );
 return( 1 );
 }
 
 
 

/*====== Decodage de la commande VS =====================================*/

static int DecoderCommandeVS( char *commande )
{
#ifdef	FK_MAC_OS
#pragma unused(commande)
#endif
 strcpy( g_fkParamCAMI[0], "VS" );
 return(0);
 }
  
 
/*====== decodage de la commande Ex =====================================*/

static int DecoderCommandeEx( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else { 
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else { 
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "Ex" );
 return( 3 );
 }
 
/*======= decodage de la commande QO =======================*/

static int DecoderCommandeQO( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "QO" );
 return(1);
 }
 
/*====== decodage de la commande QT =========================*/

static int DecoderCommandeQT( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3;
 
 chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "QT" );
 return( 1 );
 }
 
/*====== decodage de la commande PO =============================================*/

static int DecoderCommandePO( char *commande )
{
 int	l;
 char *chaine, *chTemp;
 
 chaine = commande + 3; 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 l = atoi( chTemp );
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
  chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[3] = NULL;
    else {
 	  g_fkParamCAMI[3] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[3], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "PO" );
 
 if ( l == -1 )
 	{
 	chTemp = GetInteger( NULL );
  	if ( chTemp == NULL ) g_fkParamCAMI[4] = NULL;
    	else {
 	  	g_fkParamCAMI[4] = (char *)malloc( strlen(chTemp)+1 );
 	  	strcpy( g_fkParamCAMI[4], chTemp );
 	  	}
 	 
 	chTemp = GetInteger( NULL );
  	if ( chTemp == NULL ) g_fkParamCAMI[5] = NULL;
    	else {
 	  	g_fkParamCAMI[5] = (char *)malloc( strlen(chTemp)+1 );
 	  	strcpy( g_fkParamCAMI[5], chTemp );
 	  	}
 	 
 	chTemp = GetInteger( NULL );
  	if ( chTemp == NULL ) g_fkParamCAMI[6] = NULL;
    	else {
 	  	g_fkParamCAMI[6] = (char *)malloc( strlen(chTemp)+1 );
 	  	strcpy( g_fkParamCAMI[6], chTemp );
 	  	}
 	 
 	 return(6);
 	 }
  else {
        return(3);
        }
       
 }
 
/*======= decodage de la commande Rn ===========================*/
static int DecoderCommandeRn( char *commande )
{
 char *chaine, *chTemp;
 
 chaine = commande + 3; 
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 
 chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
 
 strcpy( g_fkParamCAMI[0], "Rn" );
 return(2);
 }
 
/*====== decodage de la commande Er SANDRINE <--> DESIR ==================*/

static int DecoderCommandeEr( char *commande )
 {
  char *chTemp, *chaine;
  
  chaine = commande + 3;	/* pas tres beau mais efficace */
  
  chTemp = LocalGetString( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else { 
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
  
  chTemp = GetInteger( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }
  
  strcpy( g_fkParamCAMI[0], "Er" );
  return( 2 );
 }
 
/*====== decodage de la commande MI =====================*/

static int DecoderCommandeMI( char *commande )
{
 char *chTemp, *chaine;
 
 chaine = commande + 3;	/* pas tres beau mais efficace */
 
 chTemp = GetInteger( chaine );
  if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
 	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[1], chTemp );
 	  }
 	  
 chTemp = LocalGetString( NULL );
  if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else { 
 	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp)+1 );
 	  strcpy( g_fkParamCAMI[2], chTemp );
 	  }	  
  strcpy( g_fkParamCAMI[0], "MI" );
  return(2);
}  

/*====== Decodage de la commande MU =========================*/

static int DecoderCommandeMU( char *commande )
{
 char *chaine , *chTemp;
 
 chaine = commande + 3;
 
 chTemp = GetInteger( chaine );
 if ( chTemp == NULL ) g_fkParamCAMI[1] = NULL;
    else {
     	  g_fkParamCAMI[1] = (char *)malloc( strlen(chTemp) + 1);
     	  strcpy( g_fkParamCAMI[1], chTemp );
     	  }
     	  
 chTemp = GetInteger( chaine );
 if ( chTemp == NULL ) g_fkParamCAMI[2] = NULL;
    else {
     	  g_fkParamCAMI[2] = (char *)malloc( strlen(chTemp) + 1);
     	  strcpy( g_fkParamCAMI[2], chTemp );
     	  }
 strcpy( g_fkParamCAMI[0], "MU" );
 return(2);
 }

/*============================================================================================
	Fonction principale de decodage des commandes CAMI
============================================================================================*/

int FkClDecodeCami(const char *com, int *count, char **tab[] )
{
 int longueur;
 char cmd[3];
 int codeCAMI, i;
 int result;
 char *commande;
 int	mallocSize;
 
 cmd[2] = '\0';
 if (com == NULL )
 		{
		FkOldSVimpression("FkClDecodeCami:CAMI command NULL");
 		return(CAMI_ERR);
 		}
 longueur = strlen(com);
 if (longueur <4)
 		{
		FkOldSVimpression("FkClDecodeCami:CAMI command size < 4");
 		return(CAMI_ERR);
 		}
 mallocSize=longueur + 1;
 commande = (char *)malloc(mallocSize);
 if (commande==NULL) { FkTrace("DecodeCami:malloc commande==NULL"); return(CAMI_ERR); }
 strcpy( commande, com );
 strncpy( cmd, commande, 2);
 codeCAMI = -1;
 i=0;
 do {
 	if ( strcmp( cmd, commandeCAMI[i].commande ) == 0 ) 
 		{
 		 codeCAMI = commandeCAMI[i].code;
 		 break;
 		 }
 	i++;
 	} while ( commandeCAMI[i].commande != NULL );
 	
 if ( codeCAMI == -1 )
		{
		char	tmp[kFk_CAMI_CommandSize*2]; /* Pour pouvoir mettre une commande CAMI dedans ! */
    sprintf(tmp, "FkClDecodeCami:Unknown CAMI command %s", commande);
		FkOldSVimpression(tmp);
		return( CAMI_ERR );
		}
 
 for( i=CAMI_OK + 2; i < MAX_CAMI_PARAM; i++ ) 			/* Nettoyer la liste des paramettres */
 	if ( g_fkParamCAMI[i] != NULL )
 		{
 		 free( g_fkParamCAMI[i] );
 		 g_fkParamCAMI[i] = NULL;
 		 }
 *tab = g_fkParamCAMI;
 switch( codeCAMI )
 	{
 	 case CAMI_DT	: g_fkParamc = DecoderCommandeDT( commande );
 	 		  if (g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_DT ].fonction != NULL)
 	 		  	 	(*(commandeCAMI[ CAMI_DT ].fonction))( 0, NULL );
 	 		  	 }
 	 		  result = CAMI_DT;
 	 		  break;
 	 		  
 	 case CAMI_FT	: g_fkParamc = DecoderCommandeFT( commande );
 	 		  if (g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_FT ].fonction != NULL)
 	 		  	 	(*(commandeCAMI[ CAMI_FT ].fonction))( 0, NULL );
 	 		  	 }
 	 		  result = CAMI_FT;
 	 		  break;
 	 		  
 	 case CAMI_FP	: g_fkParamc = DecoderCommandeFP( commande );
 	 		  if (g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_FP ].fonction != NULL)
 	 		  	 	(*(commandeCAMI[ CAMI_FP ].fonction))( 0, NULL );
 	 		  	 }
 	 		  result = CAMI_FP;
 	 		  break;
 	 		  
 	 case CAMI_DB	: g_fkParamc = DecoderCommandeDB( commande );
 	 		  if (g_fkParamc >= CAMI_OK ) 
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_DB ].fonction != NULL)
 	 		  	 	 (*(commandeCAMI[ CAMI_DB ].fonction))( 0, NULL );
 	 		  	 }
 	 		  result = CAMI_DB ;
 	 		  break;
 	 		  
 	 case CAMI_CN	: g_fkParamc = DecoderCommandeCN( commande );
 	 		  if (g_fkParamc >= CAMI_OK ) 
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_CN ].fonction != NULL)
 	 		  	 	 (*(commandeCAMI[ CAMI_CN ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_CN;
  	 		  break;
  	 		  
 	 case CAMI_CT	: g_fkParamc = DecoderCommandeCT( commande );
 	 		  if ( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_CT ].fonction != NULL)
 	 		  	 	(*(commandeCAMI[ CAMI_CT ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_CT;
 	 		  break;
 	 		  
 	 case CAMI_CM	: g_fkParamc = DecoderCommandeCM( commande );
 	 		  if (g_fkParamc >= CAMI_OK ) 
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_CM ].fonction != NULL)
 	 		  	 	(*(commandeCAMI[ CAMI_CM ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_CM;
 	 		  break;
 	 case CAMI_CA	: g_fkParamc = DecoderCommandeCA( commande );
 	 		  if (g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_CA ].fonction != NULL)
 	 		  	 	(*(commandeCAMI[ CAMI_CA ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_CA;
 	 		  break;
 	 		  
 	 case CAMI_FB	: g_fkParamc = DecoderCommandeFB( commande );
 	 		  if (g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_FB ].fonction != NULL)
 	 		  	 	(*(commandeCAMI[ CAMI_FB ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_FB;
 	 		  break;
 	 		  
 	 case CAMI_CS	: g_fkParamc = DecoderCommandeCS( commande );
 	 		  if (g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_CS ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_CS ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_CS;
 	 		  break;
 	 		  
 	 case CAMI_AS	: g_fkParamc = DecoderCommandeAS( commande );
 	 		  if ( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	if (commandeCAMI[ CAMI_AS ].fonction != NULL )
 	 		  		(*(commandeCAMI[ CAMI_AS ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_AS;
 	 		  break;
 	 	
 	 case CAMI_DR	: g_fkParamc = DecoderCommandeDR( commande );
 	 		  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_DR ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_DR ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_DR;
 	 		  break;
 	 		  
 	 case CAMI_DE	: g_fkParamc = DecoderCommandeDE( commande );
 	 		  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_DE ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_DE ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_DE;
 	 		  break;
 	 		  
 	 case CAMI_FE	: g_fkParamc = DecoderCommandeFE( commande );
 	 		  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_FE ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_FE ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_FE;
 	 		  break;
 	 		  
 	 case CAMI_FR	: g_fkParamc = DecoderCommandeFR( commande );
 	 		  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_FR ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_FR ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_FR;
 	 		  break;
 	 		  
 	 case CAMI_RO	: g_fkParamc = DecoderCommandeRO( commande );
 	 		  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_RO ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_RO ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_RO;
 	 		  break;

 	 case CAMI_XA	: g_fkParamc = DecoderCommandeXA( commande );
 	 		  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_XA ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_XA ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_XA;
 	 		  break;
 	 		  
 	 case CAMI_RT	: g_fkParamc = DecoderCommandeRT( commande );
 	 		  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_RT ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_RT ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_RT;
 	 		  break;

	case CAMI_TR	: g_fkParamc = DecoderCommandeTR( commande );
			  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_TR ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_TR ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_TR;
 	 		  break;
 	 		  
 	case CAMI_DP	: g_fkParamc = DecoderCommandeDP( commande );
 			  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_DP ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_DP ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_DP;
 	 		  break;
 	 		  
 	case CAMI_RD	: g_fkParamc = DecoderCommandeRD( commande );
 			  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_RD ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_RD ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_RD;
 	 		  break;
 	 		  
 	case CAMI_DS	: g_fkParamc = DecoderCommandeDS( commande );
 			  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_DS ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_DS ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_DS;
 	 		  break;
 	 		  
 	case CAMI_DN	: g_fkParamc = DecoderCommandeDN( commande );
 			  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_DN ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_DN ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_DN;
 	 		  break;
 	 		  
 	case CAMI_TQ	: g_fkParamc = DecoderCommandeTQ( commande );
 			  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_TQ ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_TQ ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_TQ;
 	 		  break;
 	 		  
 	case CAMI_SU	: g_fkParamc = DecoderCommandeSU( commande );
 			  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_SU ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_SU ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_SU;
 	 		  break;
 	 		  
 	case CAMI_GA	: g_fkParamc = DecoderCommandeGA( commande );
		  	  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_GA ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_GA ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_GA;
 	 		  break;
 	 		  
 	case CAMI_VS	: g_fkParamc = DecoderCommandeVS( commande );
 			  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_VS ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_VS ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_VS;
 	 		  break;
 	 		  
 	case CAMI_Ok	: g_fkParamc = DecoderCommandeOk( commande );
 	 		  if( g_fkParamc >= CAMI_OK )
 	 		  	{
 	 		  	 if (commandeCAMI[ CAMI_Ok ].fonction != NULL )
 	 		  	 	(*(commandeCAMI[ CAMI_Ok ].fonction))( g_fkParamc, g_fkParamCAMI );
 	 		  	 }
 	 		  result = CAMI_Ok;
 	 		  break;
 	
 	case CAMI_En	: g_fkParamc = DecoderCommandeEn( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_En ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_En ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_En;
 			  break;
 			  
 	case CAMI_Ex	: g_fkParamc = DecoderCommandeEx( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_Ex ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_Ex ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_Ex;
 			  break;
 			  
 	case CAMI_QO	: g_fkParamc = DecoderCommandeQO( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_QO ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_QO ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_QO;
 			  break;
 			  
 	case CAMI_QT	: g_fkParamc = DecoderCommandeQT( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_QT ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_QT ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_QT;
 			  break;
 	case CAMI_ST	: g_fkParamc = DecoderCommandeST( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_ST ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_ST ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_ST;
 			  break;
 			  
 	case CAMI_PO	: g_fkParamc = DecoderCommandePO( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_PO ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_PO ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_PO;
 			  break;

 	case CAMI_PT	: g_fkParamc = DecoderCommandePT( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_PT ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_PT ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_PT;
 			  break;
 			  
 	case CAMI_Rn	: g_fkParamc = DecoderCommandeRn( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_Rn ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_Rn ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_Rn;
 			  break;
 			  
 	case CAMI_Er	: g_fkParamc = DecoderCommandeEr( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_Er ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_Er ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_Er;
 			  break;
 			  
 	case CAMI_CD	: g_fkParamc = DecoderCommandeCD( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_CD ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_CD ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_CD;
 			  break;

	case CAMI_MI	: g_fkParamc = DecoderCommandeMI( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_MI ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_MI ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_MI;
 			  break;
 			  
 	case CAMI_MU	: g_fkParamc = DecoderCommandeMU( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_MU ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_MU ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_MU;
 			  break;

 	case CAMI_WN	: g_fkParamc = DecoderCommandeMU( commande );
 			  if ( g_fkParamc >= CAMI_OK )
 			  	{
 			  	 if (commandeCAMI[ CAMI_WN ].fonction != NULL )
 			  	 	(*(commandeCAMI[ CAMI_WN ].fonction))( g_fkParamc, g_fkParamCAMI );
 			  	 }
 			  result = CAMI_WN;
 			  break;

	default	:
		result = CAMI_UNKNOWN;
		break;
 	 }
  *count = g_fkParamc;
  free( commande );
  return( result );
 }
/*-----------------------------------------------------------------------------------*/
