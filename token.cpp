#include <cmath>

#include "token.h"

/* Variables store */
std::map<std::string, double> Tok::varTree;

/* Operators store */
std::vector<Oper> Tok::opers = 
{
#pragma region Base infix & power
  {"+", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num += r.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::PLUSMIN},
  {"-", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num -= r.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::PLUSMIN},
  {"*", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num *= r.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::MULDIV},
  {"/", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (fabs(r.num) < 1e-14)
              throw "Division by zero!";
            l.num /= r.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::MULDIV},
  {"%", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (fabs(r.num) < 1e-14)
              throw "Division by zero!";

            l.num = fmod(l.num, r.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::MULDIV},
  {"^", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = pow(l.num, r.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::IOSQRT},
#pragma endregion
#pragma region Unary operations
  {"@", std::function<Tok( Tok &, const Tok & )> // unary -
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = -l.num;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::UNARYOP},
  {"#", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = sqrt(l.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"++", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name]++, l.num++;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::RIGHT, Prior::UNARYOP},
  {"--", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name]--, l.num--;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::RIGHT, Prior::UNARYOP},
#pragma endregion
#pragma region Boolean logic
  {"&&", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = ((bool)l.num && (bool)r.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::BOOLAND},
  {"||", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = ((bool)l.num || (bool)r.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::BOOLOR},
  {"!", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = !(bool)l.num;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::RIGHT, Prior::UNARYOP},
#pragma endregion
  {",", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.id = TokID::NUM;
            l.num = r.num;
            return l;
          }
        ), OperType::POSTFIX, OperAssocType::RIGHT, Prior::ZPT},
#pragma region Assignment
  {"=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name] = r.num;
            l.num = r.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"+=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name] += r.num;
            l.num += r.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"-=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name] -= r.num;
            l.num -= r.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"*=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name] *= r.num;
            l.num *= r.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"/=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            if (fabs(r.num) < 1e-14)
              throw "Division by zero!";

            Tok::varTree[l.name] /= r.num;
            l.num /= r.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"%=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            if (fabs(r.num) < 1e-14)
              throw "Division by zero!";

            Tok::varTree[l.name] = fmod(l.num, r.num);
            l.num = fmod(l.num, r.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
#pragma endregion
#pragma region Brackets
  {"(", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::OPENBR},
  {")", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            return l;
          }
        ), OperType::POSTFIX, OperAssocType::LEFT, Prior::CLOSEBR},
#pragma endregion
#pragma region Compare
  {"<", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = (l.num < r.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::LESSMORE},
  {"<=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = (l.num <= r.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::LESSMORE},
  {">", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = (l.num > r.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::LESSMORE},
  {">=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = (l.num >= r.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::LESSMORE},
  {"==", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = (fabs(l.num - r.num) < 1e-14);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::EQUALNOT},
  {"!=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = (fabs(l.num - r.num) >= 1e-14);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::EQUALNOT},
#pragma endregion
#pragma region I&O & special functions
  {"print", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            double frac = l.num - (int)l.num;

            if (fabs(frac) < 1e-14)
              printf("%i", (int)l.num);
            else
              printf("%g", l.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"println", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            double frac = l.num - (int)l.num;

            if (fabs(frac) < 1e-14)
              printf("%i\n", (int)l.num);
            else
              printf("%g\n", l.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"printw", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            double frac = l.num - (int)l.num;

            if (fabs(frac) < 1e-14)
              printf("%*i", (int)r.num, (int)l.num);
            else
              printf("%*g", (int)r.num, l.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"printwln", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            double frac = l.num - (int)l.num;

            if (fabs(frac) < 1e-14)
              printf("%*i\n", (int)r.num, (int)l.num);
            else
              printf("%*g\n", (int)r.num, l.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"scan", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            std::cin >> l.num;
            Tok::varTree[l.name] = l.num;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"sin", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = sin(l.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::SINCOS},
  {"cos", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.num = cos(l.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::SINCOS},
#pragma endregion
};

/* Find oper by its name method */
Oper & Tok::findOper( const std::string &name )
{
  for (auto &op : opers)
    if (op.name == name)
      return op;
  throw "Unknown operator!";
}

/* Output token operator */
std::ostream & operator <<( std::ostream &stream, const Tok &token )
{
  switch (token.id)
  {
  case TokID::NUM:
    stream << token.num;
    break;
  case TokID::OP:
    stream << token.op.name;
    break;
  case TokID::VAR:
    stream << token.name;
    break;
  case TokID::SPEC:
    stream << token.symbol;
    break;
  case TokID::KEYW:
    switch (token.keyw)
    {
    case Keyword::IF:
      stream << "if";
      break;
    case Keyword::ELSE:
      stream << "else";
      break;
    case Keyword::WHILE:
      stream << "while";
      break;
    default:
      stream << "<Unknown keyword>";
      break;
    }
    break;
  default:
    stream << "<Unknown token>";
    break;
  }
  return stream;
}