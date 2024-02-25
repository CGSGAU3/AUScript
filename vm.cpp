#include <fstream>
#include <sstream>

#include "vm.h"

/* Addon function to split command between () or {} impl */
std::string parseBetween( Queue<Tok> &tokList, const char last )
{
  std::stringstream res;
  int count = 1;

  if (last != '}' && last != ')')
    throw std::invalid_argument("')' or '}' need in parseBetween!");

  TokID needID = (last == '}' ? TokID::SPEC : TokID::OP);
  Tok cur;
  while (count && tokList.get(&cur))
  {
    /* Skip another token types */
    if (cur.id != needID)
    {
      res << cur << ' ';
      continue;
    }

    /* Process () */
    if (last == ')')
    {
      if (cur.op.name == "(")
        count++;
      else if (cur.op.name == ")")
        count--;
    }

    /* Process {} */
    if (last == '}')
    {
      if (cur.symbol == '{')
        count++;
      else if (cur.symbol == '}')
        count--;
    }

    /* Add current token */
    if (count != 0)
      res << cur << ' ';
  }

  if (count != 0 && !tokList.get())
    throw "Unexpected end of block!";
  return res.str();
}

/* Command parse method */
bool Command::parse( void )
{
  Tok cur;
  bool isEndOfList = false;

  /* Go to next token function */
  auto nextTok = [&]( void )
  {
    if (isEndOfList)
      throw "Unexpected end of data!";
    if (!tokList.get(&cur))
      isEndOfList = true;
  };

  /* Obtain common expression string */
  auto getExpr = [&]( void )
  {
    std::stringstream res;

    /* Write current token */
    res << cur << ' ';

    while (tokList.get(&cur))
    {
      /* Fix thing with double */
      if (cur.id == TokID::NUM)
      {
        res << cur.name << ' ';
        continue;
      }

      /* Check type of token */
      if (cur.id != TokID::SPEC)
      {
        res << cur << ' ';
        continue;
      }

      /* Break condition */
      if (cur.symbol == ';')
        break;
    }

    if (cur.id != TokID::SPEC || cur.symbol != ';')
      throw "Missing ';'";

    return res.str();
  };

  try
  {
    /* Get first token of command */
    nextTok();

    if (isEndOfList)
      return false;

    /* Obtain type of the token */
    if (cur.id == TokID::KEYW)
    {
      if (cur.keyw == Keyword::IF)
      {
        /* Skip keyword */
        id = CommandID::IF;
        nextTok();

        /* Check '(' */
        if (cur.id != TokID::OP || cur.op.name != "(")
          throw "After if '(' expected!";

        /* Parse an if statement */
        std::string exprInIf = parseBetween(tokList, ')');
        expr = Calculator(exprInIf);

        /* Parse expression in if statement */
        addon1 = new Command(tokList);
        if (!addon1->parse())
          throw "Unexpected end of if block!";

        /* Parse else statement (optional) */
        if (tokList.front().id == TokID::KEYW &&
            tokList.front().keyw == Keyword::ELSE)
        {
          /* Skip it */
          nextTok();

          /* Parse else statement */
          addon2 = new Command(tokList);
          if (!addon2->parse())
            throw "Unexpected end of else block!";
        }
      }
      else if (cur.keyw == Keyword::WHILE)
      {
        /* Skip keyword */
        id = CommandID::WHILE;
        nextTok();

        /* Check '(' */
        if (cur.id != TokID::OP || cur.op.name != "(")
          throw "After if '(' expected!";

        /* Parse a while statement */
        std::string exprInWhile = parseBetween(tokList, ')');
        expr = Calculator(exprInWhile);

        /* Parse expression in while statement */
        addon1 = new Command(tokList);
        if (!addon1->parse())
          throw "Unexpected end of while block!";
      }
      else
        throw "Invalid else without paired if!";
    }
    else if (cur.id == TokID::SPEC && cur.symbol == '{')
    {
      id = CommandID::COMPOSITE;

      /* Parse nested command */
      while (tokList.front().id != TokID::SPEC ||
             tokList.front().symbol != '}')
      {
        Command *cmd = new Command(tokList);

        if (!cmd->parse())
          throw "Unexpected end of composite block!";
        nestedCommand.push_back(cmd);
      }

      /* And skip '}' */
      nextTok();
    }
    else
    {
      id = CommandID::EXPR;
      expr = Calculator(getExpr());
    }
  }
  catch ( const char *err )
  {
    throw err;
  }
  return true;
}

/* Command run method */
void Command::run( void ) const
{
  if (id == CommandID::EXPR)
    expr.eval();
  else if (id == CommandID::IF)
  {
    if ((bool)expr.eval())
      addon1->run();
    else
      addon2->run();
  }
  else if (id == CommandID::WHILE)
  {
    while ((bool)expr.eval())
      addon1->run();
  }
  else if (id == CommandID::COMPOSITE)
  {
    for (const auto &cmd : nestedCommand)
      cmd->run();
  }
}

/* Interpreter constructor */
Interpreter::Interpreter( const std::string &fileName )
{
  std::ifstream f(fileName);

  /* Check file */
  if (!f.is_open())
    throw "File not found!";

  std::string content = "", line;

  /* Read all file */
  while (std::getline(f, line))
    content += line;
  f.close();

  /* Scan zone */
  try
  {
    Scanner scanner(content);
    tokList = scanner.getQueue();
  }
  catch ( const char *err )
  {
    throw err;
  }
  catch (...)
  {
    throw "Unknown error in scan!";
  }

  /* Parse zone */
  try
  {
    while (true)
    {
      Command *cmd = new Command(tokList);

      if (!cmd->parse())
      {
        delete cmd;
        break;
      }
      cmdList.put(cmd);
    }
  }
  catch ( const char *err )
  {
    throw err;
  }
  catch (...)
  {
    throw "Unknown error in commands parse!";
  }
}

/* Clear command function */
void clearCmd( Command *cmd )
{
  if (cmd == nullptr)
    return;

  if (cmd->addon1 != nullptr)
    clearCmd(cmd->addon1);
  if (cmd->addon2 != nullptr)
    clearCmd(cmd->addon2);
  for (auto &nest : cmd->nestedCommand)
    clearCmd(nest);
  delete cmd;
}

/* Interpreter main function */
void Interpreter::run( void )
{
  Queue<Command *> copy = cmdList;
  Command *cur;

  try
  {
    while (copy.get(&cur))
      cur->run();
  }
  catch ( const char *err )
  {
    throw err;
  }
  catch ( const std::string &err )
  {
    throw err;
  }
  catch ( const std::exception &ex )
  {
    throw ex;
  }
  catch (...)
  {
    throw "Smt goes wrong in run...";
  }
}

/* Interpreter destructor */
Interpreter::~Interpreter( void )
{
  Command *cmd;

  while (cmdList.get(&cmd))
    clearCmd(cmd);
}