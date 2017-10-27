
#include "DHT.h"
 
// what pins we're connected to
#define DHTIN 4 
#define DHTOUT 5
// Define the sensor type (DHT 11)
#define DHTTYPE DHT11

unsigned long previousMillis = 0;
const long interval = 2000;
 
DHT dht(DHTIN,DHTOUT, DHTTYPE);
 
void setup() {
  
  Serial.begin(9600);
  dht.begin();
  Serial.println("Initalizing  DHT Sensor.");
  delay(3500);
  Serial.println("Initalizion Complete.");
  //Initialize the sensor
  
}
 
void loop() { 
  // Wait a few seconds between measurements.
  delay(400);
 
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old'.

  //Read Humidity
  float hum = dht.readHumidity();
 
  // Check if any reads failed and exit early (to try again).
  if (isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  unsigned long currentMillis = millis();

  //Print out humidity readiing
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
   
    Serial.print("Humidity: ");
    Serial.print(temp);
    Serial.println(" %\t");
  }

}
