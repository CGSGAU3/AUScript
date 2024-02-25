#include <cmath>

#include "token.h"

/* Variables store */
std::map<std::string, Variable> Tok::varTree;

/* Operators store */
std::vector<Oper> Tok::opers = 
{
#pragma region Base infix & power
  {"+", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num += r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::PLUSMIN},
  {"-", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num -= r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::PLUSMIN},
  {"*", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num *= r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::MULDIV},
  {"/", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (fabs(r.var.num) < 1e-14)
              throw "Division by zero!";
            l.var.num /= r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::MULDIV},
  {"//", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (fabs(r.var.num) < 1e-14)
              throw "Division by zero!";
            l.var.num = int(l.var.num / r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::MULDIV},
  {"%", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (fabs(r.var.num) < 1e-14)
              throw "Division by zero!";

            l.var.num = fmod(l.var.num, r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::MULDIV},
  {"^", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = pow(l.var.num, r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::IOSQRT},
#pragma endregion
#pragma region Unary operations
  {"@", std::function<Tok( Tok &, const Tok & )> // unary -
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = -l.var.num;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::UNARYOP},
  {"#", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = sqrt(l.var.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"++", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name].num++, l.var.num++;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::RIGHT, Prior::UNARYOP},
  {"--", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name].num--, l.var.num--;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::RIGHT, Prior::UNARYOP},
#pragma endregion
#pragma region Boolean logic
  {"&&", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = ((bool)l.var.num && (bool)r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::BOOLAND},
  {"||", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = ((bool)l.var.num || (bool)r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::BOOLOR},
  {"!", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = !(bool)l.var.num;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::RIGHT, Prior::UNARYOP},
#pragma endregion
  {",", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.id = TokID::NUM;
            l.var.num = r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ZPT},
#pragma region Assignment
  {"=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name].num = r.var.num;
            if (l.var.type == VarType::ARRAY)
              Tok::varTree[l.name].arr[l.var.usedIndex] = r.var.num;

            l.var.num = r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"+=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name].num += r.var.num;
            l.var.num += r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"-=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name].num -= r.var.num;
            l.var.num -= r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"*=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            Tok::varTree[l.name].num *= r.var.num;
            l.var.num *= r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"/=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            if (fabs(r.var.num) < 1e-14)
              throw "Division by zero!";

            Tok::varTree[l.name].num /= r.var.num;
            l.var.num /= r.var.num;
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"//=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            if (fabs(r.var.num) < 1e-14)
              throw "Division by zero!";

            Tok::varTree[l.name].num = int(Tok::varTree[l.name].num / r.var.num);
            l.var.num = int(l.var.num / r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::RIGHT, Prior::ASSIGN},
  {"%=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            if (fabs(r.var.num) < 1e-14)
              throw "Division by zero!";

            Tok::varTree[l.name].num = fmod(l.var.num, r.var.num);
            l.var.num = fmod(l.var.num, r.var.num);
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
  {"[", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            return l;
          }
        ), OperType::POSTFIX, OperAssocType::LEFT, Prior::OPENIDX},
  {"]", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            return l;
          }
        ), OperType::POSTFIX, OperAssocType::LEFT, Prior::CLOSEIDX},
  {"[]", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            if (l.var.type != VarType::ARRAY)
              throw "Array required!";

            int index = (int)r.var.num;

            if (index >= (int)l.var.arr.size() || index < 0)
              throw "Invalid index!";

            l.var.num = Tok::varTree[l.name].arr[index];
            l.var.usedIndex = index;
            return l;
          }
        ), OperType::POSTFIX, OperAssocType::LEFT, Prior::INDEX},
#pragma endregion
#pragma region Compare
  {"<", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = (l.var.num < r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::LESSMORE},
  {"<=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = (l.var.num <= r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::LESSMORE},
  {">", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = (l.var.num > r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::LESSMORE},
  {">=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = (l.var.num >= r.var.num);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::LESSMORE},
  {"==", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = (fabs(l.var.num - r.var.num) < 1e-14);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::EQUALNOT},
  {"!=", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = (fabs(l.var.num - r.var.num) >= 1e-14);
            return l;
          }
        ), OperType::INFIX, OperAssocType::LEFT, Prior::EQUALNOT},
#pragma endregion
#pragma region I&O & special functions
  {"print", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            double frac = l.var.num - (int)l.var.num;

            if (fabs(frac) < 1e-14)
              printf("%i", (int)l.var.num);
            else
              printf("%g", l.var.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"println", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            double frac = l.var.num - (int)l.var.num;

            if (fabs(frac) < 1e-14)
              printf("%i\n", (int)l.var.num);
            else
              printf("%g\n", l.var.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"printw", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            double frac = l.var.num - (int)l.var.num;

            if (fabs(frac) < 1e-14)
              printf("%*i", (int)r.var.num, (int)l.var.num);
            else
              printf("%*g", (int)r.var.num, l.var.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"printwln", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            double frac = l.var.num - (int)l.var.num;

            if (fabs(frac) < 1e-14)
              printf("%*i\n", (int)r.var.num, (int)l.var.num);
            else
              printf("%*g\n", (int)r.var.num, l.var.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"scan", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            if (l.id != TokID::VAR)
              throw "LValue required!";

            std::cin >> l.var.num;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (std::cin.fail())
            {
              std::cin.clear();
              throw "Invalid input!";
            }
            Tok::varTree[l.name].num = l.var.num;
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::IOSQRT},
  {"sin", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = sin(l.var.num);
            return l;
          }
        ), OperType::PREFIX, OperAssocType::LEFT, Prior::SINCOS},
  {"cos", std::function<Tok( Tok &, const Tok & )>
        ([]( Tok &l, const Tok &r ) -> Tok
          {
            l.var.num = cos(l.var.num);
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
    stream << token.var.num;
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