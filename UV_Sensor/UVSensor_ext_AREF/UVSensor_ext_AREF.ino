
/*
 Analog Input DEMO for UV Sensor
 Demonstrates analog input by reading an analog sensor on analog pin 0 and
 turning on and off a light emitting diode(LED)  connected to digital pin 3. 
 The amount of time the LED will be on and off depends on
 the value obtained by analogRead(). 
 
 Created by David Cuartielles
 modified 30 Aug 2011
 By Tom Igoe
 
 modified 8 Aug 2013
 by Petteri Teikari
 
 This example code is in the public domain. 
 http://arduino.cc/en/Tutorial/AnalogInput
 
 */

int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 3;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
// int regPin = 5;
float aRef = 1800; // millivolts 
float aResolution = aRef / 1024;  // millivolts


void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT); 
  //pinMode(regPin, OUTPUT); 
  //digitalWrite(regPin, HIGH);  
  Serial.begin(115200); 
  Serial.println("UV Sensor demo");
  
  // Use external reference, 
  // i.e. connect a jumper wire from 1.8V pin to AREF pin
  analogReference(EXTERNAL);
  
    // Tutorial: Arduino and the AREF pin
    // http://tronixstuff.com/2010/12/07/tutorial-arduino-and-the-aref-pin/
 
  
}

void loop() {
  
  // digitalWrite(regPin, HIGH); // refresh just in case
  
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);  // raw reading
  // turn the ledPin on
  digitalWrite(ledPin, HIGH);  
  
  // convert to percentage
  // float percentage = (sensorValue/1023)*100;
  float voltage = aResolution * sensorValue;
  
  // write the sensor value in millivolts also to serial port (you can monitor the values in Serial Monitor)
  Serial.print("UV Sensor Voltage (A0):");
  Serial.print("\t");
  Serial.print(voltage);
  Serial.print(" mV");
  Serial.print("\t");
  Serial.print("sensorValue: ");
  Serial.println(sensorValue);
  // Serial.print("\t");
  // Serial.print("percentage: ");
  // Serial.print(percentage);
  // Serial.println("%");
  
    // The UV Sensor output when plugged to a picoammeter was around
    // ~3 nA in direct sunlight, thus the gain should have been "6 x 10^8)
    // 600 MOhm 
    
  // Invert the sensorValue now so that higher reading lead to faster blinking
  sensorValue = map(sensorValue, 0, 1023, 1023, 0);   
  // stop the program for <sensorValue> milliseconds:
  delay(sensorValue+10);          
  // turn the ledPin off:        
  digitalWrite(ledPin, LOW);   
  // stop the program for for <sensorValue> milliseconds:
  delay(sensorValue+10);                  
  

}
