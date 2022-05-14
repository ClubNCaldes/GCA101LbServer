# GCA101LbServer
LbServer (LoconetOverTCP) Firmware for GCA101 ATMEL ATMEGA328P

The board has 3 jumpers. Depending on the jumper settings, you can choose 6 different IP/Mask/Gateway or read this data from the EEPROM (first 12 bytes).

Value 1 (001): IP 192.168.0.200 / Mask 255.255.255.0 / Gateway 192.168.0.1

Value 2 (010): IP 192.168.0.50 / Mask 255.255.255.0 / Gateway 192.168.0.1

Value 3 (011): IP 192.168.1.200 / Mask 255.255.255.0 / Gateway 192.168.1.1

Value 4 (100): IP 192.168.1.50 / Mask 255.255.255.0 / Gateway 192.168.1.1

Value 5 (101): IP 192.168.2.200 / Mask 255.255.255.0 / Gateway 192.168.2.1

Value 6 (110): IP 192.168.100.88 / Mask 255.255.255.0 / Gateway 192.168.100.1

Value 0 or 7 (000 or 111): Values are read from eeprom. First 4 bytes for IP, following 4 bytes mask, following 4 bytes gateway

It is provided a file to be burned in the eeprom as an example (./lntcp/EEPROM.hex). It contains the IP address 192.168.1.50/255.255.255.0/192.168.1.1
