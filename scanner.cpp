#include <setjmp.h>

#include "scanner.h"

Scanner::Scanner( const std::string &str )
{
  const char *s = str.c_str();

  auto isOperSymbol = []
  ( char ch )
  {
    std::string all = "+-*/%()^#=,<>!&|";

    return (all.find(ch) != all.npos);
  };

  while (*s != 0)
  {
    Tok token;

    /* Process spaces */
    if (isspace((unsigned char)*s))
    {
      s++;
      continue;
    }

    /* Process numbers */
    if (isdigit((unsigned char)*s))
    {
      token.id = TokID::NUM;
      token.num = 0;
      while (isdigit((unsigned char)*s))
        token.name += *s, token.num = token.num * 10 + *s++ - '0';
      if (*s == '.')
      {
        double denom = 1;

        token.name += *s++;
        while (isdigit((unsigned char)*s))
          token.name += *s, token.num += (double(*s++) - '0') / (denom *= 10);
      }
      scanned.put(token);
      continue;
    }

    jmp_buf operFind;
    /* Process operators */
    if (isOperSymbol(*s))
    {
      token.id = TokID::OP;
      token.op.name = "";

      while (isOperSymbol(*s))
        token.op.name += *s++;

      if (setjmp(operFind))
      {
        if (token.op.name == "")
          throw "Unknown operator!";
      }
      try
      {
        token.op = Tok::findOper(token.op.name);
      }
      catch ( const char * )
      {
        token.op.name.pop_back();
        s--;
        longjmp(operFind, 1);
      }
      catch (...)
      {
        throw "Unknown error in oper search!";
      }

      scanned.put(token);
      continue;
    }

    /* Process symbols & variables */
    switch (*s)
    {
    case '{':
    case '}':
    case ';':
      token.id = TokID::SPEC;
      token.symbol = *s++;
      break;
    default: // process functions & variables
      if (isalpha((unsigned char)*s))
      {
        /* Keyword upgrade function */
        auto upgradeKeyword = []
        ( Tok &tok )
        {
          std::map<std::string, Keyword> kws = 
          {
            {"if", Keyword::IF},
            {"else", Keyword::ELSE},
            {"while", Keyword::WHILE}
          };

          if (kws.find(tok.name) != kws.end())
          {
            tok.id = TokID::KEYW;
            tok.keyw = kws[tok.name];
          }
        };

        /* Variables by default... */
        token.id = TokID::VAR;
        while (isalnum((unsigned char)*s))
          token.name += *s++;

        /* Assign number to variable if exists */
        if (Tok::varTree.find(token.name) != Tok::varTree.end())
          token.num = Tok::varTree[token.name];

        /* Try to upgrade variable to keyword */
        upgradeKeyword(token);

        /* Upgrade variable to oper */
        for (const auto &op : Tok::opers)
        {
          if (token.name == op.name)
          {
            token.id = TokID::OP;
            token.op = op;
            break;
          }
        }
        break;
      }
      else
        throw "Unknown character!";
    }
    scanned.put(token);
  }
}