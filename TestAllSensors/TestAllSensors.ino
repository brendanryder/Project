/*
   Brendan Ryder
   20/04/2018
*/


#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <stdio.h>
#include "DHT.h"
#include <math.h>


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
unsigned long previousMillisDay = 0;
unsigned long previousMillisWeek = 0;

const long interval = 5000;
double humid;
double wind;
int wifiConnected = 0;
int DBConnected = 0;

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server_addr(192,168,43,79); // IP of the MySQL *server* here
char user[] = "root";              // MySQL user login username
char password[] = "root";        // MySQL user login password


// Sample query
char INSERT_SQL[] = "INSERT INTO weather3.temp(Temperature,Humidity,Wind,StandardDeviation) VALUES (%.1f, %.1f, %.1f, %.2f)";
char INSERT_SQL_NULL[] = "INSERT INTO weather3.temp(Temperature,Humidity,Wind,StandardDeviation) VALUES (%.1f, %.1f, %.1f, NULL)";
/*
   Insert Hourly Database Values
*/
char INSERT_SQL_HOUR_AVG[] = "INSERT INTO weather3.hourlyavg (Temperature,Humidity,Wind )SELECT ROUND(AVG(Temperature),1), AVG(Humidity), ROUND(AVG(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 60 second AND NOT EXISTS( SELECT ID FROM weather3.hourlyavg WHERE recorded >= NOW() - interval 5 second limit 1)";
char INSERT_SQL_HOUR_MAX[] = "INSERT INTO weather3.hourlymax (Temperature,Humidity,Wind )SELECT ROUND(MAX(Temperature),1), MAX(Humidity), ROUND(MAX(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 60 second AND NOT EXISTS( SELECT ID FROM weather3.hourlymax WHERE time >= NOW() - interval 5 second limit 1)";
char INSERT_SQL_HOUR_MIN[] = "INSERT INTO weather3.hourlymin (Temperature,Humidity,Wind )SELECT MIN(Temperature), MIN(Humidity), ROUND(MIN(Wind),1) FROM weather3.temp WHERE recorded >= NOW() - interval 60 second AND NOT EXISTS( SELECT ID FROM weather3.hourlymin WHERE time >= NOW() - interval 5 SECOND limit 1)";

/*
   Insert Daily Database Values
*/
char INSERT_SQL_DAY_AVG[] = "INSERT INTO weather3.dailyavg( Temperature,Humidity,Wind ) SELECT AVG(Temperature), AVG(Humidity), ROUND(AVG(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 120 SECOND AND NOT EXISTS( SELECT ID FROM weather3.dailyavg WHERE time1 >= NOW() - interval 7 SECOND limit 1)";
char INSERT_SQL_DAY_MAX[] = "INSERT INTO weather3.dailymax( Temperature,Humidity,Wind ) SELECT MAX(Temperature), MAX(Humidity), ROUND(MAX(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 120 SECOND AND NOT EXISTS( SELECT ID FROM weather3.dailymax WHERE time >= NOW() - interval 7 SECOND limit 1)";
char INSERT_SQL_DAY_MIN[] = "INSERT INTO weather3.dailymin( Temperature,Humidity,Wind ) SELECT MIN(Temperature), MIN(Humidity), ROUND(MIN(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 120 SECOND AND NOT EXISTS( SELECT ID FROM weather3.dailymin WHERE time >= NOW() - interval 7 SECOND limit 1)";


/*
   Insert Weekly Database Values
*/
char INSERT_SQL_WEEK_AVG[] = "INSERT INTO weather3.weeklyavg( Temperature,Humidity,Wind ) SELECT AVG(Temperature), AVG(Humidity), ROUND(AVG(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 4 MINUTE AND NOT EXISTS( SELECT ID FROM weather3.weeklyavg WHERE time >= NOW() - interval 11 SECOND limit 1)";
char INSERT_SQL_WEEK_MAX[] = "INSERT INTO weather3.weeklymax( Temperature,Humidity,Wind ) SELECT MAX(Temperature), MAX(Humidity), ROUND(MAX(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 4 MINUTE AND NOT EXISTS( SELECT ID FROM weather3.weeklymax WHERE time >= NOW() - interval 11 SECOND limit 1)";
char INSERT_SQL_WEEK_MIN[] = "INSERT INTO weather3.weeklymin( Temperature,Humidity,Wind ) SELECT MIN(Temperature), MIN(Humidity), ROUND(MIN(Wind),2) FROM weather3.temp WHERE recorded >= NOW() - interval 4 MINUTE AND NOT EXISTS( SELECT ID FROM weather3.weeklymin WHERE time >= NOW() - interval 11 SECOND limit 1)";


char query[128];

// WiFi card example
//char ssid[] = "VM1B11F75";    // your SSID
//char pass[] = "Evft5xfeddvE";       // your SSID Password
char ssid[] = "AndroidAP";    // your SSID
char pass[] = "12345678";       // your SSID Password



