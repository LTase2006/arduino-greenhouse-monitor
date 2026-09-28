// Grunnleggende Arduino-bibliotek
#include <Arduino.h> 

// Bibliotek for I2C-kommunikasjon
#include <Wire.h> 

// Lager en ekstra seriell forbindelse
#include <SoftwareSerial.h> 

// Bibliotek for SEN55-sensoren
#include <SensirionI2CSen5x.h> 

// Lager objekt for SEN55-sensoren
SensirionI2CSen5x sen5x; 

// Seriell kommunikasjon med MKR på pinne 2 og 3
SoftwareSerial MKRSerial(2, 3); 


// Pinne for jordfuktighet
#define SOIL_PIN A0 

// Grense for våt jord
#define wetSoil 277 

// Grense for tørr jord
#define drySoil 380 

// Pinne for lyssensor
#define LIGHT_PIN A1 


void setup() { 

  // Starter Serial Monitor
  Serial.begin(115200); 


  // Starter kommunikasjonen med MKR
  MKRSerial.begin(9600); 

  // Starter I2C-kommunikasjon
  Wire.begin(); 

  // Starter SEN55-sensoren
  sen5x.begin(Wire); 

  // Setter sensorpinnene som innganger
  pinMode(SOIL_PIN, INPUT); 
  pinMode(LIGHT_PIN, INPUT); 

  // Variabler som brukes til feilmeldinger
  uint16_t error; 
  char errorMessage[256]; 


  // Nullstiller SEN55-sensoren
  error = sen5x.deviceReset(); 

  // Sjekker om det oppstod en feil
  if (error) { 

    // Gjør feilkoden om til tekst
    errorToString( 
      error, 
      errorMessage, 
      sizeof(errorMessage) 
    ); 

    // Skriver feilen i Serial Monitor
    Serial.print("SEN55 reset error: "); 
    Serial.println(errorMessage); 
  } 

  // Kort pause etter reset
  delay(100); 


  // Starter måling med SEN55
  error = sen5x.startMeasurement(); 

  // Sjekker om oppstarten feilet
  if (error) { 

    // Gjør feilkoden om til tekst
    errorToString( 
      error, 
      errorMessage, 
      sizeof(errorMessage) 
    ); 

    // Skriver feilmeldingen
    Serial.print("SEN55 start error: "); 
    Serial.println(errorMessage); 

  } else { 

    // Skriver at sensoren startet riktig
    Serial.println("SEN55 started successfully"); 
  } 

  // Venter litt før programmet fortsetter
  delay(1500); 

  Serial.println(); 
  Serial.println("UNO sensor system started"); 
} 


void loop() { 

  // Variabler for feilinformasjon
  uint16_t error; 
  char errorMessage[256]; 



  // Variabler for partikkelmålinger
  float pm1; 
  float pm25; 
  float pm4; 
  float pm10; 

  // Variabler for luftfuktighet og temperatur
  float humidity; 
  float temperature; 

  // Variabler for VOC og NOx
  float voc; 
  float nox; 


  // --------------------------------- 
  // Read SEN55 
  // --------------------------------- 

  // Leser alle måleverdiene fra SEN55
  error = sen5x.readMeasuredValues( 
    pm1, 
    pm25, 
    pm4, 
    pm10, 
    humidity, 
    temperature, 
    voc, 
    nox 
  ); 


  // Sjekker om lesingen feilet
  if (error) { 

    // Gjør feilkoden om til tekst
    errorToString( 
      error, 
      errorMessage, 
      sizeof(errorMessage) 
    ); 

    // Skriver feilen
    Serial.print("SEN55 error: "); 
    Serial.println(errorMessage); 

    // Venter ett sekund
    delay(1000); 

    // Stopper denne runden av loop
    return; 
  } 



  // Leser verdien fra jordfuktighetssensoren
  int moisture = analogRead(SOIL_PIN); 

  // Lagrer statusen til jorda
  String moistureStatus; 


  // Sjekker om jorda er for våt
  if (moisture < wetSoil) { 

    moistureStatus = "TooWet"; 

  } 
  // Sjekker om jordfuktigheten er passende
  else if (moisture < drySoil) { 

    moistureStatus = "Perfect"; 

  } 
  // Hvis ikke, er jorda for tørr
  else { 

    moistureStatus = "TooDry"; 
  } 


  // Leser verdien fra lyssensoren
  int lightValue = analogRead(LIGHT_PIN); 


  // Sender temperatur til MKR
  MKRSerial.print("T:"); 
  MKRSerial.print(temperature, 1); 

  // Sender luftfuktighet
  MKRSerial.print(",H:"); 
  MKRSerial.print(humidity, 1); 

  // Sender PM1.0
  MKRSerial.print(",P1:"); 
  MKRSerial.print(pm1, 1); 

  // Sender PM2.5
  MKRSerial.print(",P25:"); 
  MKRSerial.print(pm25, 1); 

  // Sender PM4.0
  MKRSerial.print(",P4:"); 
  MKRSerial.print(pm4, 1); 

  // Sender PM10
  MKRSerial.print(",P10:"); 
  MKRSerial.print(pm10, 1); 

  // Sender VOC
  MKRSerial.print(",VOC:"); 
  MKRSerial.print(voc, 1); 

  // Sender NOx
  MKRSerial.print(",NOX:"); 
  MKRSerial.print(nox, 1); 

  // Sender jordfuktigheten
  MKRSerial.print(",Soil:"); 
  MKRSerial.print(moisture); 

  // Sender statusen til jorda
  MKRSerial.print(",Status:"); 
  MKRSerial.print(moistureStatus); 

  // Sender lysverdien og avslutter linjen
  MKRSerial.print(",Light:"); 
  MKRSerial.println(lightValue); 


  // Skriver alle sensorverdiene i Serial Monitor
  Serial.println(); 
  Serial.println("========== SENSOR DATA =========="); 


  // Skriver temperatur
  Serial.print("Temperature:    "); 
  Serial.print(temperature, 1); 
  Serial.println(" C"); 


  // Skriver luftfuktighet
  Serial.print("Humidity:       "); 
  Serial.print(humidity, 1); 
  Serial.println(" %"); 


  Serial.println(); 


  // Skriver PM1.0
  Serial.print("PM1.0:          "); 
  Serial.print(pm1, 1); 
  Serial.println(" ug/m3"); 


  // Skriver PM2.5
  Serial.print("PM2.5:          "); 
  Serial.print(pm25, 1); 
  Serial.println(" ug/m3"); 


  // Skriver PM4.0
  Serial.print("PM4.0:          "); 
  Serial.print(pm4, 1); 
  Serial.println(" ug/m3"); 


  // Skriver PM10
  Serial.print("PM10:           "); 
  Serial.print(pm10, 1); 
  Serial.println(" ug/m3"); 


  Serial.println(); 


  // Skriver VOC-indeksen
  Serial.print("VOC Index:      "); 
  Serial.println(voc, 1); 


  // Skriver NOx-indeksen
  Serial.print("NOx Index:      "); 
  Serial.println(nox, 1); 


  Serial.println(); 


  // Skriver jordfuktigheten
  Serial.print("Soil moisture:  "); 
  Serial.println(moisture); 


  // Skriver statusen til jorda
  Serial.print("Soil status:    "); 
  Serial.println(moistureStatus); 


  // Skriver lysverdien
  Serial.print("Light:          "); 
  Serial.println(lightValue); 


  Serial.println("================================="); 


  // Venter ett sekund før neste måling
  delay(1000); 
}