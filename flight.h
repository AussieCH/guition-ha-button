#pragma once
#include <string>
#include <vector>
#include <algorithm>

// Ein Flugzeug aus den OpenSky-Statusdaten (auf das reduziert, was die Radar-Page braucht)
struct Aircraft {
  std::string cs;   // Callsign
  float lat = 0;    // Breitengrad
  float lon = 0;    // Längengrad
  float alt = 0;    // Höhe (m)
  float trk = 0;    // Kurs (Grad)
  float vel = 0;    // Geschwindigkeit (m/s)
  float dist = 0;   // Distanz zum Zuhause (km)
  bool gnd = false; // am Boden
};

// Globale Flugzeug-Liste (direkt im Header, damit sie vor den ESPHome-Globals existiert)
inline std::vector<Aircraft> g_planes;
