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

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT); 
  Serial.begin(115200); 
  Serial.println("UV Sensor demo");
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);    
  // turn the ledPin on
  digitalWrite(ledPin, HIGH);  
  // write the sensor value also to serial port (you can monitor the values in Serial Monitor)
  Serial.println(sensorValue);
  // Invert the sensorValue now so that higher reading lead to faster blinking
  sensorValue = map(sensorValue, 0, 1023, 1023, 0);   
  // stop the program for <sensorValue> milliseconds:
  delay(sensorValue+10);          
  // turn the ledPin off:        
  digitalWrite(ledPin, LOW);   
  // stop the program for for <sensorValue> milliseconds:
  delay(sensorValue+10);                  
  

}
