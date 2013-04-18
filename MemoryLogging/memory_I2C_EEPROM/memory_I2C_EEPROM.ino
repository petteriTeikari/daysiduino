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

// Mini Ultra+ Library
// http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/
#include <LowPower.h> 

#define eeprom 0x50         // Base Address of 24LC256 eeprom chip, when the address bin are GND

int sensorPin = A0;         // select the input pin for "dummy data"
unsigned int memCount = 0;  // counter to define the eeaddress
unsigned int samplesToCollect = 25; // total number of samples to collect
unsigned int time = 0;              // time (milliseconds) to be written for EEPROM

void setup(void){  
  
  Serial.begin(115200);
  Wire.begin(); 
  
}

void loop(){
  
  unsigned int i = 0;
  
  // Sleep for x s with ADC module and BOD module off
  // other options, see LowPower.cpp, e.g. 15 ms to FOREVER
  LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF); // ADC_CONTROL_OFF -> ADC_OFF
  
  if (memCount <= samplesToCollect) {
    
    // read the value from the sensor:
    unsigned int sensorValue = analogRead(sensorPin); // this is now 10-bit value [0,1023]
    sensorValue = map(sensorValue, 0, 1023, 0, 255); // convert to 8-bit value (byte)
    // time = millis(); 
    
    // First WRITE the values to the EEPROM  
    writeEEPROM(eeprom, (samplesToCollect-1)+memCount, memCount); 
    writeEEPROM(eeprom, memCount, sensorValue);     
    memCount++; // increase the loop index
        // The following lines a bit apractical as the serial
        // communication adds quite of overhead
        // Serial.print("EEPROM Write: ");
        // Serial.print(sensorValue);
        // Serial.print("\n");
    
  }
  
  else {
    
    // Then READ the values
    Serial.println("SENSOR VALUES:");
    for (i=0; i<samplesToCollect; i++){
      Serial.print(readEEPROM(eeprom, (samplesToCollect-1)+i), DEC);
      Serial.print(" ");
      Serial.print(readEEPROM(eeprom, i), DEC);
      Serial.print(" \n");
    }  
    memCount = 0; // reset the value
    delay(5);
    
  }
}
  
  
// SUBFUNCTION FOR WRITE
void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data ) {
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();

  delay(5);
}

// SUBFUNCTION FOR READ
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

