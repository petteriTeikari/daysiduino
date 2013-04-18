// Demonstration TEST CODE for Microchip 24AA256/24LC256/24FC256
// 256K I2C CMOS Serial EEPROM
// e.g. https://www.sparkfun.com/products/525

// July2010- Volkemon tweaked and commented.
// This description from: http://arduino.cc/forum/index.php/topic,20796.0.html

  //      _ _
  //  A0-|oU |-Vcc
  //  A1-|   |-WP
  //  A2-|   |-SCL
  // Vss-|   |-SDA
  //      ---
  //
  //   SDA goes to Arduino 4
  //   SCL goes to Arduino 5
  //   WP  goes to ground for now. Can be put to Vcc if write protection is needed.
  //   Vcc goes to arduino Vcc
  //   Vss goes to arduino ground
  //   A2, A1, A0 go to ground for now.
  //
  // They can be also put to either ground or Vcc to control up to 8 memory chips (2^3), giving you 2MBit of memory (8*2^15).
  // A2 A1 A0 Binary Address Hex Address 
  // 0  0  0    0b1010000       0×50 
  // 0  0  1    0b1010001       0×51 
  // 0  1  0    0b1010010       0×52 
  // 0  1  1    0b1010011       0×53 
  // 1  0  0    0b1010100       0×54 
  // 1  0  1    0b1010101       0×55 
  // 1  1  0    0b1010110       0×56 
  // 1  1  1    0b1010111       0×57

// Standard Arduino I2C library
#include <Wire.h>    

  #define eeprom 0x50         // Base Address of 24LC256 eeprom chip, when the address bin are GND

    // For alternative I2C libraries, see:
    // https://code.google.com/p/rtoslibs/downloads/list
    // DigitalIO or I2cMaster

// Using the Time library by Michael Margolis as "software RTC"
// http://www.pjrc.com/teensy/td_libs_Time.html
// http://playground.arduino.cc/Code/Time
#include <Time.h>

  // Modified from TimeSerial example:
  #define TIME_HEADER  "T"   // Header tag for serial time sync message
  #define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

    // Alternatively, one could use RTClib as used for example in:
    // "Using Arduino to measure circadian rhythms under unusual conditions"\
    // http://circahack.blogspot.com/2012/08/using-arduino-to-measure-circadian.html
    // Software RTC Library, https://github.com/adafruit/RTClib
    // #include "RTClib.h"

// Mini Ultra+ Library
// http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/
#include <LowPower.h> 


int sensorPin = A0;         // select the input pin for "dummy data"
unsigned int memCount = 0;  // counter to define the eeaddress
unsigned int sampleCount = 1;  // counter to define the recorded samples
unsigned int samplesToCollect = 3; // total number of samples to collect
unsigned int time = 0;              // time (milliseconds) to be written for EEPROM
unsigned int numberOfDataColumns = 3; // count, time, value

void setup(void){  
  
  Serial.begin(115200);
  Wire.begin(); 

  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
  
}

void loop(){
  
  unsigned int i = 0;
  
  // Sleep for x s with ADC module and BOD module off
  // other options, see LowPower.cpp, e.g. 15 ms to FOREVER
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); // ADC_CONTROL_OFF -> ADC_OFF
  
  if (sampleCount <= samplesToCollect) {
    
    // read the value from the sensor:
    unsigned int sensorValue = analogRead(sensorPin); // this is now 10-bit value [0,1023]
    sensorValue = map(sensorValue, 0, 1023, 0, 255); // convert to 8-bit value (byte)
    
    // Get the time using the "software RTC"
    if (Serial.available()) {
      processSyncMessage();
    }
    if (timeStatus()!= timeNotSet) {
      digitalClockDisplay();  
    }
  
    // Debug Line to check if the refresh is okay from the host
    if (timeStatus() == timeSet) {
      digitalWrite(13, HIGH); // LED on if synced
    } else {
      digitalWrite(13, LOW);  // LED off if needs refresh
    }
    
    // Get the current time now
    time_t timeStamp = now();
    digitalClockDisplay();  
    Serial.println(timeStamp);    
    
    // First WRITE the values to the EEPROM  
    writeEEPROM(eeprom, memCount, sampleCount); 
    writeEEPROM(eeprom, memCount+1, timeStamp);
    writeEEPROM(eeprom, memCount+2, sensorValue);     

    memCount = memCount + numberOfDataColumns; // increase the loop index
    sampleCount++; // increase the sample index
    
        // The following lines a bit apractical as the serial
        // communication adds quite of overhead
        // Serial.print("EEPROM Write: ");
        // Serial.print(sensorValue);
        // Serial.print("\n");
    
  }
  
  else {
    
    // Then READ the values
    Serial.println("SENSOR VALUES:");
    for (i=0; i<samplesToCollect*numberOfDataColumns; i = i + numberOfDataColumns){
      Serial.print(readEEPROM(eeprom, i), DEC);
      Serial.print(" ");
      Serial.print(readEEPROM(eeprom, i+1), DEC);
      Serial.print(" ");
      Serial.print(readEEPROM(eeprom, i+2), DEC);
      Serial.print(" \n");
    }  
    memCount = 0; // reset the value
    sampleCount = 1; // reset the value
    delay(5);
    
  }
}


// TIME SUBFUNCTION
void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}


// TIME SUBFUNCTION
void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}



// TIME SUBFUNCTION
void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}


// TIME SUBFUNCTION
time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
 
  
// SUBFUNCTION FOR EEPROM WRITE
void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) {
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();

  delay(5);
}


// SUBFUNCTION FOR EEPROM READ
byte readEEPROM(int deviceaddress, unsigned int eeaddress ) {
  byte rdata = 0xFF;

  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(deviceaddress,1);

  if (Wire.available()) rdata = Wire.read();

  return rdata;
}

