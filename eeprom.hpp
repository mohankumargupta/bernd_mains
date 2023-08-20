#pragma once

#include <EEPROMex.h>
#include <EEPROMVar.h>

class EEPROM_Wrapper
{

public:
    void test_simple_read_write() {
      int address = 0; // EEPROM address to start writing/reading
      int valueToWrite = 43; // Value to be written to EEPROM
      int readValue; // Variable to store the read value
      EEPROM.write(address, valueToWrite);
      readValue = EEPROM.read(address);
      Serial.print("Read value from EEPROM: ");
      Serial.println(readValue);
    }
};



