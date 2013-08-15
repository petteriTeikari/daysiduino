// Simple SPI Testing to be used with:
// Logic Sniffer (OLS): http://www.lxtreme.nl/ols/
// logic_analyzer for Arduino Uno/MEGA: https://github.com/gillham/logic_analyzer

#include <SPI.h>

// For further information, see the SPI Guide for F-RAM
// http://www.cypress.com/?docID=44520
const byte CMD_WREN = 0x06; //0000 0110 Set Write Enable Latch
const byte CMD_WRITE = 0x02; //0000 0010 Write Memory Data
const byte CMD_READ = 0x03; //0000 0011 Read Memory Data
const byte CMD_RDSR = 0x05; //0000 0101 Read Status Register
const byte CMD_WRSR = 0x01; //0000 0001 Write Status Register
const byte CMD_WRDI = 0x04; //0000 0100 Write Disable
const byte CMD_SLEEP = 0xB9; // 1011 1001 Enter Sleep Mode
const byte CMD_FASTREAD = 0x0A; // 0000 1011 Fast Read Memory Data
const byte CMD_RDID = 0x9F ; // 1001 1111 Read Device ID
const byte CMD_SNR= 0xC3 ; // 1100 0011 Read Device S/N
 
const int FRAM_CS1 = 3; //chip select for memory (2 in Daysimeter v.13)

void setup()
  {    
    Serial.begin(9600);  
    pinMode(FRAM_CS1, OUTPUT);
    digitalWrite(FRAM_CS1, HIGH);
   
    // Setting up the SPI bus
    SPI.begin();
    SPI.setDataMode(SPI_MODE0); // FRAMs support 0 and 3
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV8); // 8 MHz of system clock
  }

void loop()
  {
    
    // WREN TEST
    /*
    digitalWrite(FRAM_CS1, LOW);
    SPI.transfer(CMD_WREN); //write enable 
    digitalWrite(FRAM_CS1, HIGH);
    */
    
    // WRITE TEST
    /*
    int addr = 1;
    byte addrMSB = (addr >> 8) & 0xff;
    byte addrLSB = addr & 0xff;
    digitalWrite(FRAM_CS1, LOW);   
    SPI.transfer(CMD_WREN); //write enable 
    digitalWrite(FRAM_CS1, HIGH); //after WREN the /CS goes high  
    digitalWrite(FRAM_CS1, LOW); // and back LOW before op-code
    SPI.transfer(CMD_WRITE); //write command
    SPI.transfer(addrMSB); // address 1
    SPI.transfer(addrLSB); // address 2
    SPI.transfer(2); // data
    digitalWrite(FRAM_CS1, HIGH); 
    */
    
    // READ TEST
    int addr = 1;
    byte addrMSB = (addr >> 8) & 0xff;
    byte addrLSB = addr & 0xff;
    digitalWrite(FRAM_CS1, LOW); 
    SPI.transfer(CMD_READ);
    SPI.transfer(addrMSB);
    SPI.transfer(addrLSB);
    int buf = SPI.transfer(0xFF); // What is the actual purpose of the 0x00?
                              // "Don't Care" in Fig. 5 of f SPI Guide for F-RAM
                              // http://www.cypress.com/?docID=44520
                              // And apparently one can get different results
                              // with 0x00 compared to 0xFF
                              // http://forum.arduino.cc/index.php?topic=162242.15
    digitalWrite(FRAM_CS1, HIGH);
  }
