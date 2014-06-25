/*
	File:		FKServVersions.h

	Contains:	libPetri version 1.0.32 (Release Candidate)
	Copyright:	© 2002 by Mars Team.

	Owned by:	Yann Thierry-Mieg, yann.thierry-mieg@lip6.fr

	Change History (most recent first):
	10/02/2003 : version 1.0.32 : Added support for inhibitor arcs + minor rewriting in Element class for std::rel_ops problems
	03/12/2002 : version 1.0.2 (RC) : finalized export to gspn, getAssymetry on all elements, debug, major revisions with introduction of ParserWrapper. See README.txt for more details on the structure of the lib. 
	16/03/2002 : version 0.99 (alpha): export to gspn and subclass calculation routines
	08/03/2002 : version 0.98 (alpha): export to cami format and Macao
	13/02/2002 : version 0.97 (alpha): basic data structures for PNet manipulation + IVM Matrice format parser

*/
#ifndef _FKServVersions_
#define	_FKServVersions_

/*************************************/
/* Change the four following strings */
/*************************************/
const char kFk_KitName[]		="LibPetri";
const char kFk_ToolName[]		="Library";
const char kFk_ToolVersion[]		="1.0.32";
const char kFk_ToolCopyright[]	="Yann Thierry-Mieg 2002";

#endif
