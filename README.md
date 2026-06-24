# Guition Knob – Wohnzimmer-Steuerung (Home Assistant)

ESPHome-Firmware für den **Guition JC3636K718C** (runder 1.8" Knob-Screen,
ESP32-S3). Ein schlankes Steuer-Panel für drei Entitäten im Wohnzimmer:

| Kachel | Tippen | Knob drehen |
|--------|--------|-------------|
| **Ventilator** | An/Aus (`fan.toggle`) | Drehzahl 0–100 % |
| **Storen** (Garten) | Auf / Zu (je nach Position) | Position 0–100 % |
| **Licht** | An/Aus (`light.toggle`) | Helligkeit 0–100 % |

**Horizontal wischen** wechselt die Kachel (3 Punkte unten zeigen die aktive an).

## Einrichten

1. **Entity-IDs eintragen** in [`guition-button.yaml`](guition-button.yaml) unter
   `substitutions:` (`fan_entity`, `cover_entity`, `light_entity`).
2. **Secrets anlegen:**
   ```bash
   cp secrets.yaml.example secrets.yaml
   # WLAN + frei gewählte api_key / ota_password eintragen
   openssl rand -base64 32   # taugt als api_key
   ```
3. **Erster Flash MUSS über USB** erfolgen (die 16-MB-Partitionstabelle lässt sich
   nicht per OTA einspielen):
   ```bash
   esphome run guition-button.yaml
   ```
   Danach funktioniert OTA über WLAN.

## Hinweise / Eigenheiten dieses Boards

- **Strapping-Warnungen** für GPIO0 (LED-Ring) sind normal und harmlos.
- **Knob ist kein Quadratur-Encoder** – Richtung steckt darin, *welcher* Pin pulst
  (links GPIO2, rechts GPIO1). Deshalb zwei `binary_sensor`, kein `rotary_encoder`.
- **Knob lässt sich nicht drücken** (GPIO0 = BOOT) – geschaltet wird per Tippen.
- **OTA-Rollback:** nach dem Flashen das Gerät ~1 Min laufen lassen, bevor du es
  neu startest (esp-idf markiert das neue Image erst dann als „gut"). `safe_mode:`
  ist aktiv.
- Build-Host braucht beim Kompilieren **Internet** (Google Fonts + MDI-Icons werden
  geladen und in die Firmware gebacken; zur Laufzeit nichts davon nötig).

## Logs streamen

API ist hier verschlüsselt; am einfachsten:
```bash
esphome logs guition-button.yaml
```
