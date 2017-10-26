// Reading temperature using lm35

// initialize the library with the numbers of the interface pins
// initialize our variables
float temperature;
int tempPin = A0; // analog input pin
void setup() {
//set up serial
Serial.begin(9600);
}
void loop() {
 //gets and prints the data from the lm35
  temperature = analogRead(tempPin);

  //converts data into *C
  //0.48828125 comes from the following expression:

  // (SUPPLY_VOLTAGE x 1000 / 1024) / 10 where SUPPLY_VOLTAGE is 5.0V (the voltage used to power LM35)
  // 1024 is 2^10, (the analog value).

  // 1000 is used to change the unit from V to mV
  // 10 is constant. Each 10 mV is directly proportional to 1 Celcius.
  // Therefore:(5.0 * 1000 / 1024) / 10 = 0.48828125
  double conv =  (5.0 * 1000 / 1024) / 10;
  temperature = temperature*conv;
  Serial.print("CELSIUS: ");
  Serial.print(temperature);
  Serial.println("*C ");

  delay(1500);
}

