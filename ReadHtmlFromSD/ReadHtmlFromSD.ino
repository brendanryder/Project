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

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// size of buffer used to capture HTTP requests
#define BUFF_SIZE 20

// MAC address from Ethernet shield sticker under board
//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(169,254,8,153); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80
File myFile;
char HTTP_req[BUFF_SIZE] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer

void setup()
{
  Serial.begin(9600);       // for debugging

  // initialize SD card
  Serial.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("SD card initialization failed!");
    return;    // init failed
  }
  Serial.println("SUCCESS - SD card initialized.");
  // check for file existing
  if (!SD.exists("data_csv.html")) {
    Serial.println("Can't find file");
    return;  // can't find index file
  }
  Serial.println("Found file.");

//  Ethernet.begin(ip);  // initialize Ethernet device
  server.begin();           // start to listen for clients
}

void loop()
{
  EthernetClient client = server.available();  // try to get client

if (client) {  // got client?
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {   // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        // buffer first part of HTTP request in HTTP_req array (string)
        // leave last element in array as 0 to null terminate string (BUFF_SIZE - 1)
        if (req_index < (BUFF_SIZE - 1)) {
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
          //if else statements to open requested files. . if they exist.
          //Reading in file in bytes using buffer . . 
          //Example code http://forum.arduino.cc/index.php?topic=203207.0
          else if (StrContains(HTTP_req, "GET /c3.min.js")) {
            myFile = SD.open("c3.min.js");
          }
          else if (StrContains(HTTP_req, "GET /c3.css")) {
            myFile = SD.open("c3.css");

          }
          
          else if (StrContains(HTTP_req, "GET /d3.v3.min.js")) {
            myFile = SD.open("d3.v3.min.js");
          }
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
          //          StrClear(HTTP_req, BUFF_SIZE);
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
