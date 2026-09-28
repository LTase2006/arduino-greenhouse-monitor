#include "thingProperties.h" 

// Lagrer data som kommer fra UNO
String incomingData = ""; 

void setup() { 
  // Starter seriell kommunikasjon med PC
  Serial.begin(115200); 


  // Starter kommunikasjon med UNO
  Serial1.begin(9600); 

  // Venter litt før programmet fortsetter
  delay(1500); 

  // Skriver informasjon i Serial Monitor
  Serial.println("MKR started"); 
  Serial.println("Waiting for sensor data from UNO..."); 


  // Starter variablene som brukes i Arduino Cloud
  initProperties(); 

  // Kobler MKR til Arduino Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection); 

  // Setter nivå for feilmeldinger
  setDebugMessageLevel(2); 
  ArduinoCloud.printDebugInfo(); 

  // Reserverer plass til innkommende data
  incomingData.reserve(250); 
} 

void loop() { 
  // Oppdaterer forbindelsen til Arduino Cloud
  ArduinoCloud.update(); 

  // Sjekker om det har kommet data fra UNO
  while (Serial1.available() > 0) { 
    // Leser ett tegn om gangen
    char c = Serial1.read(); 

    // Ny linje betyr at hele meldingen er mottatt
    if (c == '\n') { 
      // Fjerner unødvendige mellomrom
      incomingData.trim(); 

      // Sjekker at meldingen ikke er tom
      if (incomingData.length() > 0) { 
        // Behandler sensordataene
        parseSensorData(incomingData); 
      } 

      // Tømmer teksten før neste melding
      incomingData = ""; 
    } 
    // Legger tegn til meldingen
    else if (c != '\r') { 
      incomingData += c; 
    } 
  } 
} 

// Leser de forskjellige sensorverdiene fra meldingen
void parseSensorData(String data) { 

  // Variabler for SEN55-sensoren
  float temperature = 0; 
  float humidity = 0; 
  float pm1 = 0; 
  float pm25 = 0; 
  float pm4 = 0; 
  float pm10 = 0; 
  float voc = 0; 
  float nox = 0; 

  // Variabler for jordfuktighet og lys
  int soilMoisture = 0; 
  int lightValue = 0; 

  // Tekst som beskriver jordfuktigheten
  String soilStatus = ""; 


  // Henter temperatur
  temperature = 
    getValue(data, "T:", ",").toFloat(); 

  // Henter luftfuktighet
  humidity = 
    getValue(data, "H:", ",").toFloat(); 

  // Henter PM1.0
  pm1 = 
    getValue(data, "P1:", ",").toFloat(); 

  // Henter PM2.5
  pm25 = 
    getValue(data, "P25:", ",").toFloat(); 

  // Henter PM4.0
  pm4 = 
    getValue(data, "P4:", ",").toFloat(); 

  // Henter PM10
  pm10 = 
    getValue(data, "P10:", ",").toFloat(); 

  // Henter VOC-verdi
  voc = 
    getValue(data, "VOC:", ",").toFloat(); 

  // Henter NOx-verdi
  nox = 
    getValue(data, "NOX:", ",").toFloat(); 

  // Henter jordfuktighet
  soilMoisture = 
    getValue(data, "Soil:", ",").toInt(); 

  // Henter status for jorda
  soilStatus = 
    getValue(data, "Status:", ","); 

  // Henter lysverdien
  lightValue = 
    getValue(data, "Light:", "").toInt(); 


  // Lager én tekst med alle sensorverdiene
  sensorData = 
    "Temperature: " + String(temperature, 1) + " C\n\n" + 
    "Humidity: " + String(humidity, 1) + " %\n\n" + 
    "PM1.0: " + String(pm1, 1) + " ug/m3\n\n" + 
    "PM2.5: " + String(pm25, 1) + " ug/m3\n\n" + 
    "PM4.0: " + String(pm4, 1) + " ug/m3\n\n" + 
    "PM10: " + String(pm10, 1) + " ug/m3\n\n" + 
    "VOC: " + String(voc, 1) + "\n\n" + 
    "NOx Index: " + String(nox, 1) + "\n\n" + 
    "Soil moisture: " + String(soilMoisture) + "\n\n" + 
    "Soil status: " + soilStatus + "\n\n" + 
    "Light: " + String(lightValue) + "\n\n" + 
    "--------------------"; 

  // Skriver mottatte data i Serial Monitor
  Serial.println(); 
  Serial.println("===== DATA RECEIVED FROM UNO ====="); 

  Serial.println(sensorData); 

  Serial.println("=================================="); 
} 


// Funksjon som finner en bestemt verdi i teksten
String getValue( 
  String data, 
  String startText, 
  String endText 
) { 

  // Finner hvor ønsket tekst starter
  int startPosition = data.indexOf(startText); 

  // Returnerer tom tekst hvis verdien ikke finnes
  if (startPosition == -1) { 
    return ""; 
  } 

  // Flytter posisjonen til starten av selve verdien
  startPosition += startText.length(); 

  // Hvis det ikke finnes en slutttekst, les resten
  if (endText == "") { 
    return data.substring(startPosition); 
  } 

  // Finner hvor verdien slutter
  int endPosition = 
    data.indexOf(endText, startPosition); 

  // Leser resten hvis sluttpunktet ikke finnes
  if (endPosition == -1) { 
    return data.substring(startPosition); 
  } 

  // Returnerer bare verdien mellom start og slutt
  return data.substring( 
    startPosition, 
    endPosition 
  ); 
}