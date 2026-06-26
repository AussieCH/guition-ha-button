# Guition Knob – Wohnzimmer-Steuerung (Home Assistant)

ESPHome-Firmware für den **Guition JC3636K718C** (runder 1.8″ Knob-Screen,
ESP32-S3) als Steuer-Panel fürs Wohnzimmer. Bedienung über **Drehrad**,
**Touch-Gesten** und **On-Screen-Buttons**; Tonausgabe über den internen
Lautsprecher; LED-Ring als Statusanzeige.

Alles läuft über die **ESPHome-API direkt mit Home Assistant** – kein Cloud-Dienst.

## Kacheln (11)

Horizontal wischen wechselt die Kachel (Punkte unten = aktive Kachel). Nach dem Boot
ist die **Uhr** aktiv (Position 0).

| # | Kachel | Knob drehen | Tippen | Mittel-Button |
|---|--------|-------------|--------|---------------|
| 0 | **Uhr / Home** | – | – | – |
| 1 | **Timer** | Zeit / durch Timer scrollen | – | ＋ hinzufügen · ♪ Ton |
| 2 | **Apple TV** | Lautstärke ± | – | ▶❚❚ Play/Pause · ⏻ Power |
| 3 | **Ventilator** | Stufe 0–6 (≈4 Klicks/Stufe) | – | ↺ Drehrichtung umkehren |
| 4 | **Storen TV** | – | – | ▲ Öffnen / ▼ Schließen |
| 5 | **Licht (alle)** | Master-Helligkeit (1,5 Umdr.) | alle an/aus | – |
| 6 | **Gradient** | Helligkeit ±5 % | an/aus | 🎨 Farbmenü |
| 7 | **Lightstrip** | Helligkeit ±5 % | an/aus | 🎨 Farbmenü |
| 8 | **Stehlampe** | Helligkeit ±5 % | an/aus | 🎨 Farbmenü |
| 9 | **System** | Display-Helligkeit | – | – |
| 10 | **Flugradar** | Radius 10–50 km | Flieger → Details | – |

### Gesten

- **Horizontal wischen** → Kachel wechseln
- **Nach unten wischen** (auf einer Licht-Kachel) → Farbmenü
- **Nach oben wischen** (überall) → Liste der aktiven Timer
- In den Untermenüs (Farbe / Timer-Liste / Flug-Details): **wischen/tippen** = zurück

## Die Kacheln im Detail

### Uhr / Home (Startseite)
Wochentag + Datum, große Uhrzeit (HA-Zeit) und zwei Temperaturen:
**Balkon** (`sensor.wohnzimmer_garten_im_schatten_2m_temperatur`) und
**Wohnzimmer** (`sensor.wohnzimmer_temperatur`).

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
- **Licht (alle)**: absoluter Master-Dimmer – Knob setzt alle Lampen smooth auf denselben
  Wert (analog zur System-Helligkeit, ~1,5 Umdrehungen), Tippen schaltet alle an/aus.

Helligkeit/Status werden live aus HA gelesen.

### Apple TV
`media_player.wohnzimmer_2` + `remote.wohnzimmer`.
- **Play/Pause** (ein Toggle-Button), **Lautstärke** über den Knob.
- **Power**: `remote.send_command` mit `wakeup` / `suspend` (das `media_player`-Entity
  selbst schaltet nie aus).
- Status (Läuft / Pause / Aus) kommt aus dem `media_player` – Pause wird hier als
  `idle` gemeldet.

### System
Diagnose: freier Heap, Loop-Zeit (Last-Indikator), **Akku** (ADC auf GPIO6, grobe
%-Schätzung; Lade-Erkennung heuristisch über die Spannung → Akku-Icon wird zum
Lade-Icon) und **Display-Helligkeit** (10–100 %, per Knob über 1,5 Umdrehungen, bleibt
über Kacheln/Sleep/Reboot erhalten).

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

### Flugradar (OpenSky)
Radar-Ansicht der Flugzeuge im Umkreis (Live-Daten von [OpenSky](https://opensky-network.org/)):
- Zuhause im Zentrum, Entfernungsringe, Norden oben; Flugzeug-Symbole **in Flugrichtung
  gedreht** mit Callsign.
- **Knob** = Radius **10–50 km**; **Tippen** auf einen Flieger → Details (Höhe, Speed,
  Kurs, Distanz), Tippen schließt wieder.
- Auth via **OAuth2** (`client_credentials`); Abruf alle **5 s**, aber nur wenn die
  Radar-Kachel aktiv **und** das Display wach ist (schont das OpenSky-Kontingent).
- Keine echten Kartenkacheln (auf dem ESP nicht praktikabel) – bewusst ein Radar-View.

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

1. **Entity-IDs / Werte** in [`guition-button.yaml`](guition-button.yaml) unter
   `substitutions:` anpassen (Lüfter-Scripts, Storen, 3 Hue-Lampen, Apple-TV-Entities,
   Temperatur-Sensoren, Home-Koordinaten fürs Flugradar).
2. **Secrets anlegen:**
   ```bash
   cp secrets.yaml.example secrets.yaml
   openssl rand -base64 32   # taugt als api_key
   # WLAN + api_key + ota_password eintragen
   ```
   Fürs **Flugradar** zusätzlich `opensky_auth` (Basic) oder ein OAuth2-Token-Body
   (`opensky_oauth_body`, `client_credentials`) aus einem kostenlosen
   [OpenSky](https://opensky-network.org/)-Account.
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

## Board-Referenz

Pinout, ESPHome-Komponenten-Wahl und die wichtigsten Gotchas für dieses Board sind in
[`docs/guition-jc3636k718c.md`](docs/guition-jc3636k718c.md) gesammelt (Knob ist kein
Quadratur-Encoder, Strapping-Pins, 16-MB-Flash, LVGL-Performance, Lambda-Fallen …).

---

*Ursprüngliche Board-Referenz / Voice-Assistant-Repo: [esphome-guition-jc3636k718c-va](https://github.com/MichalZaniewicz/esphome-guition-jc3636k718c-va).*
