// Copyright 2021 Zusitools

#ifndef HEKTO_BUILDER_HPP_
#define HEKTO_BUILDER_HPP_

#include <cstdint>
#include <cstdio>
#include <cstdlib>

struct Kilometrierung
{
  // Invariante: -oo < km < oo
  // Invariante: 0 <= |hm| <= 9
  // Invariante: km < 0 ==> hm < 0
  // Invariante: km > 0 ==> hm > 0
  const int km;
  const int hm;

  static Kilometrierung fromMeter(int wert_m)
  {
#ifndef LGV
    // Runde auf Hektometer
    int wert_m_int = std::abs(wert_m) + 50;
    int vorzeichen = (wert_m < 0 ? -1 : 1);
    return { vorzeichen * (wert_m_int / 1000),
             vorzeichen * ((wert_m_int % 1000) / 100) };
#else
    // Runde auf Kilometer
    int wert_m_int = std::abs(wert_m) + 500;
    int vorzeichen = (wert_m < 0 ? -1 : 1);
    return { vorzeichen * (wert_m_int / 1000), 0 };
#endif
  }

  int toHektometer() const { return 10 * km + hm; }

  bool istNegativ() const { return hm < 0; }
};

enum class Standort : std::uint8_t
{
  kEigenerStandort = 0,
  kMontageAmAnkerpunkt = 1,
};

namespace HektoBuilder {

void
Build(FILE* fd,
#ifdef LGV
      Standort standort,
#endif
      Kilometrierung kilometrierung);

}

#endif // HEKTO_BUILDER_HPP_
