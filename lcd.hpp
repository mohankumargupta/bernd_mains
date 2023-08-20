#pragma once
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> 

class LCD {
  private:
    hd44780_I2Cexp _lcd;
  public:
   enum DIMENSIONS { TWENTY_COLUMNS = 20, FOUR_ROWS = 4 };
   
   void begin(DIMENSIONS cols, DIMENSIONS rows) {
      _lcd.begin(cols,rows);
   }
   
   template<typename T>
   void print(T message) {
     _lcd.print(message);
   }

   template<typename T>
   void printAtRow(uint8_t row, T message) {
     _lcd.setCursor(0, row);
     _lcd.print(message);
   } 

   template<typename T>
   void printAtPosition(uint8_t column, uint8_t row, T message) {
     _lcd.setCursor(column, row);
     _lcd.print(message);
   }      

   void setCursor(uint8_t col, uint8_t row) {
      _lcd.setCursor(col, row);
   }

   void clear() {
     _lcd.clear();
   }


};

