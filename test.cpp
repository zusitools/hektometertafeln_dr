// Copyright 2021 Zusitools

#include <iostream>

#include "dll.hpp"

int
main(int argc, char** argv)
{
  std::cout << Init(argv[1]) << '\n';
  std::cout << dllVersion() << '\n';
  std::cout << Autor() << '\n';
  std::cout << Bezeichnung() << '\n';
  std::cout << AbstandTafeln() << '\n';

  const char* datei;
  std::cout << Erzeugen(138500, 1, &datei) << '\n';
  std::cout << datei << '\n';
  std::cout << Erzeugen(40700, 1, &datei) << '\n';
  std::cout << datei << '\n';
  std::cout << Erzeugen(2900, 1, &datei) << '\n';
  std::cout << datei << '\n';
}
