#include <Wire.h> 
#include "RTClib.h" // https://github.com/adafruit/RTClib
#include <LowPower.h> // https://github.com/rocketscream/Low-Power

RTC_Millis RTC;

void setup(void){  
  
  Serial.begin(115200);
  Wire.begin(); 

  // following line sets the RTC to the date & time this sketch was compiled
  RTC.begin(DateTime(__DATE__, __TIME__));
  
}

void loop(){
  
    // Sleep for x s with ADC module and BOD module off
    // other options, see LowPower.cpp, e.g. 15 ms to FOREVER
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); // ADC_CONTROL_OFF -> ADC_OFF
            
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
    
    delay(10);
}

