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

  auto erzeugen = [](int meter) {
    const char* datei;
    int result = Erzeugen(meter, 1, &datei);
    std::cout << "Ergebnis: " << result << ", Erzeugte Datei: \"" << datei
              << "\"\n";
  };
  erzeugen(138500);
  erzeugen(40700);
  erzeugen(2900);
}
