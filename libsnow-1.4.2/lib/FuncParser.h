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

#ifndef YY_FUNC_FUNCPARSER_H_INCLUDED
# define YY_FUNC_FUNCPARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int funcdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ALL = 258,
    VIRG = 259,
    POINT = 260,
    INC = 261,
    MULT = 262,
    PLUS = 263,
    MOINS = 264,
    INF = 265,
    SUP = 266,
    DEC = 267,
    IDENT = 268,
    NBR_SS = 269
  };
#endif
/* Tokens.  */
#define ALL 258
#define VIRG 259
#define POINT 260
#define INC 261
#define MULT 262
#define PLUS 263
#define MOINS 264
#define INF 265
#define SUP 266
#define DEC 267
#define IDENT 268
#define NBR_SS 269

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 89 "FuncParser.yxx" /* yacc.c:1909  */

  char	*chaine;
  int	entier;
  class CFunc * func;
  class CFuncTerm * functerm;
  list<CFuncElt > * lelts;
  CFuncElt * elements;

#line 91 "FuncParser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE funclval;

int funcparse (void);

#endif /* !YY_FUNC_FUNCPARSER_H_INCLUDED  */
