/*
   Brendan Ryder
   30/03/2018
*/


#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <stdio.h>
#include "DHT.h"

// what pins we're connected to
#define DHTIN 4
#define DHTOUT 5

// Define the sensor type (DHT 11)
#define DHTTYPE DHT11

DHT dht(DHTIN, DHTOUT, DHTTYPE);

const int windSensorPin = A0; //Defines the pin that the anemometer output is connected to
int windSensorValue = 0; //Variable stores the value direct from the analog pin
double sensorVoltage = 0; //Variable that stores the voltage (in Volts) from the anemometer being sent to the analog pin
double windSpeed = 0; // Wind speed in meters per second (m/s)

double voltageConstant = (5 / 1023); //constant value, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V

//Adafruit anemometer datasheet specs

double minVoltage = 0.41; // Min voltage O/P  (0 m/s wind)
double maxVoltage = 2.0; // Max voltage O/P   (for 32.4m/s wind speed)

double windSpeedMin = 0; // Wind speed in m/s corresponding to voltage minimum
double windSpeedMax = 32.4; // Wind speed in m/s corresponding to voltage maximum

double myTemperature;
int tempPin = A3; // analog input pin
unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;
unsigned long previousMillisHumidity = 0;
const long interval = 5000;
double humid;
double wind;
int wifiConnected = 0;
int DBConnected = 0;

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server_addr(192, 168, 0, 157); // IP of the MySQL *server* here
char user[] = "root";              // MySQL user login username
char password[] = "root";        // MySQL user login password


// Sample query
char INSERT_SQL[] = "INSERT INTO weather3.temp(Temperature,Humidity,Wind) VALUES (%.1f, %.1f, %.1f)";
/*
   Insert Hourly Database Values
*/
char INSERT_SQL_HOUR_AVG[] = "INSERT INTO weather3.hourlyavg (Temperature,Humidity,Wind )SELECT ROUND(AVG(Temperature),1), AVG(Humidity), ROUND(AVG(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 119 second AND NOT EXISTS( SELECT ID FROM weather3.hourlyavg WHERE recorded >= NOW() - interval 119 second limit 1)";
char INSERT_SQL_HOUR_MAX[] = "INSERT INTO weather3.hourlymax (Temperature,Humidity,Wind )SELECT ROUND(MAX(Temperature),1), MAX(Humidity), ROUND(MAX(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 119 second AND NOT EXISTS( SELECT ID FROM weather3.hourlymax WHERE time >= NOW() - interval 119 second limit 1)";
char INSERT_SQL_HOUR_MIN[] = "INSERT INTO weather3.hourlymin (Temperature,Humidity,Wind )SELECT MIN(Temperature), MIN(Humidity), ROUND(MIN(Wind),1) FROM weather3.temp WHERE recorded >= NOW() - interval 60 minute AND NOT EXISTS( SELECT ID FROM weather3.dailyavg WHERE time1 >= NOW() - interval 55 SECOND limit 1)";

/*
   Insert Daily Database Values
*/
char INSERT_SQL_DAY_AVG[] = "INSERT INTO weather3.dailyavg( Temperature,Humidity,Wind ) SELECT AVG(Temperature), AVG(Humidity), ROUND(AVG(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 1 DAY AND NOT EXISTS( SELECT ID FROM weather3.dailyavg WHERE time1 >= NOW() - interval 1 DAY limit 1)";

/*
   Insert Monthly Database Values
*/
char INSERT_SQL_MONTH_AVG[] = "INSERT INTO weather3.monthlyavg( Temperature,Humidity,Wind ) SELECT AVG(Temperature), AVG(Humidity), ROUND(AVG(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 30 DAY AND NOT EXISTS( SELECT ID FROM weather3.dailyavg WHERE time1 >= NOW() - interval 30 DAY limit 1)";
char query[128];

// WiFi card example
char ssid[] = "VM1B11F75";    // your SSID
char pass[] = "Evft5xfeddvE";       // your SSID Password


WiFiClient client;
MySQL_Connection conn((Client *)&client);

void setup() {

  Serial.begin(9600);
  dht.begin();
  Serial.println("Initalizing  DHT Sensor.");
  delay(1500);
  Serial.println("Initalizion Complete.");
  while (!Serial); // wait for serial port to connect
  int i = 0;
  for (i = 0; i < 25; i++) {
    humid = dht.readHumidity();
    delay(300);
    Serial.println(humid);
  }

}

