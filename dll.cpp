// Copyright 2021 Zusitools

#include "dll.hpp"

#include <shlwapi.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>

#include "hekto_builder.hpp"

// Globale Variablen
DWORD g_zusi_datenpfad_laenge;
char g_zielverzeichnis[MAX_PATH]; // wird von Zusi beim Initialisieren gesetzt;
                                  // ohne abschliessenden Slash/Backslash
char g_outDatei[MAX_PATH];        // zwecks Rueckgabe an Zusi

DLL_EXPORT uint32_t
Init(const char* zielverzeichnis)
{
  HKEY key;
  g_zusi_datenpfad_laenge = MAX_PATH;
  if (!SUCCEEDED(RegOpenKeyEx(
        HKEY_LOCAL_MACHINE, "Software\\Zusi3", 0, KEY_READ, &key))) {
    fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
    return 0;
  }

  const auto hatDatenverzeichnisRegulaer =
    (RegQueryValueEx(
       key, "DatenVerzeichnis", nullptr, nullptr, nullptr, nullptr) ==
     ERROR_SUCCESS);
  const auto hatDatenverzeichnisSteam =
    (RegQueryValueEx(
       key, "DatenVerzeichnisSteam", nullptr, nullptr, nullptr, nullptr) ==
     ERROR_SUCCESS);

  const auto liesDatenverzeichnis = [&](const char* wertName) -> bool {
    DWORD type;
    g_zusi_datenpfad_laenge = MAX_PATH;
    // Kann RegGetValue nicht nutzen, da auf Windows XP nicht unterstuetzt.
    if (!SUCCEEDED(RegQueryValueEx(key,
                                   wertName,
                                   nullptr,
                                   &type,
                                   (LPBYTE)g_zielverzeichnis,
                                   &g_zusi_datenpfad_laenge))) {
      return false;
    }
    if (type != REG_SZ) {
      return false;
    }
    // "If the data has the REG_SZ, REG_MULTI_SZ or REG_EXPAND_SZ type, the
    // string may not have been stored with the proper terminating null
    // characters. Therefore, even if the function returns ERROR_SUCCESS, the
    // application should ensure that the string is properly terminated before
    // using it"
    if (g_zielverzeichnis[g_zusi_datenpfad_laenge - 1] != '\0') {
      g_zusi_datenpfad_laenge =
        std::min(g_zusi_datenpfad_laenge + 1, static_cast<DWORD>(MAX_PATH));
      g_zielverzeichnis[g_zusi_datenpfad_laenge - 1] = '\0';
    }
    g_zusi_datenpfad_laenge = std::strlen(g_zielverzeichnis);
    return true;
  };

  if (hatDatenverzeichnisRegulaer && hatDatenverzeichnisSteam) {
    // Wenn _InstSetup/usb.dat relativ zum Zusi-Verzeichnis existiert, sind
    // wir eine regulaere Version
    std::array<char, MAX_PATH> buf;
    GetModuleFileName(nullptr, buf.data(), buf.size());
    PathRemoveFileSpec(buf.data());
    if (PathFileExists(
          (std::string(buf.data()) + "\\_InstSetup\\usb.dat").c_str())) {
      if (!liesDatenverzeichnis("DatenVerzeichnis")) {
        fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
        return 0;
      }
    } else {
      if (!liesDatenverzeichnis("DatenVerzeichnisSteam")) {
        fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
        return 0;
      }
    }
  } else if (hatDatenverzeichnisRegulaer) {
    if (!liesDatenverzeichnis("DatenVerzeichnis")) {
      fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
      return 0;
    }
  } else if (hatDatenverzeichnisSteam) {
    if (!liesDatenverzeichnis("DatenVerzeichnisSteam")) {
      fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
      return 0;
    }
  } else {
    fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
    return 0;
  }

  fprintf(stderr, "Zielverzeichnis: %s\n", g_zielverzeichnis);

  if (!PathAppend(g_zielverzeichnis, zielverzeichnis)) {
    fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
    return 0;
  }

  if (!PathAppend(g_zielverzeichnis, "Hektometertafeln")) {
    fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
    return 0;
  }

  // Der an Zusi zurueckgegebene Pfad soll keinen fuehrenden Backslash
  // enthalten
  if (g_zielverzeichnis[g_zusi_datenpfad_laenge - 1] != '\\') {
    g_zusi_datenpfad_laenge += 1;
  }

  return 1;
}

DLL_EXPORT const char*
dllVersion()
{
  return "0.0.4";
}

DLL_EXPORT const char*
Autor()
{
  return "Zusitools";
}

DLL_EXPORT const char*
Bezeichnung()
{
#ifdef LGV
  return "Borne kilometrique LGV";
#else
  return "Hektometertafeln (DR)";
#endif
}

DLL_EXPORT float
AbstandTafeln()
{
#ifdef LGV
  return 1000.0;
#else
  return 200.0;
#endif
}

DLL_EXPORT float
AbstandGleis(uint8_t modus)
{
  if (static_cast<Standort>(modus) == Standort::kEigenerStandort) {
    return 3.0;
  } else {
    return 4.0;
  }
}

DLL_EXPORT const char*
Gruppe()
{
  return "Hektometertafeln";
}

DLL_EXPORT void
Config(HWND appHandle)
{
  return;
}

const char*
GetDateiname(
#ifdef LGV
  Standort standort,
#endif
  Kilometrierung kilometrierung)
{
#ifdef LGV
  snprintf(g_outDatei,
           sizeof(g_outDatei) / sizeof(g_outDatei[0]),
           "%s\\Borne_%skm_%d.ls3",
           g_zielverzeichnis,
           standort == Standort::kEigenerStandort ? "" : "p_",
           std::abs(kilometrierung.km));
#else
  snprintf(g_outDatei,
           sizeof(g_outDatei) / sizeof(g_outDatei[0]),
           "%s\\Hekto_DR_Mast_beids_%d_%d.ls3",
           g_zielverzeichnis,
           std::abs(kilometrierung.km),
           std::abs(kilometrierung.hm));
#endif
  return g_outDatei;
}

// "name": kein abschliessender Slash/Backslash
bool
CreateDirectoryWithParents(std::string name)
{
  const auto attrib = GetFileAttributes(name.c_str());
  if (attrib == INVALID_FILE_ATTRIBUTES) {
    static const std::string separators{ "/\\" };
    const auto lastSlashPos = name.find_last_of(separators);
    if (lastSlashPos == std::string::npos) {
      return false;
    }
    if (!CreateDirectoryWithParents(name.substr(0, lastSlashPos))) {
      return false;
    }
    return CreateDirectory(name.c_str(), nullptr);
  } else {
    return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0 ||
           (attrib & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
  }
}

DLL_EXPORT uint8_t
Erzeugen(float wert_m, uint8_t modus, const char** datei)
{
  *datei = nullptr;

  const auto km_basis = Kilometrierung::fromMeter(wert_m);

  const auto standort = static_cast<Standort>(modus);
#ifndef LGV
  if (standort != Standort::kEigenerStandort) {
    fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
    return 0;
  }
#endif

  *datei = GetDateiname(
#ifdef LGV
             standort,
#endif
             km_basis) +
           g_zusi_datenpfad_laenge;
  if (!CreateDirectoryWithParents(g_zielverzeichnis)) {
    fprintf(stderr, "failed at %s:%d\n", __FILE__, __LINE__);
    return 0;
  }

  FILE* fd = fopen(g_outDatei, "w");
  assert(fd != nullptr);
  HektoBuilder::Build(fd,
#ifdef LGV
                      standort,
#endif
                      km_basis);
  fclose(fd);

  return 1;
}
