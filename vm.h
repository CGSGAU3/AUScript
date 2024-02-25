#pragma once

#include "calc.h"

/* Different types of command */
enum struct CommandID
{
  EXPR,     // for common expressions
  IF,       // if/else statement
  WHILE,    // while loop
  ECHO,     // echo command
  COMPOSITE // for commands in {}
};

/* Addon function to split command between () or {} */
std::string parseBetween( Queue<Tok> &tokList, const char last );

/* Command structure */
struct Command
{
  CommandID id;        // type of the command
  Calculator expr;     // for expressions
  Command *addon1,     // for if statement & while
          *addon2;     // for else statement
  std::string echoStr; // for echo

  /* For composite commands */
  std::vector<Command *> nestedCommand;

  /* Token list reference to convenient parse */
  Queue<Tok> &tokList;

  /* Default initialization */
  Command( Queue<Tok> &input ) : id(CommandID::EXPR),
    addon1(nullptr), addon2(nullptr), tokList(input), 
    echoStr("")
  {
  }

  /* Self-parse method */
  bool parse( void );

  /* Run method */
  void run( void ) const;
};

/* Interpreter class */
class Interpreter
{
  Queue<Command *> cmdList; // List of commands
  Queue<Tok> tokList;       // List of all tokens

public:
  /* Constructor from file */
  Interpreter( const std::string &fileName );

  /* All copy & move features are forbidden */
  Interpreter( const Interpreter &other ) = delete;
  Interpreter( Interpreter &&other ) noexcept = delete;
  Interpreter & operator =( const Interpreter &other ) = delete;
  Interpreter & operator =( Interpreter &&other ) noexcept = delete;

  /* Run function */
  void run( void );

  /* Destructor */
  ~Interpreter( void );
};