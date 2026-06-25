# Guition Knob – Wohnzimmer-Steuerung (Home Assistant)

ESPHome-Firmware für den **Guition JC3636K718C** (runder 1.8″ Knob-Screen,
ESP32-S3) als Steuer-Panel fürs Wohnzimmer. Bedienung über **Drehrad**,
**Touch-Gesten** und **On-Screen-Buttons**; Tonausgabe über den internen
Lautsprecher; LED-Ring als Statusanzeige.

Alles läuft über die **ESPHome-API direkt mit Home Assistant** – kein Cloud-Dienst.

## Kacheln (9)

Horizontal wischen wechselt die Kachel (Punkte unten = aktive Kachel).

| # | Kachel | Knob drehen | Tippen | Mittel-Button |
|---|--------|-------------|--------|---------------|
| 0 | **Ventilator** | Stufe 0–6 (≈4 Klicks/Stufe) | – | ↺ Drehrichtung umkehren |
| 1 | **Storen TV** | – | – | ▲ Öffnen / ▼ Schließen (2 Buttons) |
| 2 | **Licht (alle)** | alle heller/dunkler (relativ) | alle an/aus | – |
| 3 | **Gradient** | Helligkeit ±5 % | an/aus | 🎨 Farbmenü |
| 4 | **Lightstrip** | Helligkeit ±5 % | an/aus | 🎨 Farbmenü |
| 5 | **Stehlampe** | Helligkeit ±5 % | an/aus | 🎨 Farbmenü |
| 6 | **Apple TV** | Lautstärke ± | – | ▶❚❚ Play/Pause · ⏻ Power |
| 7 | **System** | – | – | – |
| 8 | **Timer** | Zeit / durch Timer scrollen | – | ＋ hinzufügen · ♪ Ton |

### Gesten

- **Horizontal wischen** → Kachel wechseln
- **Nach unten wischen** (auf einer Licht-Kachel) → Farbmenü
- **Nach oben wischen** (überall) → Liste der aktiven Timer
- In den Untermenüs (Farbe / Timer-Liste): **wischen** = zurück

## Die Kacheln im Detail

### Ventilator (RF über HA-Scripts)
Kein `fan`-Entity – der Lüfter wird per Funk über HA-Scripts geschaltet, die Ist-Stufe
kommt aus `input_number.fan_wohnzimmer_speed`. ≈4 Knob-Klicks = eine Stufe (passend zum
~300°-Bogen). Mittel-Button kehrt die Drehrichtung um.

| Stufe | Script | | Stufe | Script |
|---|---|---|---|---|
| 0 | `…_stop` | | 4 | `…_speed_3_duplizieren` |
| 1 | `…_speed_1_t` | | 5 | `…_speed_5` |
| 2 | `…_speed_2` | | 6 | `…_speed_6` |
| 3 | `…_speed_3` | | ↺ | `…_umkehren` |

### Storen TV
`cover.store_wohnzimmer_sud`, nur **Auf/Zu** über zwei Buttons (kein Knob, keine
Kippstellung). Der Ring zeigt die aktuelle Position.

### Licht – 3× Philips Hue + Sammel-Kachel
Lampen: `light.gradient_lightstrip_wohnzimmer`, `light.lightstrip_wohnzimmer`,
`light.stehlampe_wohnzimmer`.
- **Einzelkacheln**: Tippen = an/aus, Knob = Helligkeit ±5 %.
- **Farbmenü** (runter wischen *oder* Paletten-Button): Knob dreht den **Farbton** (Hue),
  der Strip ändert die Farbe live (`rgb_color`). Hoch wischen = zurück.
- **Licht (alle)**: Master-Dimmer – Knob macht alle eingeschalteten Lampen **relativ**
  heller/dunkler (jede ab ihrem eigenen Wert), Tippen schaltet alle an/aus.

Helligkeit/Status werden live aus HA gelesen.

### Apple TV
`media_player.wohnzimmer_2` + `remote.wohnzimmer`.
- **Play/Pause** (ein Toggle-Button), **Lautstärke** über den Knob.
- **Power**: `remote.send_command` mit `wakeup` / `suspend` (das `media_player`-Entity
  selbst schaltet nie aus).
- Status (Läuft / Pause / Aus) kommt aus dem `media_player` – Pause wird hier als
  `idle` gemeldet.

