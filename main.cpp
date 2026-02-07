#include <iostream>
#include <string>
#include "environment.h"

int main(int argc, char* argv[])
{
   try
   {
      Environment environment(argc, argv);
      environment.start();
   }
   catch (std::string ex)
   {
      std::cerr << ex << std::endl;
   }
   catch (...)
   {
      std::cerr << "An unknown exception has occurred\n";
   }
   return 0;
}