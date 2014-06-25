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

#ifndef YY_GFUNC_GFUNCPARSER_H_INCLUDED
# define YY_GFUNC_GFUNCPARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int gfuncdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUM = 258,
    COMMA = 259,
    PLUS = 260,
    MINUS = 261,
    S = 262,
    ESCL = 263,
    GT = 264,
    LT = 265,
    OBRCK = 266,
    CBRCK = 267,
    STRING = 268
  };
#endif
/* Tokens.  */
#define NUM 258
#define COMMA 259
#define PLUS 260
#define MINUS 261
#define S 262
#define ESCL 263
#define GT 264
#define LT 265
#define OBRCK 266
#define CBRCK 267
#define STRING 268

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 83 "GfuncParser.yxx" /* yacc.c:1909  */

  char	*chaine;
  int	entier;
  class CFunc * func;
  class CFuncTerm * functerm;
  list<CFuncElt > * lelts;
  CFuncElt * elements;

#line 89 "GfuncParser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE gfunclval;

int gfuncparse (void);

#endif /* !YY_GFUNC_GFUNCPARSER_H_INCLUDED  */
