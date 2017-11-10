#include <SD.h>;
#include "DHT.h"

// what pins we're connected to
#define DHTIN 4
#define DHTOUT 5
// Define the sensor type (DHT 11)
#define DHTTYPE DHT11

unsigned long previousMillis = 0;
const long interval = 5000;
float humRead;
float tempRead;

DHT dht(DHTIN, DHTOUT, DHTTYPE);

File myFile;

void setup() {
  

  Serial.begin(9600);
  Serial.print("\nInitializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  //Initialize the sensor
  dht.begin();

  /*
   * Allow time for Sensor values to re read correctly
   * as sensor reading can be delayed for up to 2 seconds
   */
  for(int i=0; i<50; i++)
  {
    dht.readHumidity();
    dht.readTemperature();
    delay(50);
  }

  Serial.println("initialization done.");
  myFile = SD.open("DHTlogger.csv", FILE_WRITE);
  dht.readHumidity();
  dht.readTemperature();
  delay(3000);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("File 'DHTlogger.csv' ready for writing...");
    myFile.print("Humidity");
    myFile.print(",");
    myFile.print("Temerature");
    myFile.println();
    myFile.close();
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening file\n");
  }
  dht.readHumidity();
  dht.readTemperature();
  
  delay(2500);
}

void loop() {

  readAndLogTempHumidity();
 
}

void readAndLogTempHumidity() {

    // Wait a few mS between measurements.
    delay(400);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old'

    // Read Humidity as percentage
    humRead = dht.readHumidity();
    // Read temperature as Celsius
    tempRead = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(humRead) || isnan(tempRead)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      Serial.print("Humidity: ");
      Serial.print(humRead);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(tempRead);
      Serial.println(" *C ");

      myFile = SD.open("DHTLogger.csv", FILE_WRITE);

      // if the file opened okay, write to it:
      if (myFile)
      {
        myFile.print(humRead);
        myFile.print(",");
        myFile.print(tempRead);
        myFile.println();
        myFile.close();

      }
      else {
        // if the file didn't open, print an error:
        Serial.println("error opening DHTlogger.csv \n");
      }
    }
  }

