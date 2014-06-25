# What are build goals 
lib_LIBRARIES = libPNet.a
bin_PROGRAMS = cami2gspn cami2smart snow

#parsers : Lex Yacc
AM_YFLAGS = -d 
AM_LFLAGS = -olex.yy.c -i
BUILT_SOURCES =  FuncParser.h   GguardParser.h  \
	GcolParser.h   GmarkParser.h   MarkParser.h \
	GfuncParser.h  GuardParser.h   tobsParser.h
#	DeclarationParser.h

LYFILES = ../parsers/FuncLexer.ll  ../parsers/GfuncLexer.ll   ../parsers/GmarkLexer.ll  ../parsers/MarkLexer.ll \
	  ../parsers/GcolLexer.ll  ../parsers/GguardLexer.ll  ../parsers/GuardLexer.ll  ../parsers/tobsLexer.ll \
	../parsers/FuncParser.yxx  ../parsers/GfuncParser.yxx   ../parsers/GmarkParser.yxx  ../parsers/MarkParser.yxx \
	../parsers/GcolParser.yxx  ../parsers/GguardParser.yxx  ../parsers/GuardParser.yxx  ../parsers/tobsParser.yxx


CPPFILES = 	../sources/PNet.cpp  \
	../sources/Arcs.cpp  ../sources/Arc.cpp    ../sources/CFunc.cpp ../sources/CFuncTerm.cpp ../sources/CFuncElt.cpp  \
        ../sources/Domains.cpp  ../sources/Domain.cpp  ../sources/PNClasses.cpp  ../sources/PNClass.cpp ../sources/Element.cpp \
	../sources/Places.cpp  ../sources/Place.cpp    ../sources/Mark.cpp ../sources/Marking.cpp \
        ../sources/Transitions.cpp ../sources/Transition.cpp  ../sources/Guard.cpp \
	../sources/Variable.cpp ../sources/Variables.cpp \
	../sources/PNetIO.cpp ../sources/ParserCAMI.cpp   ../sources/LineParser.cpp ../sources/ParserWrapper.cpp \
	../sources/parsedef.cpp ../sources/parsenet.cpp  \
	../sources/CalcSubClasses.cpp ../sources/Position.cpp ../sources/FKCLDecode.c

HEADERFILES = ../interfaces/Arc.h          ../interfaces/Marking.h        ../interfaces/PNet.h \
	../interfaces/Arcs.h            ../interfaces/PNetIO.h \
	../interfaces/CalcSubClasses.h  ../interfaces/parsedef.h      \
	../interfaces/CFuncElt.h       ../interfaces/parsenet.h       ../interfaces/Position.h \
	../interfaces/CFunc.h          ../interfaces/ParserCAMI.h   ../interfaces/CFuncTerm.h   \
	    ../interfaces/Guard.h         ../interfaces/ParserWrapper.h  ../interfaces/Transition.h \
	../interfaces/Domain.h           ../interfaces/Place.h          ../interfaces/Transitions.h \
	../interfaces/Domains.h         ../interfaces/LineParser.h    ../interfaces/Places.h         ../interfaces/Variable.h \
	../interfaces/Element.h           ../interfaces/PNClasses.h      ../interfaces/Variables.h \
	../interfaces/Mark.h          ../interfaces/PNClass.h  ../interfaces/FKCLDecode.h


libPNet_a_SOURCES = $(CPPFILES) $(HEADERFILES) $(LYFILES)
cami2gspn_SOURCES = $(CPPFILES) $(HEADERFILES) $(LYFILES) ../sources/cami2gspn.cc
cami2smart_SOURCES = $(CPPFILES) $(HEADERFILES) $(LYFILES) ../sources/cami2smart.cc
snow_SOURCES = $(CPPFILES) $(HEADERFILES) $(LYFILES) ../sources/snow.cpp

# -g -Wextra

AM_CPPFLAGS = -I ../interfaces -Wall -O3

# ../sources/GuardParser.cpp ../sources/GuardLexer.cpp ../sources/GguardParser.cpp ../sources/GguardLexer.cpp ../sources/MarkParser.cpp ../sources/MarkLexer.cpp 
# ../sources/FuncParser.cpp ../sources/FuncLexer.cpp GfuncParser.cpp GfuncLexer.cpp GcolParser.cpp GcolLexer.cpp GmarkParser.cpp GmarkLexer.cpp tobsParser.cpp tobsLexer.cpp
