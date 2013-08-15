### Decode SPI Data
###
### SCK  = D2 (on positive edge)
### CS   = D3 (negated)
### MOSI = D4
### MISO = D5
###
decode spi posedge lsb D2 !D3 D4 D5

