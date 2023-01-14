// Copyright 2021 Zusitools

#include "dll.hpp"

#include <cassert>
#include <iostream>

int
main(int argc, char** argv)
{
  assert(argc > 1);

  std::cout << Init(argv[1]) << '\n';
  std::cout << dllVersion() << '\n';
  std::cout << Autor() << '\n';
  std::cout << Bezeichnung() << '\n';
  std::cout << AbstandTafeln() << '\n';

  auto erzeugen = [](int meter, [[maybe_unused]] uint8_t standort) {
    const char* datei;
    int result = Erzeugen(meter,
#ifdef LGV
                          standort,
#else
                          0,
#endif
                          &datei);
    std::cout << "Ergebnis: " << result << '\n';
    if (datei != nullptr) {
      std::cout << "Erzeugte Datei: \"" << datei << "\"\n";
    }
  };
  erzeugen(138400, 0);
  erzeugen(40700, 1);
  erzeugen(2900, 0);
}
