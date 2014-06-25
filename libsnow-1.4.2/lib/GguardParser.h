/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_GGUARD_GGUARDPARSER_H_INCLUDED
# define YY_GGUARD_GGUARDPARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int gguarddebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IN = 258,
    INC = 259,
    DEC = 260,
    D = 261,
    NOT = 262,
    AND = 263,
    OR = 264,
    VIRG = 265,
    CRO_O = 266,
    CRO_F = 267,
    PAR_O = 268,
    PAR_F = 269,
    ESCL = 270,
    CARET = 271,
    ACC_O = 272,
    ACC_F = 273,
    EGAL = 274,
    INF = 275,
    SUP = 276,
    INF_E = 277,
    SUP_E = 278,
    N_EGAL = 279,
    T_TRUE = 280,
    T_FALSE = 281,
    IDENT = 282,
    NBR_SS = 283
  };
#endif
/* Tokens.  */
#define IN 258
#define INC 259
#define DEC 260
#define D 261
#define NOT 262
#define AND 263
#define OR 264
#define VIRG 265
#define CRO_O 266
#define CRO_F 267
#define PAR_O 268
#define PAR_F 269
#define ESCL 270
#define CARET 271
#define ACC_O 272
#define ACC_F 273
#define EGAL 274
#define INF 275
#define SUP 276
#define INF_E 277
#define SUP_E 278
#define N_EGAL 279
#define T_TRUE 280
#define T_FALSE 281
#define IDENT 282
#define NBR_SS 283

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 85 "GguardParser.yxx" /* yacc.c:1909  */

  char	*chaine;
  int	entier;
  class Guard * guard;
  pair<class Variable*,int> *variable;
  list<string> * elements;

#line 118 "GguardParser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE gguardlval;

int gguardparse (void);

#endif /* !YY_GGUARD_GGUARDPARSER_H_INCLUDED  */
