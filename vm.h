#pragma once

#include "calc.h"

/* Different types of command */
enum struct CommandID
{
  EXPR,      // for common expressions
  IF,        // if/else statement
  WHILE,     // while loop
  FOR,       // for loop
  BREAK,     // break statement
  CONTINUE,  // continue statement
  ECHO,      // echo command
  ARRAY,     // array declaration command
  COMPOSITE, // for commands in {}
};

/* Addon function to split command between () or {} */
std::string parseBetween( Queue<Tok> &tokList, const char last );

/* Command structure */
struct Command
{
  CommandID id;        // type of the command
  Calculator expr;     // for expressions
  Command *addon1,     // for if statement & while
          *addon2,     // for else statement
          *addon3;     // for "for" loop
  Command *loop;       // for loop management
  bool loopStopped;    // break flag for loops
  bool loopContinued;  // continue flag for loops
  std::string echoStr; // for echo & array name

  /* For composite commands */
  std::vector<Command *> nestedCommand;

  /* Token list reference to convenient parse */
  Queue<Tok> &tokList;

  /* Default initialization */
  Command( Queue<Tok> &input ) : id(CommandID::EXPR),
    addon1(nullptr), addon2(nullptr), addon3(nullptr),
    loop(nullptr), tokList(input), echoStr(""), 
    loopStopped(false), loopContinued(false)
  {
  }

  /* Self-parse method */
  bool parse( Command *cycle = nullptr );

  /* Run method */
  void run( void );
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

  /* Clear method */
  void clear( void );

  /* Destructor */
  ~Interpreter( void );
};