### System
Diagnose: freier Heap, Loop-Zeit (Last-Indikator) und **Akku** (ADC auf GPIO6, grobe
%-Schätzung – Kurve ggf. pro Gerät kalibrieren).

### Timer (bis zu 5 parallel)
- **Zeit einstellen**: Paletten-Button **＋** tippen → Add-Modus, Knob stellt 0–180 min,
  Button wird **✓** → bestätigen fügt den Timer hinzu.
- **Scrollen**: ohne Add-Modus scrollt der Knob durch die laufenden Timer (Anzeige zeigt
  den Countdown des gewählten).
- **Ton**: ♪-Button wählt eine von drei Melodien (Vorschau): *Flight of the Bumblebee*,
  *Eye of the Tiger*, *Für Elise*.
- **Ablauf**: gewählte Melodie wiederholt sich **und** der LED-Ring blinkt – beides bis
  aufs Display getippt wird.
- **Farben**: jeder der 5 Timer hat eine eigene Farbe (rot · orange · gelb · grün · blau).
  Die Farbe färbt **Uhr-Icon, Display-Ring und LED-Ring-Blinken** des jeweiligen Timers.
- **Übersicht/Abbruch**: nach oben wischen → Liste aller aktiven Timer, je mit ✕.

### Display-Sleep
Nach **60 s ohne Bedienung** geht das Backlight aus; Touch oder Knob weckt wieder auf
(die Weck-Berührung löst keine Aktion aus).

## Hardware-Features genutzt

- **Display** ST77916 QSPI 360×360 + **LVGL**
- **Touch** CST816
- **Drehrad** (kein Quadratur-Encoder – Richtung = welcher Pin pulst, GPIO1/2)
- **LED-Ring** WS2812 (13 LED, GPIO0) – Timer-Alarm-Blinken
- **Audio** PCM5100A I²S-DAC + Speaker (Timer-Töne via `rtttl`), DAC-Mute GPIO46
- **Akku-ADC** GPIO6

## Einrichten

1. **Entity-IDs** in [`guition-button.yaml`](guition-button.yaml) unter `substitutions:`
   anpassen (Lüfter-Scripts, Storen, 3 Hue-Lampen, Apple-TV-Entities).
2. **Secrets anlegen:**
   ```bash
   cp secrets.yaml.example secrets.yaml
   openssl rand -base64 32   # taugt als api_key
   # WLAN + api_key + ota_password eintragen
   ```
3. **Erster Flash MUSS über USB** (die 16-MB-Partitionstabelle lässt sich nicht per OTA
   einspielen):
   ```bash
   esphome run guition-button.yaml
   ```
   Danach laufen Updates **per OTA** über WLAN:
   ```bash
   esphome run guition-button.yaml --device <ip-oder-hostname>
   ```
4. In Home Assistant das entdeckte ESPHome-Gerät hinzufügen (mit dem `api_key`) und
   **„Dem Gerät erlauben, Home-Assistant-Aktionen auszuführen"** aktivieren – sonst
   reagieren Tasten/Knob nicht.

## Eigenheiten / Lessons learned

- **Erstes Flashen nur per USB.** Wenn der Knob über USB allein nicht sauber bootet
  (Display dunkel, kein USB-Port), hilft ein **Akku** als Stromquelle. BOOT = Pinhole
  links neben USB-C. Nach dem Flash **kalt starten ohne BOOT** zu drücken.
- **HA-Aktionen müssen freigegeben werden** (siehe Einrichten Schritt 4).
- **Strapping-Warnungen** (GPIO0/3/45/46) sind normal und harmlos.
- **OTA-Rollback:** Gerät nach dem Flashen ~1 Min laufen lassen (`safe_mode:` aktiv).
- Der **Build-Host braucht Internet** (Google Fonts + MDI-Icons werden beim Kompilieren
  geladen und in die Firmware gebacken).
- **Akku-Anzeige** ist eine grobe Heuristik – bei Bedarf die Spannungs-Kurve anpassen.

## Logs streamen

```bash
esphome logs guition-button.yaml --device <ip-oder-hostname>
```

---

*Board-Referenz und Pinout: [esphome-guition-jc3636k718c-va](https://github.com/MichalZaniewicz/esphome-guition-jc3636k718c-va).*
