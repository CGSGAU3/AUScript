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

/* Clear command function */
void clearCmd( Command *cmd )
{
  if (cmd == nullptr)
    return;

  if (cmd->addon1 != nullptr)
    clearCmd(cmd->addon1);
  if (cmd->addon2 != nullptr)
    clearCmd(cmd->addon2);
  if (cmd->addon3 != nullptr)
    clearCmd(cmd->addon3);
  for (auto &nest : cmd->nestedCommand)
    clearCmd(nest);
  delete cmd;
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

    /* Chek empty command */
    if (cur.id == TokID::SPEC && cur.symbol == ';')
      return res.str();

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
      /* Because switch swears */
      std::string exprInIf, exprInWhile, 
                  exprFor3, exprArraySize;

      switch (cur.keyw)
      {
      case Keyword::IF:
        /* Skip keyword */
        id = CommandID::IF;
        nextTok();

        /* Check '(' */
        if (cur.id != TokID::OP || cur.op.name != "(")
          throw "After if '(' expected!";

        /* Parse an if statement */
        exprInIf = parseBetween(tokList, ')');
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
        break;
      case Keyword::WHILE:
        /* Skip keyword */
        id = CommandID::WHILE;
        nextTok();

        /* Check '(' */
        if (cur.id != TokID::OP || cur.op.name != "(")
          throw "After while '(' expected!";

        /* Parse a while statement */
        exprInWhile = parseBetween(tokList, ')');
        expr = Calculator(exprInWhile);

        /* Parse expression in while statement */
        addon1 = new Command(tokList);
        if (!addon1->parse())
          throw "Unexpected end of while block!";
        break;
      case Keyword::FOR:
        /* Skip keyword */
        id = CommandID::FOR;
        nextTok();

        /* Check '(' */
        if (cur.id != TokID::OP || cur.op.name != "(")
          throw "After for '(' expected!";

        /* Parse 1st expression */
        addon1 = new Command(tokList);
        if (!addon1->parse() || addon1->id != CommandID::EXPR)
          throw "Error in parse 1st expression in for loop!";

        /* Get expression to stop loop (2nd expression) */
        nextTok();
        expr = Calculator(getExpr());

        /* Parse 3rd expression */
        addon2 = new Command(tokList);
        exprFor3 = parseBetween(tokList, ')');
        addon2->expr = Calculator(exprFor3);

        /* Parse body of loop */
        addon3 = new Command(tokList);
        if (!addon3->parse())
          throw "Unexpected end of for block!";

        break;
      case Keyword::ECHO:
        /* Get information from token & skip */
        id = CommandID::ECHO;
        echoStr = cur.name;
        nextTok();

        if (cur.id != TokID::SPEC || cur.symbol != ';')
          throw "After echo statement ';' required!";
        break;
      case Keyword::ARRAY:
        /* Skip it */
        id = CommandID::ARRAY;
        nextTok();

        /* Check '(' */
        if (cur.id != TokID::OP || cur.op.name != "(")
          throw "After array '(' expected!";
        nextTok();

        /* Check name */
        if (cur.id != TokID::VAR)
          throw "Invalid array name!";
        echoStr = cur.name;
        nextTok();

        /* Check ')' */
        if (cur.id != TokID::OP || cur.op.name != ")")
          throw "After array name ')' expected!";
        nextTok();

        /* Check '(' again */
        if (cur.id != TokID::OP || cur.op.name != "(")
          throw "After array name with () '(' expected!";

        /* Parse array size expression */
        exprArraySize = parseBetween(tokList, ')');
        expr = Calculator(exprArraySize);

        cur = tokList.front();
        if (cur.id != TokID::SPEC || cur.symbol != ';')
          throw "After array declaration ';' required!";
        break;
      case Keyword::ELSE:
        throw "Invalid else without paired if!";
      default:
        throw "Unknown keyword!";
      }
    }
    else if (cur.id == TokID::SPEC)
    {
      if (cur.symbol == '{')
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
      else if (cur.symbol == ';') // empty command
        return true;
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
  switch (id)
  {
  case CommandID::EXPR:
    expr.eval();
    break;
  case CommandID::IF:
    if ((bool)expr.eval())
      addon1->run();
    else if (addon2 != nullptr)
      addon2->run();
    break;
  case CommandID::WHILE:
    while ((bool)expr.eval())
      addon1->run();
    break;
  case CommandID::FOR:
    for (addon1->expr.eval(); (bool)expr.eval(); addon2->expr.eval())
      addon3->run();
    break;
  case CommandID::ECHO:
    std::cout << echoStr << std::endl;
    break;
  case CommandID::ARRAY:
    Tok::varTree[echoStr].type = VarType::ARRAY;
    Tok::varTree[echoStr].arr.resize((size_t)expr.eval());
    for (auto &num : Tok::varTree[echoStr].arr)
      num = 0;
    break;
  case CommandID::COMPOSITE:
    for (const auto &cmd : nestedCommand)
      cmd->run();
    break;
  default:
    throw "Unknown command!";
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

      try
      {
        if (!cmd->parse())
        {
          clearCmd(cmd);
          break;
        }
        cmdList.put(cmd);
      }
      catch ( const char *err )
      {
        clearCmd(cmd);
        clear();
        throw err;
      }
      catch ( const std::exception &ex )
      {
        clearCmd(cmd);
        clear();
        throw ex;
      }
      catch (...)
      {
        clearCmd(cmd);
        clear();
        throw "Unknown error in parse!";
      }
    }
  }
  catch ( const char *err )
  {
    throw err;
  }
  catch ( const std::exception &ex )
  {
    throw ex;
  }
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

/* Interpreter clear */
void Interpreter::clear( void )
{
  Command *cmd;

  while (cmdList.get(&cmd))
    clearCmd(cmd);
}

/* Interpreter destructor */
Interpreter::~Interpreter( void )
{
  clear();
}