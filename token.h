#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>

/* Token forward declaration */
struct Tok;

/* Tokens are different */
enum struct TokID
{
  OP,   // operators ex. + - * 
  NUM,  // numbers
  VAR,  // for variables
  KEYW, // keywords ex. if/else/while
  SPEC, // special symbols: { } ;
};

/* Keywords also different */
enum struct Keyword
{
  IF,   // if/else statement
  ELSE, // if/else statement
  WHILE // while loop

  /* Note. there is no for loop, because it harder */
};

/* Operator type & associativity types */
enum struct OperType
{
  INFIX,  // ex. +-/*
  PREFIX, // ex. unary -
  POSTFIX // ex. ',' or ')'
};

enum struct OperAssocType
{
  LEFT, // for almost everything
  RIGHT // for '=' or ','
};

/* Definitions of priotities */
enum struct Prior
{
  OPENBR,   // (
  CLOSEBR,  // )
  ZPT,      // ,
  ASSIGN,   // = and all = family
  BOOLOR,   // ||
  BOOLAND,  // &&
  EQUALNOT, // == !=
  LESSMORE, // < <= > >=
  PLUSMIN,  // + -
  MULDIV,   // * / %
  UNARYOP,  // unary - or unary ! or ++/--
  IOSQRT,   // print/scan ^ #
  SINCOS,   // sin/cos/etc.
};

/* Operator structure */
struct Oper
{
  /* Operator name */
  std::string name;

  /* Callback to operator */
  std::function<Tok( Tok &, const Tok & )> doFunc;

  OperType type;           // operator type
  OperAssocType assocType; // operator associativity type
  Prior prior;               // operator priority
};

/* Token structure */
struct Tok
{
  TokID id;         // ID to guess
  Oper op;          // for operators
  double num;       // for numbers
  std::string name; // for variables
  char symbol;      // for special symbols
  Keyword keyw;     // for keywords

  /* Static fields to use operators in convenient way */
  static std::vector<Oper> opers;
  static std::map<std::string, double> varTree;
  static Oper & findOper( const std::string &name );
};

/* Output token */
std::ostream & operator <<( std::ostream &stream, const Tok &token );