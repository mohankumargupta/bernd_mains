#include <Arduino.h>
#line 1 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino"
#include <EEPROMex.h>
#include <EEPROMVar.h>

#include <hd44780.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

hd44780_I2Cexp lcd;

constexpr int boo = 2;

#line 13 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino"
void test_all_lcd_positions();
#line 22 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino"
void test_simple_read_write();
#line 32 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino"
void setup();
#line 39 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino"
void loop();
#line 13 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino"
void test_all_lcd_positions() {
    for (int i=0; i<20; i++) {
      for (int j=0; j<4; j++) {
        lcd.setCursor(i, j);
        lcd.print('8');
      }
    }
}

void test_simple_read_write() {
    int address = 0; // EEPROM address to start writing/reading
    int valueToWrite = 43; // Value to be written to EEPROM
    int readValue; // Variable to store the read value
    EEPROM.write(address, valueToWrite);
    readValue = EEPROM.read(address);
    Serial.print("Read value from EEPROM: ");
    Serial.println(readValue);
}

void setup() {
    Serial.begin(9600);
    lcd.begin(20, 4);
    test_all_lcd_positions();
    test_simple_read_write();
}

void loop() {

}


