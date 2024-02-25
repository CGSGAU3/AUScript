#include "calc.h"

Calculator::Calculator( const std::string &str )
{
  try
  {
    /* Scan & parse expression */
    scanner = Scanner(str);
    parser = Parser(scanner.getQueue());

    if (!scanner.getQueue().empty())
      poliz = parser.run();
  }
  catch ( const char *err )
  {
    scanner.clear();
    parser.clear();
    throw err;
  }
  catch (...)
  {
    throw "Smt goes wrong in calc...";
  }
}

double Calculator::eval( void ) const
{
  Stack<Tok> sEval;
  auto polizCopy = poliz;

  /* Nothing to evaluate */
  if (poliz.empty())
    return 1;

  /* Check declaration of variables to avoid collapses */
  auto checkDeclared = []
  ( const std::string &name )
  {
    if (Tok::varTree.find(name) == Tok::varTree.end())
    {
      std::string errMsg = "Undeclared variable '" + 
                           name + "'!";

      throw errMsg;
    }
  };

  Tok token, res;

  /* Get all operators & operands from the queue */
  while (polizCopy.get(&token))
  {
    Tok left, right;

    /*
     * Variable/number -> push in stack
     * Operator -> get operands from the stack & push result
     */
    if (token.id == TokID::NUM || 
        token.id == TokID::VAR )
    {
      if (token.id == TokID::VAR && 
          Tok::varTree.find(token.name) != Tok::varTree.end())
        token.num = Tok::varTree[token.name];

      sEval.push(token);
    }
    else
    {
      /* Pop both left & right operands or only left for unary */
      if (token.op.type != OperType::PREFIX || 
          token.op.name == "printw" ||
          token.op.name == "printwln")
      {
        /* In practice never reached but for safety we must */
        if (!sEval.pop(&right))
          throw "Not enough arguments!";

        if (right.id == TokID::VAR)
        {
          /* Check declaration of right operand */
          checkDeclared(right.name);

          /* variable -> number transition */
          right.num = Tok::varTree[right.name], right.id = TokID::NUM;
        }
      }

      /* The same reason with right */
      if (!sEval.pop(&left))
        throw "Not enough arguments!";

      /* If lvalue not required we can make transition */
      if (left.id == TokID::VAR && 
          token.op.name != "=" && token.op.name != "++" &&
          token.op.name != "--" && token.op.name != "+=" &&
          token.op.name != "-=" && token.op.name != "/=" &&
          token.op.name != "%=" && token.op.name != "+=" &&
          token.op.name != "scan")
      {
        /* Check declaration of left operand */
        checkDeclared(left.name);

        /* variable -> number transition */
        left.num = Tok::varTree[left.name], left.id = TokID::NUM;
      }

      /* Check printw & printwln, because they require 2 arguments */
      /* After ',' callback right -> left */
      if (token.op.name == "," && 
         (polizCopy.front().op.name == "printw" || 
          polizCopy.front().op.name == "printwln"))
        sEval.push(left);

      /* Call function of operator and push result to stack */
      token.op.doFunc(left, right);
      sEval.push(left);
    }
  }

  /* Get result from the stack */
  if (!sEval.pop(&res))
    throw "Undefined error!";
  if (!sEval.empty()) // unreachable due to always correct poliz
    throw "Not enough operators!";

  /* Check declaration of result if its variable */
  if (res.id == TokID::VAR)
    checkDeclared(res.name);

  return res.num;
}