#include <setjmp.h>

#include "vm.h"

int main( void )
{
  jmp_buf jmpBuffer;

  auto outputNumber = []
  ( double num )
  {
    double frac = num - (int)num;

    if (fabs(frac) < 1e-14)
      printf("%i", (int)num);
    else
      printf("%.16g", num);
  };

  auto outputVar = [&]
  ( const Variable &var )
  {
    if (var.type == VarType::SINGLE)
      outputNumber(var.num);
    else
    {
      std::cout << "[";
      for (size_t i = 0; i < var.arr.size() - 1; i++)
        std::cout << var.arr[i] << ", ";
      std::cout << var.arr[var.arr.size() - 1];
      std::cout << "]";
    }
  };

  // If we catch error it'll go there and redo input
  if (setjmp(jmpBuffer));

  try
  {
    while (true)
    {
      std::string str;

      std::cout << ">>> ";
      std::getline(std::cin, str);

      if (str.length() == 0)
        continue;
      if (str == "exit" || str == "exit()")
        break;
      if (str == "getVars")
      {
        std::cout << "\nVariables:" << std::endl;
        for (const auto &p : Tok::varTree)
        {
          std::cout << p.first << " = ";
          outputVar(p.second);
          std::cout << std::endl;
        }
        continue;
      }
      if (str == "pifTable")
      {
        Interpreter intp("pif.aus");

        intp.run();
        continue;
      }
      if (str == "sort")
      {
        Interpreter intp("sort.aus");

        intp.run();
        continue;
      }
      if (str == "toBin")
      {
        Interpreter intp("arr.aus");

        intp.run();
#if 0
        long t = clock();
        t = clock() - t;
        std::cout << "time: " << t << " milliseconds" << std::endl;
#endif
        continue;
      }

      Calculator calc(str);

      outputNumber(calc.eval());
      std::cout << std::endl;
    }
  }
  catch ( const char *err )
  {
    // Don't return fail but try to input expression one more time
    // If there is no LITERALLY error, may be do std::cout instead of std::cerr?
    std::cerr << err << std::endl;
    longjmp(jmpBuffer, 1);
  }
  catch ( const std::string &undecl )
  {
    // Because we can
    std::cerr << undecl << std::endl;
    longjmp(jmpBuffer, 1);
  }
  catch ( const std::exception &ex )
  {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch (...)
  {
    std::cerr << "Smt goes wrong..." << std::endl;
    return EXIT_FAILURE;
  }
}