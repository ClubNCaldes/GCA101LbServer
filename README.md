# GCA101LbServer
LbServer (LoconetOverTCP) Firmware for GCA101 ATMEL ATMEGA328P

Depending on the switch settings, you can choose 4 different IP/Mask/Gateway. The board has 3 switches. Pointing towards the connectors means 0, and pointing towards the heat diffuser means 1. The switch number 1 means bit 0, and switch numer 3 (closer to the board led) means bit 2.

Value 7 (111): IP 192.168.0.200 / Mask 255.255.255.0 / Gateway 192.168.0.1
Value 6 (110): IP 192.168.1.200 / Mask 255.255.255.0 / Gateway 192.168.1.1
Value 5 (101): IP 192.168.100.88 / Mask 255.255.255.0 / Gateway 192.168.100.1
Value 0 (000, all switches pointing connectors): Values are read from eeprom. First 4 bytes for IP, following 4 bytes mask, following 4 bytes gateway

It is provided a file to be burned in the eeprom as an example (./lntcp/EEPROM.hex). It contains the IP address 192.168.1.50/255.255.255.0/192.168.1.1
