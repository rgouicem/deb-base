/*******************************************************************************
 *
 *	Module	: TraiteCami.h -> FKCLDecode.h
 *	Auteur	: Bonnaire Xavier
 *	Date		: 08/04/92
 *					: Decodage et traitement des commandes CAMI
 *	Date		: 27/02/96
 *					:	Renomme FKCLDecode.h  transformations pour FrameKit jlm
 *
 ********************************************************************************/
 
#ifndef _TRAITE_CAMI_H
#define _TRAITE_CAMI_H

typedef void ( *TraiteC)(int paramc, char *paramv[] );
typedef enum { add=1, supress=2, insert=3, add_end=4 } OperationCM;

/*======================================================================================
	Definition des constantes CAMI
======================================================================================*/

#define CAMI_DB		1	/* Debut Batch */
#define CAMI_CN		2	/* Creer Noeud */
#define CAMI_CT		3	/* Creer Texte */
#define CAMI_CM		4	/* Creer texte Multiple */
#define CAMI_CA		5	/* Creer Arc */
#define CAMI_FB		6	/* Fin Batch */
#define CAMI_CS		7	/* Creer Sommet */
#define CAMI_AS		8	/* Associer Sommet */
#define CAMI_DT		9	
#define CAMI_FT		10
#define CAMI_DR		11	/* Debut Resultat */
#define CAMI_DE		12	/* Debut Ensemble */
#define CAMI_FR		13	/* Fin Resultat */
#define CAMI_RO		14
#define CAMI_XA		15	/* Modifier Attribut */
#define CAMI_FE		16	/* Fin Ensemble */
#define CAMI_RT		17
#define CAMI_TR		18
#define CAMI_DP		19	/* Debut Reponse Dialogue */
#define CAMI_RD		20	/* Reponse Dialogue */
#define CAMI_DS		21	/* Dialogue Suite */
#define CAMI_DN		22	/* Dialogue Objet */
#define CAMI_TQ		23	/* Etat Question */
#define CAMI_SU		24	/* Supprimer Objet */
#define CAMI_GA		25	/* Generation arbre */
#define CAMI_Ok		26	/* Aucun problemes */
#define CAMI_VS		27	/* Validation */
#define CAMI_En		28	/* Fin de communication avec DESIR */
#define CAMI_FP		29	/* Fin Reponse Dialogue */
#define CAMI_CP		30	/* Creer Port */
#define CAMI_AP		31	/* Associer Port */
#define CAMI_CB		32	/* Creer Boite */
#define CAMI_CD		33	/* Changer  De boite */
#define CAMI_Ex		34	/* Executer action */
#define CAMI_QO		35	/* Question Objet */
#define CAMI_QT		36	/* Question Texte */
#define CAMI_ST		37	/* Supprimer Texte */
#define CAMI_PO		38	/* Positionner Objet */
#define CAMI_Rn		39	/* Reponse Numero message */
#define CAMI_Er		40	/* Erreur SANDRINE <--> DESIR */
#define CAMI_MI		41	/* Message Interactif */
#define CAMI_MU		42	/* Message Urgent */
#define CAMI_PT         43      /* Positionner tag */
#define CAMI_FIN	44
#define	CAMI_WN		45  /* Message de Warning 01/97 */

#define MAX_CAMI	CAMI_FIN - 1;

/* Macro de gestion des erreurs */

#define CAMI_ERROR(x)	CAMI_OK - x

#define CAMI_OK		-1
#define CAMI_ERR	CAMI_ERROR(1)
#define CAMI_CM_OPER	CAMI_ERROR(2)
#define CAMI_UNKNOWN	CAMI_ERROR(3)

#define MAX_CAMI_PARAM	10



/*======================================================================================
	Prototypes des fonctions de service
======================================================================================*/
#ifdef _OLD_AMI_INTERFACE_
#define InitDecodeCami()		FkClInitDecodeCami()
#define InitialiseFonctions(f)	FkClInitialiseFonctions(f)
#define ChangerFonction(cmd, f)	FkClChangerFonction(cmd, f)
#define DecodeCami(commande, count, tab)	FkClDecodeCami(commande, count, tab)
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern void 	FkClInitDecodeCami( void );
extern void 	FkClInitialiseFonctions( TraiteC f[] );
extern void 	FkClChangerFonction( int cmd, TraiteC f );
extern int 		FkClDecodeCami(const char *commande, int *count, char **tab[] );
#ifdef __cplusplus
}
#endif
#endif
