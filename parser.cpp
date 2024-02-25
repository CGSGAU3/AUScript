#include "parser.h"

Queue<Tok> & Parser::run( void )
{
  State state = State::PREFIX;

  auto checkAssoc = []
  ( const Oper &o1, const Oper &o2 )
  {
    int p1 = (int)o1.prior, p2 = (int)o2.prior;

    if (o1.assocType == OperAssocType::RIGHT ||
        o2.assocType == OperAssocType::RIGHT)
      return p1 > p2;
    return p1 >= p2;
  };

  auto dropOpers = [&]
  ( const Oper &op )
  {
    while (!opStack.empty() && checkAssoc(opStack.front().op, op))
    {
      Tok token;

      opStack.pop(&token);
      opQueue.put(token);
    }
  };

  while (true)
  {
    Tok token;

    if (state == State::PREFIX ||
        state == State::SUFFIX)
    {
      if (!source.get(&token))
        if (state == State::SUFFIX)
          state = State::DONE;
        else
          throw "Unexpected end of expression!";
    }

    switch (state)
    {
    case State::PREFIX:
      if (token.id == TokID::NUM || token.id == TokID::VAR)
        opQueue.put(token), state = State::SUFFIX;
      else if (token.op.type == OperType::PREFIX)
        opStack.push(token);
      else if (token.op.name == "-")
        token.op = Tok::findOper("@"), opStack.push(token);
      else
        throw "Number or prefix operator expected!";
      break;
    case State::SUFFIX:
      if (token.id != TokID::OP || token.op.name == "(")
        throw "Operator or ')' expected!";

      /* Put oper to the stack, but at first drop opers with
         prior >= current oper prior to the result queue */
      dropOpers(token.op);

      if (token.op.name != ")")
        opStack.push(token), state = State::PREFIX;
      else if (!opStack.pop())
        throw "Missing '('";
      break;
    case State::DONE:
      /* Process closing bracket */
      dropOpers(Tok::findOper(")"));
      if (!opStack.empty())
        throw "Missing ')'";
      return opQueue;
    }
  }
}