WiFiClient client;
MySQL_Connection conn((Client *)&client);
int SDCounter = 0;
double data[10000];
double StandardDeviationResult = 0;


void setup() {

  Serial.begin(9600);
  dht.begin();
  Serial.println("Initalizing  DHT Sensor.");
  delay(1500);
  Serial.println("Initalizion Complete.");
  while (!Serial); // wait for serial port to connect
  int i = 0;
  /*Setup DHT to allow accurate readings, as DHT sensor values
    may be delayed by up to 2 seconds
  */
  for (i = 0; i < 95; i++) {
    humid = dht.readHumidity();
    delay(75);
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

  myTemperature = round(myTemperature);
}

void readCurrentHumidity()
{
  //Read Humidity
  humid = dht.readHumidity();

  //Check if any reads failed and exit early (to try again).
  while (isnan(humid)) {
    Serial.print("Failed to read from DHT sensor!           ..");
    Serial.println(humid);
    humid = dht.readHumidity();
  }

}

void readCurrentWindspeed()
{
  windSensorValue = analogRead(windSensorPin); //Get a value between 0 and 1023 from the analog pin connected to the anemometer
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
  unsigned long currentMillisHour = millis();
  unsigned long currentMillisDay = millis();
  unsigned long currentMillisWeek = millis();

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

  if (currentMillis - previousMillis >= 3500) {
    previousMillis = currentMillis;
    data[SDCounter] = myTemperature;
    SDCounter++;
    Serial.print("SD Counter value: ");
    Serial.println(SDCounter);
    // Initiate the query class instance
    if (SDCounter == 12) {
      Serial.println("Recording data. SD");
      StandardDeviationResult = calculateStandardDev(data, SDCounter);
      sprintf(query, INSERT_SQL, myTemperature, humid, windSpeed, StandardDeviationResult);
      SDCounter = 0;
    }
    else {
      Serial.println("Recording data.");
      sprintf(query, INSERT_SQL_NULL, myTemperature, humid, windSpeed);
    }
    // Execute the query
    cur_mem->execute(query);
    // Note: since there are no results, we do not need to read any data

  }

  if (currentMillisHour - previousMillis1 >= 6000) {
    previousMillis1 = currentMillisHour;

    Serial.println("Recording data into hourly.");
    // Initiate the query class instance
    cur_mem->execute(INSERT_SQL_HOUR_AVG);
    cur_mem->execute(INSERT_SQL_HOUR_MAX);
    cur_mem->execute(INSERT_SQL_HOUR_MIN);
    // Note: since there are no results, we do not need to read any data
  }

  if (currentMillisDay - previousMillisDay >= 9000) {
    previousMillisDay = currentMillisDay;

    Serial.println("Recording data into Daily.");
    // Initiate the query class instance
    cur_mem->execute(INSERT_SQL_DAY_AVG);
    cur_mem->execute(INSERT_SQL_DAY_MAX);
    cur_mem->execute(INSERT_SQL_DAY_MIN);
  }

  if (currentMillisWeek - previousMillisWeek >= 12000) {
    previousMillisWeek = currentMillisWeek;

    Serial.println("Recording data into Weelky.");
    // Initiate the query class instance
    cur_mem->execute(INSERT_SQL_WEEK_AVG);
    cur_mem->execute(INSERT_SQL_WEEK_MAX);
    cur_mem->execute(INSERT_SQL_WEEK_MIN);
  }

  // Deleting the cursor also frees up memory used
  delete cur_mem;
}

void connectToDB()
{
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1500);
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
  delay(2500);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    delay(1500);
    system("reboot > /dev/ttyGS0");
    while (true);
  }

  else {
    Serial.println("Connected to network");
    IPAddress ip = WiFi.localIP();
    delay(1500);
    Serial.print("My IP address is: ");
    Serial.println(ip);
    wifiConnected = 1;
  }
  // End WiFi section
}

double calculateStandardDev(double data[], int i)
{
  int ArrSIZE;
  ArrSIZE = i;
  int j = 0;
  double sum = 0.0, mean, standardDeviation = 0.0;
  double deviation[100];
  double deviationSquared[100];
  double deviationSum = 0.0;
  double variance = 0.0;

  for (j = 0; j < ArrSIZE; j++)
  {
    sum += data[j];
  }

  mean = sum / ArrSIZE;
  /*******************************/
  for (j = 0; j < ArrSIZE; j++) {
    deviation[j] = (data[j] - mean);
    deviationSquared[j] = pow(deviation[j], 2);
    deviationSum += deviationSquared[j];
  }

  variance = (deviationSum / (ArrSIZE-1));
  standardDeviation = sqrt(variance);
  return (standardDeviation);

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

