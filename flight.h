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

// Bildschirm-Positionen der angezeigten Marker (für Tipp-Trefferprüfung)
inline int g_shown_n = 0;
inline int16_t g_shown_x[10];   // Offset von der Mitte (px)
inline int16_t g_shown_y[10];
inline int g_shown_pi[10];      // Index in g_planes

// Ausgewähltes Flugzeug (Detail-Overlay)
inline Aircraft g_sel_ac;
inline std::string g_sel_cs;    // Callsign zum Wiederfinden nach jedem Abruf

// OpenSky OAuth2 (Bearer)
inline std::string g_token;
inline std::string g_authhdr;     // "Bearer <token>" (persistent für den Header-Lambda)
inline uint32_t g_token_exp = 0;  // millis, ab wann der Token erneuert werden muss
