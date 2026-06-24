# Guition Knob – Wohnzimmer-Steuerung (Home Assistant)

ESPHome-Firmware für den **Guition JC3636K718C** (runder 1.8" Knob-Screen,
ESP32-S3). Ein schlankes Steuer-Panel fürs Wohnzimmer mit **6 Kacheln**:

| Kachel | Tippen | Knob drehen | Mittel-Button |
|--------|--------|-------------|---------------|
| **Ventilator** | – | Stufe ±1 (Stop, 1–6) | Drehrichtung umkehren |
| **Storen** | Auf / Zu | rechts = auf, links = zu | – |
| **Licht (alle)** | alle an/aus | Helligkeit ±10 % (alle) | Farb-Preset weiter |
| **Gradient** | an/aus | Helligkeit ±10 % | Farb-Preset weiter |
| **Lightstrip** | an/aus | Helligkeit ±10 % | Farb-Preset weiter |
| **Stehlampe** | an/aus | Helligkeit ±10 % | Farb-Preset weiter |

**Horizontal wischen** wechselt die Kachel (Punkte unten zeigen die aktive an).
Der **Mittel-Button** (auf Ventilator + Licht-Kacheln) hat seinen eigenen Druck;
ein Tipp dort schaltet die Lampe *nicht* mit.

### Licht (3× Philips Hue)

Drei Hue-Leuchten: `light.gradient_lightstrip_wohnzimmer`,
`light.lightstrip_wohnzimmer`, `light.stehlampe_wohnzimmer`. Die **Sammel-Kachel
„Licht (alle)"** steuert alle drei gemeinsam (eine Komma-Entity-Liste), die drei
Einzelkacheln je eine Lampe. Tippen = an/aus, Dial = Helligkeit, Mittel-Button =
nächstes **Farb-Preset** (Warmweiß → Orange → Blau → Lila → Orchidee → Tomate, per
`color_name`). Helligkeit/Status werden live aus HA gelesen (`brightness`-Attribut).

### Ventilator (RF über HA-Scripts)

Der Ventilator hat **kein `fan`-Entity** und keinen Rückkanal — er wird per Funk
über HA-Scripts geschaltet. Die aktuelle Stufe wird **lokal auf dem Gerät** gehalten
(0 = Stop, 1–6), Dial im Uhrzeigersinn erhöht, gegen den Uhrzeigersinn senkt. Pro
Stufenwechsel wird `script.turn_on` mit dem passenden Script gerufen:

| Stufe | Script | | Stufe | Script |
|---|---|---|---|---|
| 0 | `…_stop` | | 4 | `…_speed_3_duplizieren` |
| 1 | `…_speed_1_t` | | 5 | `…_speed_5` |
| 2 | `…_speed_2` | | 6 | `…_speed_6` |
| 3 | `…_speed_3` | | ↺ | `…_umkehren` (Button) |

Die Script-Namen sind in `act_fan_apply` / `act_fan_reverse` in
[`guition-button.yaml`](guition-button.yaml) hinterlegt. Da kein Rückkanal existiert,
kann die Anzeige von der Realität abweichen, wenn der Lüfter anderswo geschaltet wird.

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
