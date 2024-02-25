#pragma once

#include "qust.h"
#include "token.h"

/* Parser class */
class Parser
{
  Queue<Tok> opQueue; // Queue of operands
  Stack<Tok> opStack; // Stack of operations
  Queue<Tok> source;  // Source queue

  /* State describes what we expect to meet */
  enum struct State
  {
    PREFIX, // number/variable/prefix operator
    SUFFIX, // infix operator/close bracket
    DONE,   // process closing bracket & return
  };

public:

  /* Dummy constructor */
  Parser( void )
  {
  }

  /* Constructor to write source queue */
  explicit Parser( const Queue<Tok> &src ) : source(src)
  {
  }

  /* Main parser method */
  Queue<Tok> & run( void );

  /* Clear method */
  void clear( void )
  {
    opQueue.clear();
    opStack.clear();
    source.clear();
  }
};