void loop()
{
  callFunctions();
}


void readCurrentTemperature()
{
  /*converts data into degrese celcius
    0.48828125 comes from the following expression:
    (SUPPLY_VOLTAGE x 1000 / 1024) / 10 where SUPPLY_VOLTAGE is 5.0V (the voltage used to power LM35)
    1024 is 2^10, (the analog value).

    1000 is used to change the unit from V to mV
    10 is constant. Each 10 mV is directly proportional to 1 Celcius.
    Therefore:(5.0 * 1000 / 1024) / 10 = 0.48828125
  */
  myTemperature = analogRead(tempPin);
  double conv =  (5.0 * 1000 / 1024) / 10;
  myTemperature = myTemperature * conv;
}

void readCurrentHumidity()
{
  unsigned long currentMillisHumidity = millis();
  //Read Humidity
  if (currentMillisHumidity - previousMillisHumidity >= 200) {
    previousMillisHumidity = currentMillisHumidity;
    humid = dht.readHumidity();

    //Check if any reads failed and exit early (to try again).
    if (isnan(humid)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    Serial.println(humid);
  }
}

void readCurrentWindspeed()
{
  windSensorValue = analogRead(windSensorPin); //Get a value between 0 and 1023 from the analog pin connected to the anemometer`
  sensorVoltage = windSensorValue * voltageConstant; //sensor value -> voltage
  //Serial.println(sensorVoltage);

  //Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
  if (sensorVoltage < minVoltage + 0.01)
    windSpeed = 0; //Check if voltage is below minimum value. If so, set wind speed to zero.

  else {
    windSpeed = ((sensorVoltage - minVoltage) * windSpeedMax / (maxVoltage - minVoltage));
    //For voltages above min value calculate wind speed.
  }


}

void insertDataIntoDatabase()
{

  unsigned long currentMillis = millis();
  unsigned long currentMillis1 = millis();

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

  if (currentMillis - previousMillis >= 20000) {
    system("ifconfig wlan0 up > /dev/ttyGS0");
    previousMillis = currentMillis;
    Serial.println("Recording data.");
    // Initiate the query class instance
    sprintf(query, INSERT_SQL, myTemperature, humid, windSpeed);
    // Execute the query
    cur_mem->execute(query);
    //cur_mem->execute(INSERT_SQL_HOUR_AVG);
    // cur_mem->execute(INSERT_SQL_HOUR_MAX);
    // Note: since there are no results, we do not need to read any data
    // Deleting the cursor also frees up memory used
    system("ifconfig wlan0 down > /dev/ttyGS0");
  }
  //
  if (currentMillis1 - previousMillis1 >= 30000) {
    system("ifconfig wlan0 up > /dev/ttyGS0");
    previousMillis1 = currentMillis1;
    Serial.println("Recording data into hourly avg.");
    // Initiate the query class instance
    cur_mem->execute(INSERT_SQL_HOUR_AVG);
    Serial.println("Recording data into hourly max.");
    cur_mem->execute(INSERT_SQL_HOUR_MAX);
    // Note: since there are no results, we do not need to read any data
    // Deleting the cursor also frees up memory used
    system("ifconfig wlan0 down > /dev/ttyGS0");

  }


  delete cur_mem;
}

void connectToDB()
{
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
    DBConnected = 1;
  }
  else {
    Serial.println("Connection failed.");
    conn.close();
  }
}

void connectToWifi()
{
  // Begin WiFi section
  int status = WiFi.begin(ssid, pass);
  delay(1500);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    system("reboot > /dev/ttyGS0");
    while (true);
  }

  else {
    Serial.println("Connected to network");
    IPAddress ip = WiFi.localIP();
    Serial.print("My IP address is: ");
    Serial.println(ip);
    wifiConnected = 1;
  }
  // End WiFi section
}

void callFunctions()
{
  if (!wifiConnected) {
    connectToWifi();
  }

  if (!DBConnected) {
    connectToDB();
  }

  readCurrentHumidity();
  readCurrentTemperature();
  readCurrentWindspeed();
  insertDataIntoDatabase();

}
