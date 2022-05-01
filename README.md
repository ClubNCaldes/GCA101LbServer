# GCA101LbServer
LbServer (LoconetOverTCP) Firmware for GCA101 ATMEL ATMEGA328P

Depending on the jumper settings, you can choose 4 different IP/Mask/Gateway. The board has 3 jumpers. Pointing towards the connectors means 0, and pointing towards the heat diffuser means 1. The jumper number 1 means bit 0, and jumper number 3 (closer to the board led) means bit 2.

Value 7 (111, all jumpers pointing towards heat diffuser): IP 192.168.0.200 / Mask 255.255.255.0 / Gateway 192.168.0.1

Value 6 (110): IP 192.168.1.200 / Mask 255.255.255.0 / Gateway 192.168.1.1

Value 5 (101): IP 192.168.100.88 / Mask 255.255.255.0 / Gateway 192.168.100.1

Value 0 (000, all jumpers pointing towards connectors): Values are read from eeprom. First 4 bytes for IP, following 4 bytes mask, following 4 bytes gateway

It is provided a file to be burned in the eeprom as an example (./lntcp/EEPROM.hex). It contains the IP address 192.168.1.50/255.255.255.0/192.168.1.1
