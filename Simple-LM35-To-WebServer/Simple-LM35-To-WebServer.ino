/*
 Simple sketch to display current temperature reading from LM35
 on webserver.
 Using example webserver sketch as base for displaying data to webpage.
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(169,254,8,153);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

float temp;
int tempPin = A0; // analog input pin

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  //gets and prints the data from the lm35
  temp = analogRead(tempPin);

  //converts data into *C
  //0.48828125 comes from the following expression:

  // (SUPPLY_VOLTAGE x 1000 / 1024) / 10 where SUPPLY_VOLTAGE is 5.0V (the voltage used to power LM35)
  // 1024 is 2^10, (the analog value).

  // 1000 is used to change the unit from V to mV
  // 10 is constant. Each 10 mV is directly proportional to 1 Celcius.
  // Therefore:(5.0 * 1000 / 1024) / 10 = 0.48828125
  double conv =  (5.0 * 1000 / 1024) / 10;
  temp = temp*conv;
  Serial.print("CELSIUS: ");
  Serial.print(temp);
  Serial.println("*C ");

  delay(1500);
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"3\">");
          // output the value of each analog input pin
            client.print("Current Temperature");
            client.print(" is ");
            client.print(temp);
            client.print(" *C ");
            client.println("<br />");       
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

