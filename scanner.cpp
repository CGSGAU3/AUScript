#include <setjmp.h>

#include "scanner.h"

Scanner::Scanner( const std::string &str )
{
  const char *s = str.c_str();

  auto isOperSymbol = []
  ( char ch )
  {
    std::string all = "+-*/%()^#=,<>!&|[]";

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
      token.var.num = 0;
      while (isdigit((unsigned char)*s))
        token.name += *s, token.var.num = token.var.num * 10 + *s++ - '0';
      if (*s == '.')
      {
        double denom = 1;

        token.name += *s++;
        while (isdigit((unsigned char)*s))
          token.name += *s, token.var.num += (double(*s++) - '0') / (denom *= 10);
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
            {"while", Keyword::WHILE},
            {"for", Keyword::FOR},
            {"break", Keyword::BREAK},
            {"continue", Keyword::CONTINUE},
            {"echo", Keyword::ECHO},
            {"array", Keyword::ARRAY}
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
          token.var = Tok::varTree[token.name];

        /* Try to upgrade variable to keyword */
        upgradeKeyword(token);

        /* Check echo */
        if (token.id == TokID::KEYW && token.keyw == Keyword::ECHO)
        {
          while (isspace((unsigned char)*s))
            s++;
          if (*s++ != '(')
            throw "After echo keyword need (<text>) !";

          /* Pre-parse string in echo */
          int brCount = 1;
          while (brCount && *s != 0)
          {
            if (*s == '(')
              brCount++;
            if (*s == ')')
              brCount--;

            if (brCount != 0)
              token.name += *s;
            s++;
          }

          if (*s == 0 && brCount != 0)
            throw "Troubles with brackets in echo!";

          /* Finaize (skip 'echo' word) */
          token.name = token.name.substr(4);
        }

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