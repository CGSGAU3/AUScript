#pragma once

#include "qust.h"
#include "token.h"

/* Scanner class */
class Scanner
{
  Queue<Tok> scanned; // Queue to write result

public:

  /* Dummy constructor */
  Scanner( void )
  {
  }

  /* Constructor-autoscanner */
  explicit Scanner( const std::string &str );

  /* Get result method */
  Queue<Tok> getQueue( void ) const
  {
    return scanned;
  }

  /* Clear method */
  void clear( void )
  {
    scanned.clear();
  }
};