// Demo for using SPI FRAM Memory with Ardino
// by Kerry D. Wong
// http://www.kerrywong.com/2012/01/15/using-fram-as-nonvolatile-memory-with-arduino/

// In this blog post I will give an example on interfacing two Ramtron FM25C160 FRAM chips 
// with Arduino. Since the basic operations on an FRAM are very similar to those on a 
// standard EEPROM, I would recommend reading this SPI EEPROM tutorial first 
// if you are not familiar with the protocol.

// The main difference between the operation modes of an FM25C160 and a standard EEPROM 
// is that FM25C160 allows multi-byte sequential reads and writes whereas EEPROM typically 
// has a limited write buffer and long writes beyond the buffer length must be broken 
// into multiple writes. We will take advantage of this in our code later.
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
 
const int FRAM_CS1 = 2; //chip select for memory (2 in Daysimeter v.13)
 
/**
 * Write to FRAM (original code with FM25C160)
                  - FM25C160 is a 16bit memory with 2 address bytes
                  - and 11 address bits, whereas the corresponding values
                  - for FM25V20 (2 MBit) are 3 and 18
                  - for FM25V02 (256 kbit) are 2 and 15
 * addr: starting address
 * buf: pointer to data
 * count: data length. 
 *        If this parameter is omitted, it is defaulted to one byte.
 * returns: 0 operation is successful
 *          -1 address out of range
 */
int FRAMWrite(int addr, byte *buf, int count=1)
{
  int cs = 0;
  cs = FRAM_CS1;
   
  if (addr > 0x7ff) // correct this for 256 kb and 2048 kB chips
    {
    return -1;
    }
   
  byte addrMSB = (addr >> 8) & 0xff;
  byte addrLSB = addr & 0xff;
   
  digitalWrite(cs, LOW);   
  SPI.transfer(CMD_WREN);  //write enable 
  digitalWrite(cs, HIGH);
   
  digitalWrite(cs, LOW);
  SPI.transfer(CMD_WRITE); //write command
  SPI.transfer(addrMSB);
  // SPI.transfer(secondByte); // for the 2 MBit chip
  SPI.transfer(addrLSB);
   
  for (int i = 0;i < count;i++) 
    {
    SPI.transfer(buf[i]);
    }
 
  digitalWrite(cs, HIGH);   
  return 0;
}
 
/**
 * Read from FRAM (original code with FM25C160)
                  - FM25C160 is a 16bit memory with 2 address bytes
                  - and 11 address bits, whereas the corresponding values
                  - for FM25V20 (2 MBit) are 3 and 18
                  - for FM25V02 (256 kbit) are 2 and 15
 * addr: starting address
 * buf: pointer to data
 * count: data length. 
 *        If this parameter is omitted, it is defaulted to one byte.
 * returns: 0 operation is successful
 *          1 address out of range
 */
int FRAMRead(int addr, byte *buf, int count=1)
{
  int cs = 0;
  cs = FRAM_CS1;  
   
  if (addr > 0x7ff) 
    {
    return -1;
    }
 
  byte addrMSB = (addr >> 8) & 0xff;
  byte addrLSB = addr & 0xff;
   
  digitalWrite(cs, LOW);
   
  SPI.transfer(CMD_READ);
  SPI.transfer(addrMSB);
 // SPI.transfer(secondByte); // for the 2 MBit chip
  SPI.transfer(addrLSB);
   
  for (int i=0; i < count; i++) 
    {
    buf[i] = SPI.transfer(0x00); // What is the actual purpose of the 0x00?
    Serial.println(buf[i]);
    }
 
  digitalWrite(cs, HIGH);
   
  return 0;
}
 
void setup()
{
  
  Serial.begin(9600);  
  pinMode(FRAM_CS1, OUTPUT);
  digitalWrite(FRAM_CS1, HIGH);
 
  // Setting up the SPI bus
  SPI.begin();
  SPI.setDataMode(SPI_MODE0); // FRAMs support 0 and 3
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4); // 8 MHz of system clock
  
}
 
void loop()
{
  
  //Test
  char buf[] ="Test";
  char buf2[] ="PROB";
   
  int bufByte;
  int bufByte2;
   
  Serial.println(buf);
  Serial.println(buf2);
   
  int writeFlag = FRAMWrite(1, (byte*) buf, 4);
  int readFlag = FRAMRead(1, (byte*) buf, 4);   

  Serial.println("CHAR looped");   
  for (int i = 0; i < 4; i++) { 
    Serial.print(i); 
    Serial.print("\t"); 
    Serial.print(writeFlag);    
    Serial.print("\t");
    Serial.print(readFlag);    
    Serial.print("\t");
    Serial.print(buf[i]);
    Serial.print("\t");
    Serial.println(buf2[i]); // Problem now that only zeroes are returned
                              // empty (space?) as character
  }
  
  Serial.println("INT in a loop");
  for (int i = 0; i < 5; i++) { 
    bufByte = i*5;
    FRAMWrite(i, (byte*) bufByte);
    bufByte = 1;
    FRAMRead(i, (byte*) bufByte2);
    Serial.print(i);    
    Serial.print("\t");
    Serial.print(bufByte);
    Serial.print("\t");
    Serial.println(bufByte2); // Problem now that only zeroes are returned
  }
  Serial.println(" ");
  
  delay(1000);
  
}
