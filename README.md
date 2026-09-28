# Effektivisering av drivhus


Dette repoet inneholder koden til en sensorprototype som overvåker klimaet i et hobbydrivhus. Systemet består av to Arduino-kort med hver sin oppgave:

| Fil | Kort | Oppgave |
| --- | --- | --- |
| `TEL100_UNO_Kode.ino` | Arduino UNO | Leser sensorene og sender målingene til MKR |
| `TEL100_MKR_Kode.ino` | Arduino MKR WiFi 1010 | Mottar målingene og sender dem til Arduino IoT Cloud |

Oppdelingen ble gjort fordi SEN55 (I²C) og Arduino IoT Cloud på samme kort ga kommunikasjonsfeil.

```
Sensorer → Arduino UNO → (seriell) → Arduino MKR WiFi 1010 → Arduino IoT Cloud → Arduino IoT Remote
```

---

## TEL100_UNO_Kode.ino – sensoravlesning

Koden kjører på Arduino UNO og gjør følgende:

1. **Oppstart:** starter Serial Monitor (115200 baud), seriell kommunikasjon mot MKR (SoftwareSerial på pinne 2 og 3, 9600 baud) og I²C. SEN55 nullstilles og måling startes. Eventuelle feil skrives til Serial Monitor.
2. **Leser SEN55** (via I²C): temperatur, luftfuktighet, PM1.0, PM2.5, PM4.0, PM10, VOC-indeks og NOx-indeks. Hvis lesingen feiler, skrives en feilmelding og runden hoppes over.
3. **Leser jordfuktighet** (kapasitiv sensor på A0) og klassifiserer verdien:
   - under 277: "TooWet"
   - 277–379: "Perfect"
   - 380 eller høyere: "TooDry"
4. **Leser lysnivå** (fotoresistor på A1) som en rå analogverdi (0–1023).
5. **Sender til MKR** som én tekstlinje med fast format, avsluttet med linjeskift:
   ```
   T:22.7,H:51.2,P1:4.0,P25:4.3,P4:4.4,P10:4.5,VOC:66.0,NOX:2.0,Soil:385,Status:TooDry,Light:929
   ```
6. **Skriver alle verdiene** i Serial Monitor for feilsøking.
7. Venter 1 sekund og gjentar.

### Pinner (UNO)

| Sensor / kort | Pinne |

| Jordfuktighetssensor | A0 |
| Fotoresistor (lys) | A1 |
| SEN55 (I²C) | A4 (SDA), A5 (SCL) |
| Til MKR (TX) | 3 |

### Bibliotek

- SensirionI2CSen5x
- Wire og SoftwareSerial (følger med Arduino)

---

## TEL100_MKR_Kode.ino – kommunikasjon til skyen

Koden kjører på Arduino MKR WiFi 1010 og gjør følgende:

1. **Oppstart:** starter Serial Monitor, seriell kommunikasjon mot UNO (`Serial1`, 9600 baud) og kobler til Arduino IoT Cloud.
2. **Mottar data:** leser tegn fra UNO til den får et linjeskift. Da regnes hele meldingen som mottatt.
3. **Tolker meldingen** (`parseSensorData` og `getValue`): plukker ut hver enkelt verdi fra strengen ved å lete etter nøkler som `T:`, `H:`, `VOC:` og `Soil:`.
4. **Lager en lesbar tekst** med alle sju parameterne og lagrer den i Cloud-variabelen `sensorData`:
   ```
   Temperature: 22.7 C
   Humidity: 51.2 %
   PM1.0: 4.0 ug/m3
   PM2.5: 4.3 ug/m3
   PM4.0: 4.4 ug/m3
   PM10: 4.5 ug/m3
   VOC: 66.0
   NOx Index: 2.0
   Soil moisture: 385
   Soil status: TooDry
   Light: 929
   --------------------
   ```
5. **Synkroniserer med skyen** via `ArduinoCloud.update()`. Brukeren ser verdiene i appen Arduino IoT Remote.
6. **Skriver mottatte data** i Serial Monitor for feilsøking.

### Merk

MKR-koden bruker "thingProperties.h", som definerer Cloud-variabelen "sensorData" og WiFi-oppsettet. Denne filen genereres av Arduino IoT Cloud. Du trenger også en egen "arduino_secrets.h" med WiFi-opplysningene dine hvis du skal kjøre koden selv.

---

## Målte parametere

Temperatur, luftfuktighet, jordfuktighet, lysintensitet, VOC, NOx og luftpartikler (PM1.0, PM2.5, PM4.0, PM10).