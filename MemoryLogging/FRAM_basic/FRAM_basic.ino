// Demo for using SPI FRAM Memory with Ardino
// by Kerry D. Wong
// http://www.kerrywong.com/2012/01/15/using-fram-as-nonvolatile-memory-with-arduino/

#include <SPI.h>
 
const byte CMD_WREN = 0x06; //0000 0110 Set Write Enable Latch
const byte CMD_WRDI = 0x04; //0000 0100 Write Disable
const byte CMD_RDSR = 0x05; //0000 0101 Read Status Register
const byte CMD_WRSR = 0x01; //0000 0001 Write Status Register
const byte CMD_READ = 0x03; //0000 0011 Read Memory Data
const byte CMD_WRITE = 0x02; //0000 0010 Write Memory Data
 
const int FRAM_CS1 = 2; //chip select 1
// const int FRAM_CS2 = 9; //chip select 2
 
/**
 * Write to FRAM (assuming 2 FM25C160 are used)
 * addr: starting address
 * buf: pointer to data
 * count: data length. 
 *        If this parameter is omitted, it is defaulted to one byte.
 * returns: 0 operation is successful
 *          1 address out of range
 */
int FRAMWrite(int addr, byte *buf, int count=1)
{
  int cs = 0;
  cs = FRAM_CS1;
   
  if (addr > 0x7ff) return -1;
   
  byte addrMSB = (addr >> 8) & 0xff;
  byte addrLSB = addr & 0xff;
   
  digitalWrite(cs, LOW);   
  SPI.transfer(CMD_WREN);  //write enable 
  digitalWrite(cs, HIGH);
   
  digitalWrite(cs, LOW);
  SPI.transfer(CMD_WRITE); //write command
  SPI.transfer(addrMSB);
  SPI.transfer(addrLSB);
   
  for (int i = 0;i < count;i++) SPI.transfer(buf[i]);
 
  digitalWrite(cs, HIGH);
   
  return 0;
}
 
/**
 * Read from FRAM (assuming 2 FM25C160 are used)
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
   
  if (addr > 0x7ff) return -1;
 
  byte addrMSB = (addr >> 8) & 0xff;
  byte addrLSB = addr & 0xff;
   
  digitalWrite(cs, LOW);
   
  SPI.transfer(CMD_READ);
  SPI.transfer(addrMSB);
  SPI.transfer(addrLSB);
   
  for (int i=0; i < count; i++) buf[i] = SPI.transfer(0x00);
 
  digitalWrite(cs, HIGH);
   
  return 0;
}
 
void setup()
{
  
  Serial.begin(9600);  
  pinMode(FRAM_CS1, OUTPUT);
  digitalWrite(FRAM_CS1, HIGH);
  // pinMode(FRAM_CS2, OUTPUT);
  // digitalWrite(FRAM_CS2, HIGH);
 
  //Setting up the SPI bus
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);  
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  
}
 
void loop()
{
  
  //Test
  char buf[]="This is a test";
   
  FRAMWrite(1, (byte*) buf, 14);  
  FRAMRead(1, (byte*) buf, 14);
   
  for (int i = 0; i < 14; i++) { 
    Serial.print(i); Serial.print("\t"); Serial.println(buf[i]);
  }
  
  delay(1000);
  
}
