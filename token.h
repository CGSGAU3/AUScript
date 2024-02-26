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
  SPEC  // special symbols: { } ;
};

/* Keywords also different */
enum struct Keyword
{
  IF,    // if/else statement
  ELSE,  // if/else statement
  WHILE, // while loop
  FOR,   // for loop
  ECHO,  // raw print on screen
  ARRAY, // array declaration
};

/* Operator type & associativity types */
enum struct OperType
{
  INFIX,  // ex. +-/*
  PREFIX, // ex. unary -
  POSTFIX // ex. ')'
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
  OPENIDX,  // [
  CLOSEIDX, // ]
  PLUSMIN,  // + -
  MULDIV,   // * / %
  UNARYOP,  // unary - or unary ! or ++/--
  IOSQRT,   // print/scan ^ #
  SINCOS,   // sin/cos/etc.
  INDEX,    // []
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

/* Variable type enum */
enum struct VarType
{
  SINGLE, // for single variables
  ARRAY,  // for arrays
};

/* Variable structure */
struct Variable
{
  /* Variable type */
  VarType type = VarType::SINGLE;

  double num;              // single
  std::vector<double> arr; // arrays
  int usedIndex;           // let [] return lvalue
};

/* Token structure */
struct Tok
{
  TokID id;         // ID to guess
  Oper op;          // for operators
  Variable var;     // for numbers & arrays
  std::string name; // for variables
  char symbol;      // for special symbols
  Keyword keyw;     // for keywords

  /* Static fields to use operators in convenient way */
  static std::vector<Oper> opers;
  static std::map<std::string, Variable> varTree;
  static Oper & findOper( const std::string &name );
};

/* Output token */
std::ostream & operator <<( std::ostream &stream, const Tok &token );