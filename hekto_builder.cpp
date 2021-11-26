// Copyright 2021 Zusitools

#include "hekto_builder.hpp"
#include "templates.hpp"

#include <boost/algorithm/string/replace.hpp>

#include <array>
#include <cassert>
#include <string>
#include <string_view>
#include <unordered_map>

namespace HektoBuilder {

void
Build(FILE* fd, Kilometrierung kilometrierung)
{
  const bool ist_negativ = kilometrierung.istNegativ();
  const int zahl_oben = std::abs(kilometrierung.km);
  const int ziffer_unten = std::abs(kilometrierung.hm);

  assert(zahl_oben >= 0);
  assert(zahl_oben <= 999);
  assert(ziffer_unten >= 0);
  assert(ziffer_unten <= 9);

  std::unordered_map<std::string, float> replacements;
  const auto& set_ziffer = [&](std::string_view prefix, int ziffer) {
    if (ziffer == 8) {
      replacements[std::string(prefix) + "_lo_u"] = 1000.0 / 2550.0;
      replacements[std::string(prefix) + "_lo_v"] = 2000.0 / 2550.0;
      replacements[std::string(prefix) + "_ro_u"] = 1000.0 / 2550.0;
      replacements[std::string(prefix) + "_ro_v"] = 2550.0 / 2550.0;
      replacements[std::string(prefix) + "_lu_u"] = 0.0 / 2550.0;
      replacements[std::string(prefix) + "_lu_v"] = 2000.0 / 2550.0;
      replacements[std::string(prefix) + "_ru_u"] = 0.0 / 2550.0;
      replacements[std::string(prefix) + "_ru_v"] = 2550.0 / 2550.0;
    } else {
      std::array<double, 10> xoffsets = { 550.0,  1650.0, 0.0,    1650.0, 550.0,
                                          1100.0, 550.0,  1100.0, 0.0,    0.0 };
      std::array<double, 10> yoffsets = {
        1000.0, 0.0, 0.0, 1000.0, 0.0, 0.0, 2000.0, 1000.0, 0.0, 1000.0
      };
      double breite = (ziffer == 6 ? -550.0 : 550.0);
      double hoehe = (ziffer == 6 ? -1000.0 : 1000.0);
      replacements[std::string(prefix) + "_lo_u"] = xoffsets[ziffer] / 2550.0;
      replacements[std::string(prefix) + "_lo_v"] = yoffsets[ziffer] / 2550.0;
      replacements[std::string(prefix) + "_ro_u"] =
        (xoffsets[ziffer] + breite) / 2550.0;
      replacements[std::string(prefix) + "_ro_v"] = yoffsets[ziffer] / 2550.0;
      replacements[std::string(prefix) + "_lu_u"] = xoffsets[ziffer] / 2550.0;
      replacements[std::string(prefix) + "_lu_v"] =
        (yoffsets[ziffer] + hoehe) / 2550.0;
      replacements[std::string(prefix) + "_ru_u"] =
        (xoffsets[ziffer] + breite) / 2550.0;
      replacements[std::string(prefix) + "_ru_v"] =
        (yoffsets[ziffer] + hoehe) / 2550.0;
    }
  };

  auto contents = [&]() {
    set_ziffer("hm", ziffer_unten);

    set_ziffer("einer", zahl_oben % 10);
    if (zahl_oben < 10) {
      return templates::template1;
    }

    set_ziffer("zehner", (zahl_oben / 10) % 10);
    if (zahl_oben < 100) {
      return templates::template2;
    }

    set_ziffer("hunderter", (zahl_oben / 100) % 10);
    return templates::template3;
  }();

  for (const auto& [key, value] : replacements) {
    boost::replace_all(contents, key, std::to_string(value));
  }

  fprintf(fd, "%s", contents.c_str());
}

} // namespace HektoBuilder
