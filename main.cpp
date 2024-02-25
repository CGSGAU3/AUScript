#include <setjmp.h>

#include "vm.h"

int main( void )
{
  jmp_buf jmpBuffer;

  /*
   * TODO
   *   - make keyword to output NORMAL string
   *       echo(Hello, World!);
   *   - make arrays (VERY OPTIONAL)
   */

  auto outputNumber = []
  ( double num )
  {
    double frac = num - (int)num;

    if (fabs(frac) < 1e-14)
      printf("%i", (int)num);
    else
      printf("%g", num);
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
      {
        std::cout << std::endl;
        continue;
      }
      if (str == "exit" || str == "exit()")
        break;
      if (str == "getVars")
      {
        std::cout << "\nVariables:" << std::endl;
        for (const auto &p : Tok::varTree)
        {
          std::cout << p.first << " = ";
          outputNumber(p.second);
          std::cout << std::endl;
        }
        continue;
      }
      if (str == "pifTable")
      {
        Interpreter intp("pif.aus");

        std::cout << "Input last number: ";
        long t = clock();
        intp.run();
        t = clock() - t;
        std::cout << "time: " << t << " milliseconds" << std::endl;
        continue;
      }

      Calculator calc(str);

      std::cout << calc.eval() << std::endl;
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