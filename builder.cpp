#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, char const *argv[])
{
  system("g++ one/main.cpp one/Core/*.cpp one/Loaders/*.cpp one/SQConnect/*.cpp one/Squirrel/*.cpp -lsfml-graphics -lsfml-window -lsfml-system -std=c++11 -o one/engine.out");
  return 0;
}

// find -name \*.out -print
