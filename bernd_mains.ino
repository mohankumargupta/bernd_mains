#include <Wire.h>
#include "lcd.hpp"
#include "eeprom.hpp"

LCD mylcd;
EEPROM_Wrapper eeprom;

void setup() {
    Serial.begin(9600);
    
    mylcd.begin(LCD::DIMENSIONS::TWENTY_COLUMNS, LCD::DIMENSIONS::FOUR_ROWS);
    mylcd.printAtRow(0, "hello");
    mylcd.printAtPosition(6, 0, "world");
    eeprom.test_simple_read_write();
}

void loop() {

}

