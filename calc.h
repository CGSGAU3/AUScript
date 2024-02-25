#pragma once

#include "token.h"
#include "scanner.h"
#include "parser.h"

/* Calculator class */
class Calculator
{
  Scanner scanner;  // Scanner to scan
  Parser parser;    // Parser to parse
  Queue<Tok> poliz; // POLIZ queue of tokens

public:
  /* Default constructor for vm */
  Calculator( void )
  {
  }

  /* Constructor-auto-scanner/parser */
  explicit Calculator( const std::string &str );

  /* Main calculator method */
  double eval( void ) const;
};