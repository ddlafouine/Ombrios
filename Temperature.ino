#include <WiFi.h>
#include "ThingSpeak.h"
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "";   // your network SSID (name) 
const char* password = "";   // your network password

WiFiClient client;

unsigned long myChannelNumber = X; // Replace with your channel number
const char * myWriteAPIKey = ""; // Replace with your API key

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 600000; // 10 minutes

// DS18B20 setup
#define ONE_WIRE_BUS 4 // Pin where the DS18B20 is connected
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variable to hold temperature readings
float temperatureC;

void initDS18B20() {
  sensors.begin();
  if (sensors.getDeviceCount() == 0) {
    Serial.println("No DS18B20 sensors found! Check wiring.");
    while (1);
  }
}

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  initDS18B20();
  
  WiFi.mode(WIFI_STA);   
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }

    // Request temperature readings from DS18B20
    sensors.requestTemperatures();
    temperatureC = sensors.getTempCByIndex(0); // Get temperature in Celsius
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureC);
    
    // Write to ThingSpeak
    int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureC, myWriteAPIKey);
    // Uncomment if you want to send temperature in Fahrenheit
    // int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}
