#include <Wire.h>
#include "program.hpp"
#include "lcd.hpp"
#include "eeprom.hpp"
#include "rtc.hpp"

Program_Wrapper program;
LCD_Wrapper lcd;
EEPROM_Wrapper eeprom;
RTC_Wrapper rtc;

void setup_lcd()
{
  lcd.begin(LCD_Wrapper::DIMENSIONS::TWENTY_COLUMNS, LCD_Wrapper::DIMENSIONS::FOUR_ROWS);
  //lcd.printAtRow(0, "hello");
  //lcd.printAtPosition(6, 0, "world");
  
}

void read_eeprom()
{
}

void print_intro()
{
  Serial.print(F("\n\n  Mains 230VAC monitor.  Ver "));
  Serial.println(program.version);
  Serial.println("");
  /*
  Serial.println(__cplusplus);
  Serial.println(__GNUC__);
  Serial.println(__GNUC_MINOR__);
  Serial.println(__GNUC_PATCHLEVEL__);
  */
}

void setup()
{
  Serial.begin(9600);
  print_intro();
  
  program.setup_pins();
  rtc.begin(Program_Wrapper::ArduinoPins::DS3234_SPI_CS);

  setup_lcd();
  read_eeprom();
  eeprom.test_simple_read_write();
}

void loop()
{
}
