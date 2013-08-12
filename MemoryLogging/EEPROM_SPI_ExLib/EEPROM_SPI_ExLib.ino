/*
 * EEPROMEx 
 *
 * Demonstrates reading, writing and updating data in the EEPROM
 * located internally in the microcontroller
 *
 * Example provided by Thijs Elenbaas (EEPROMEx library)
 * http://thijs.elenbaas.net/2012/07/extended-eeprom-library-for-arduino/
 * 
 * This example code is in the public domain.
 */

#include <EEPROMEx.h>
#include "Arduino.h"

// The following functions / calls are basically re-arranged from
// the example code EEPROMEx included with the library

// Possible to write/read different data types directly
// e.g.

  // BYTE
  // byte input  = 120;
  // byte output = 0;
  // EEPROM.write(addressByte,input);   // same function as writeByte
  // output = EEPROM.read(addressByte); // same function as readByte
  
  // INT
  // int input  = 30000;
  // int output = 0;
  // EEPROM.writeInt(addressInt,input);
  // output = EEPROM.readInt(addressInt);
    
  // LONG
  // long input  = 200000000;
  // long output = 0;
  // EEPROM.writeLong(addressLong,input);
  // output = EEPROM.readLong(addressLong);
    
  // FLOAT
  // double input  = 1010102.50;
  // double output = 0.0;
  // EEPROM.writeFloat(addressFloat,input);
  // output = EEPROM.readFloat(addressFloat);
  
// Possible also to update value rather than to write everything

  // double input  = 1000002.50;
  // double output = 0.0;
  // EEPROM.updateDouble(addressDouble,input);   
  // output = EEPROM.readDouble(addressDouble);

// Test reading and updating a string (char array) to EEPROM

  // char input[] = "Arduino";
  // char output[] = "       ";
  // EEPROM.writeBlock<char>(addressCharArray, input, 7);
  // EEPROM.readBlock<char>(addressCharArray, output, 7);

// updating and reading a byte array

  //  int itemsInArray = 7;
  //  byte initial[] = {1, 0, 4, 0, 16, 0 , 64 };
  //  byte input[]   = {1, 2, 4, 8, 16, 32, 64 };    
  //  byte output[sizeof(input)];
  //  EEPROM.writeBlock<byte>(addressByteArray, initial, itemsInArray);
  //  int writtenBytes = EEPROM.updateBlock<byte>(addressByteArray, input, itemsInArray);
  //  EEPROM.readBlock<byte>(addressByteArray, output, itemsInArray);

const int maxAllowedWrites = 80;
const int memBase          = 350;

int addressByte;
int addressInt;
int addressLong;
int addressFloat;
int addressDouble;
int addressByteArray;
int addressCharArray;
    
  
void setup()
{
  Serial.begin(9600);
  
  // start reading from position memBase (address 0) of the EEPROM. Set maximumSize to EEPROMSizeUno 
  // Writes before membase or beyond EEPROMSizeUno will only give errors when _EEPROMEX_DEBUG is set
  EEPROM.setMemPool(memBase, EEPROMSizeUno);
  
  // Set maximum allowed writes to maxAllowedWrites. 
  // More writes will only give errors when _EEPROMEX_DEBUG is set
  EEPROM.setMaxAllowedWrites(maxAllowedWrites);
  delay(100);
  Serial.println("");       
  
  // Always get the adresses first and in the same order
  addressByte      = EEPROM.getAddress(sizeof(byte));
  addressInt       = EEPROM.getAddress(sizeof(int));
  addressLong      = EEPROM.getAddress(sizeof(long));
  addressFloat     = EEPROM.getAddress(sizeof(float));
  addressDouble    = EEPROM.getAddress(sizeof(double));    
  addressByteArray = EEPROM.getAddress(sizeof(byte)*7);  
  addressCharArray = EEPROM.getAddress(sizeof(char)*7);  

  int loopCount = 8;

  // Write some integers and read them back during the same cycle
  Serial.println("Write/Read (Same cycle)");       
  Serial.println("");       
  for (int I=0;I<loopCount;I++) 
  {
    int input  = I*55;
    int output = 0;
    EEPROM.writeInt(addressInt,input);
    output = EEPROM.readInt(addressInt);    
    
    Serial.print(I);
    Serial.print('\t');
    Serial.print(addressInt);    
    Serial.print('\t');
    Serial.println(output);
    delay(20);
  }

  // Write integers in a loop and then read them in a loop  
  
    // FIRST WRITE
    Serial.println("");       
    Serial.println("Addresses during writing)");
    int addressInt_list [loopCount];
    for (int I=0;I<loopCount;I++) 
    {
       int input  = I*55;
       addressInt_list[I] = EEPROM.getAddress(sizeof(int));
       Serial.println(addressInt_list[I]);
       EEPROM.writeInt(addressInt_list[I],input);    
       delay(20);
    }

    // THEN READ
    Serial.println("");       
    Serial.println("Write/Read (Different cycle)");       
    for (int I=0;I<loopCount;I++) 
    {
      int output [loopCount];  
      output[I] = EEPROM.readInt(addressInt_list[I]);
      
      Serial.print(I);
      Serial.print('\t');
      Serial.print(addressInt_list[I]);    
      Serial.print('\t');
      Serial.println(output[I]);
      delay(20);
    }

    
    


}

void loop()
{
  // Nothing to do during loop
}    


