# 1 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino"
# 2 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino" 2
# 3 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino" 2

# 5 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino" 2
# 6 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino" 2
# 7 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino" 2
# 8 "C:\\Users\\Mohan\\Developer\\wowki\\bernd_mains\\bernd_mains.ino" 2

hd44780_I2Cexp lcd;

constexpr int boo = 2;

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
