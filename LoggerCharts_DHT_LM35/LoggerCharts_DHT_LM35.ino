/*--------------------------------------------------------------
Using Following examples to get code running 
  - WebServer example
  - SD card examples
  - Ethernet library
  - SD Card library

  Brendan Ryder
  Used example code from http://forum.arduino.cc/index.php?topic=203207.0 
  and built and added to this sketch to enable reading html file stored
  on SD card. This .html file also contains other file types wiich must be
  accommodated for such as .csv, .js and .css files.
  This sketch reads html file, which contains javascript to display charts,
  generated using c3js charting library. The data output on the chart is data that is 
  stored in a .csv file, also contained on the SD card.
  --------------------------------------------------------------*/
#include <Ethernet.h>
#include <SD.h>
#include "DHT.h"

// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   20
// what pins we're connected to
#define DHTIN 4
#define DHTOUT 5
// Define the sensor type (DHT 11)
#define DHTTYPE DHT11

unsigned long previousMillis = 0;
const long interval = 10000;
float humRead;

int temp;
int tempPin = A3; // analog input pin

DHT dht(DHTIN, DHTOUT, DHTTYPE);

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 12, 16, 214); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80
File myFile;
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer

int i = 0;
bool setupDone = false;
void setup()
{
  //Initialize the sensor
  dht.begin();
  for (i = 0; i < 150; i++)
  {
    humRead = dht.readHumidity();
    delay(50);
  }
  
  system("ifdown eth0");
  system("ifup eth0");
  system("ifconfig -a > /dev/ttyGS0");

  dht.readHumidity();
  Serial.begin(9600);       // for debugging
  myFile = SD.open("DHTlogger.csv", FILE_WRITE);
  delay(2000);
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());

  // initialize SD card
  //  Serial.println("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    return;    // init failed
  }
  // if the file opened  pened okay, write to it:
  if (myFile) {
    Serial.print("File 'DHTlogger.csv' ready for writing...");
    myFile.close();
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening file\n");
  }

  Ethernet.begin(mac, ip);  // initialize Ethernet device
  server.begin();           // start to listen for clients
}

void loop()
{
  readAndLogTempHumidity();
  connectWebPage();

}
void readAndLogTempHumidity() {
  // Wait a few mS between measurements.
  delay(250);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old'

  // Read Humidity as percentage
  humRead = dht.readHumidity();
  // Read temperature as Celsius
  temp = analogRead(tempPin);
  //converts data into *C
  //0.48828125 comes from the following expression:

  // (SUPPLY_VOLTAGE x 1000 / 1024) / 10 where SUPPLY_VOLTAGE is 5.0V (the voltage used to power LM35)
  // 1024 is 2^10, (the analog value).

  // 1000 is used to change the unit from V to mV
  // 10 is constant. Each 10 mV is directly proportional to 1 Celcius.
  // Therefore:(5.0 * 1000 / 1024) / 10 = 0.48828125

  double conv =  (5.0 * 1000 / 1024) / 10;
  temp = temp * conv;
  
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("*C ");
  Serial.print("\tHum: ");
  Serial.print(humRead);
  Serial.println("% ");

  // Check if any reads failed and exit early (to try again).
  if (isnan(humRead)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Serial.print("Humidity(%): ");
    Serial.print(humRead);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" *C ");

    myFile = SD.open("DHTLogger.csv", FILE_WRITE);

    // if the file opened okay, write to it:
    if (myFile)
    {
      myFile.print(humRead);
      myFile.print(",");
      myFile.print(temp);
      myFile.println();
      myFile.close();

    }
    else {
      // if the file didn't open, print an error:
      Serial.println("error opening DHTlogger.csv \n");
    }
  }
}
void connectWebPage()
{
  EthernetClient client = server.available();  // try to get client

  if (client) {  // got client?
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {   // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        // buffer first part of HTTP request in HTTP_req array (string)
        // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;          // save HTTP request character
          req_index++;
        }
        // print HTTP request character to serial monitor
        Serial.print(c);
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank) {
          // open requested web page file
          if (StrContains(HTTP_req, "GET / ")
              || StrContains(HTTP_req, "GET /data_csv.htm")) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
            myFile = SD.open("data_csv.html");        // open web page file
          }
          /* Uncomment if we need to pull file (c3.min.js) locally 
          else if (StrContains(HTTP_req, "GET /c3.min.js"))
          {
            myFile = SD.open("c3.min.js");

            byte clientBuf[1024];
            int clientCount = 0;

            while (myFile.available())
            {
              clientBuf[clientCount] = myFile.read();
              clientCount++;

              if (clientCount > 1023)
              {
                client.write(clientBuf, 1024);
                clientCount = 0;
              }
            }
            if (clientCount > 0) client.write(clientBuf, clientCount);
            myFile.close();
          }
          */
          else if (StrContains(HTTP_req, "GET /c3.css"))
          {
            myFile = SD.open("c3.css");

          }
          
          /* Uncomment if we need to pull file (d3.v3.min.js) locally 
          else if (StrContains(HTTP_req, "GET /d3.v3.min.js"))
          {
            myFile = SD.open("d3.v3.min.js");
            byte clientBuf[1024];
            int clientCount = 0;

            while (myFile.available())
            {
              clientBuf[clientCount] = myFile.read();
              clientCount++;
              if (clientCount > 1023)
              {
                client.write(clientBuf, 1024);
                clientCount = 0;
              }
            }
            if (clientCount > 0) client.write(clientBuf, clientCount);
            myFile.close();
          }
*/
          else if (StrContains(HTTP_req, "GET /DHTlogger.csv")) {
            myFile = SD.open("DHTlogger.csv");
          }

          if (myFile) {
            while (myFile.available()) {
              client.write(myFile.read()); // send web page to client
            }
            myFile.close();
          }
          // reset buffer index and all buffer elements to 0
          req_index = 0;
          //          StrClear(HTTP_req, REQ_BUF_SZ);
          break;
        }
        // every line of text received from the client ends with \r\n
        if (c == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      } // end if (client.available())
    } // end while (client.connected())
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  } // end if (client)
}

char StrContains(char *str, char *sfind)
{
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);

  if (strlen(sfind) > len) {
    return 0;
  }
  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return 1;
      }
    }
    else {
      found = 0;
    }
    index++;
  }
  return 0;
}


