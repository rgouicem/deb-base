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

#ifndef YY_MARK_MARKPARSER_H_INCLUDED
# define YY_MARK_MARKPARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int markdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ALL = 258,
    VIRG = 259,
    PLUS = 260,
    MOINS = 261,
    POINT = 262,
    INC = 263,
    MULT = 264,
    INF = 265,
    SUP = 266,
    IDENT = 267,
    NBR_SS = 268
  };
#endif
/* Tokens.  */
#define ALL 258
#define VIRG 259
#define PLUS 260
#define MOINS 261
#define POINT 262
#define INC 263
#define MULT 264
#define INF 265
#define SUP 266
#define IDENT 267
#define NBR_SS 268

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 90 "MarkParser.yxx" /* yacc.c:1909  */

  char	*chaine;
  int	entier;
  class Marking * marking;
  class Mark * mark;
  list<vector<Element> > * lelts;
  vector<Element> * elements;

#line 89 "MarkParser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE marklval;

int markparse (void);

#endif /* !YY_MARK_MARKPARSER_H_INCLUDED  */
