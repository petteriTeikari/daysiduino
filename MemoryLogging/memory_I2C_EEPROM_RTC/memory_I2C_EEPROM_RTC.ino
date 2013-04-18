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

    // For alternative I2C libraries, see:
    // https://code.google.com/p/rtoslibs/downloads/list
    // DigitalIO or I2cMaster

// Alternatively, one could use RTClib as used for example in:
// "Using Arduino to measure circadian rhythms under unusual conditions"\
// http://circahack.blogspot.com/2012/08/using-arduino-to-measure-circadian.html
// Software RTC Library, https://github.com/adafruit/RTClib
#include "RTClib.h"

// Mini Ultra+ Library
// http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/
#include <LowPower.h> 

#define eeprom 0x50         // Base Address of 24LC256 eeprom chip, when the address bin are GND

RTC_Millis RTC;

int sensorPin = A0;         // select the input pin for "dummy data"
unsigned int memCount = 0;  // counter to define the eeaddress
unsigned int sampleCount = 1;  // counter to define the recorded samples
unsigned int samplesToCollect = 3; // total number of samples to collect
unsigned int time = 0;              // time (milliseconds) to be written for EEPROM
unsigned int numberOfDataColumns = 3; // count, time, value

void setup(void){  
  
  Serial.begin(115200);
  Wire.begin(); 

  // following line sets the RTC to the date & time this sketch was compiled
  RTC.begin(DateTime(__DATE__, __TIME__));
  
}

void loop(){
  
  unsigned int i = 0;
  
  // Sleep for x s with ADC module and BOD module off
  // other options, see LowPower.cpp, e.g. 15 ms to FOREVER
  // LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); // ADC_CONTROL_OFF -> ADC_OFF
  
  if (sampleCount <= samplesToCollect) {
    
    // read the value from the sensor:
    unsigned int sensorValue = analogRead(sensorPin); // this is now 10-bit value [0,1023]
    sensorValue = map(sensorValue, 0, 1023, 0, 255); // convert to 8-bit value (byte)
    
    // Get the time using the "software RTC"
    DateTime now = RTC.now();  
    
    // Print on the Serial Monitor / Console the time (debugging)
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    // Integer that could be saved to the EEPROM
    // Could be then converted to human-readable format when reading
    // in the data in Python, Matlab, Java, whatever
    Serial.print(" seconds since 1970: ");
    Serial.println(now.unixtime());
    
    unsigned int timeStamp = now.unixtime();
    
      // e.g. 2013/4/18 11:34:07
      // -----------------------
      // seconds since 1970: 1,366,284,847 - requires 32-bit integer, thus
      //                                     4 x 8-bit memory addresses from
      //                                     EEPROM      

    
    // First WRITE the values to the EEPROM  
    writeEEPROM(eeprom, memCount, sampleCount); // only correct until 255
    writeEEPROM(eeprom, memCount+1, timeStamp); // now is truncated to 8-bit
    writeEEPROM(eeprom, memCount+2, sensorValue);      

    memCount = memCount + numberOfDataColumns; // increase the loop index
    sampleCount++; // increase the sample index
    
        // The following lines a bit apractical as the serial
        // communication adds quite of overhead
        // Serial.print("EEPROM Write: ");
        // Serial.print(sensorValue);
        // Serial.print("\n");
        
    delay(1000); // quick'n'dirty second counter to avoid the complexity of "sleep mode"
    
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

