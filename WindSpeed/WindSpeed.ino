/*
 * Brendan Ryder
 * 09/02/2018
 * Windspeed using adafruit anemoter
 * This is code with help from using online examples from fourms and hackerscapes.com in order
 * to measure windspeed in metres per second
 */


const int sensorPin = A0; //Defines the pin that the anemometer output is connected to
int sensorValue = 0; //Variable stores the value direct from the analog pin
double sensorVoltage = 0; //Variable that stores the voltage (in Volts) from the anemometer being sent to the analog pin
double windSpeed = 0; // Wind speed in meters per second (m/s)

double voltageConstant = (5/1023); //constant value, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V

//Adafruit anemometer datasheet specs

double minVoltage = 0.41; // Min voltage O/P  (0 m/s wind)
double maxVoltage = 2.0; // Max voltage O/P   (for 32.4m/s wind speed)

double windSpeedMin = 0; // Wind speed in m/s corresponding to voltage minimum
double windSpeedMax = 32.4; // Wind speed in m/s corresponding to voltage maximum

void setup()
{
  Serial.begin(9600);
}

//Anemometer calculations

void loop()
{
  sensorValue = analogRead(sensorPin); //Get a value between 0 and 1023 from the analog pin connected to the anemometer`
  sensorVoltage = sensorValue * voltageConstant; //sensor value -> voltage
  Serial.println(sensorVoltage);
  
  //Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
  if (sensorVoltage < minVoltage+0.01)
    windSpeed = 0; //Check if voltage is below minimum value. If so, set wind speed to zero. 
  
  else { 
    windSpeed = ((sensorVoltage - minVoltage) * windSpeedMax / (maxVoltage - minVoltage));
  //For voltages above min value calculate wind speed.
  } 

  Serial.print("Wind speed: ");
  Serial.println(windSpeed);

  //Print The current windspeed in m/s every 500 milliseconds

  delay(500);
}
