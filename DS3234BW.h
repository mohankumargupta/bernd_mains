/*
   DS3234BW.h - library for DS3234 RTC  Ver  0.33

  This library contains the following functions:
   1) User date and time is in decimal but is sent and received from RTC in BCD format.
   2) Dow, Day of Week, Mon = 1, Sun = 7. ISO 8601 compliant.
   3) Read DATE     - Date, Month, Year and DoW.
   4) Set  DATE     - Date, Month, Year. Dow is calculated and set.
   5) Read TIME     - Hour, Minute, Second.  24 Hour mode only.
   6) Set  TIME     - Hour, Minute, Second.  24 Hour mode only.
   7) Read seconds in decimal format.
   8) Set seconds in decimal format.
   9) Read DoW      - Day of Week.
  10) Set DoW       - Day of Week based on RTC date, month, year.
  11) Read DS3234 internal temperature in degree C.
  12) Read  Control register in binary format.
  13) Write Control register in binary format.
  14) Read  Control / Status register in binary format.
  15) Write Control / Status register in binary format.
  16) The osc stopped flag is reset after the micro is
      working for longer than a minute.
  16) 
  16) 
  16) 
  16) 
  16) 

  Things to do:
   1)  Unix time, Local time.
   2)  UTC system time.
   2)  Alarms
   3)  Alarm interrupt power supply.

 */



 #ifndef DS3234BW_h
 #define DS3234BW_h

 #define regSec          0x00
 #define regMin          0x01
 #define regHr           0x02
 #define regDoW          0x03
 #define regDate         0x04
 #define regMon          0x05
 #define regYr           0x06
 #define regAlarm1Sec    0x07
 #define regAlarm1Min    0x08
 #define regAlarm1Hr     0x09
 #define regAlarm1Date   0x0A
 #define regAlarm2_Min   0x0B
 #define regAlarm2_Hr    0x0C
 #define regAlarm2_Date  0x0D

 #define regCont  0x0E                              //  Register bit allocations are shown below: 
 const byte regContDefault = 0x07;                  //  B00000111  Ext interrupt enable. Alarm 1 and 2 enable.
/*
 Control register 0Eh/8Eh
  bit7 EOSC   Enable Oscillator (1 if oscillator must be stopped when on battery)
  bit6 BBSQW  Battery Backed Square Wave
  bit5 CONV   Convert temperature (1 forces a conversion NOW)
  bit4 RS2    Rate select - frequency of square wave output
  bit3 RS1    Rate select
  bit2 INTCN  Interrupt control (1 for use of the alarms and to disable square wave)
  bit1 A2IE   Alarm2 interrupt enable (1 to enable)
  bit0 A1IE   Alarm1 interrupt enable (1 to enable)
*/

 #define regContStat  0x0F
 const byte regContStatDefault = 0x83;             //   B 1000 0011
/*
 Control / Status register 0Fh/8Fh
  bit7 OSF      Oscillator Stop Flag (if 1 then oscillator has stopped and time / date are wrong )
  bit6 BB32kHz  Battery Backed 32kHz output (1 if square wave is needed when powered by battery)
  bit5 CRATE1   Conversion rate 1  temperature compensation rate
  bit4 CRATE0   Conversion rate 0  temperature compensation rate
  bit3 EN32kHz  Enable 32kHz output (1 if needed)
  bit2 BSY      Busy with TCXO functions
  bit1 A2F      Alarm 2 Flag - (1 if alarm2 was triggered)
  bit0 A1F      Alarm 1 Flag - (1 if alarm1 was triggered)
*/

 #define regAgeOffset    0x10
 #define regTempMSB      0x11
 #define ds3234Ram       0x14



 #define secInMin              (60UL)
 #define secInHr             (3600UL)
 #define secInDay           (86400UL)
 #define unixSecTo2000  (946684800UL)
 #define LEAP_YEAR(Y)  ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

 static const uint8_t daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };


 typedef unsigned long time_t;

 struct tm {
   int8_t sec;   // seconds 0 to 59
   int8_t min;   // minutes 0 to 59
   int8_t hour;  // hours   0 to 23
   int8_t date;  // date    1 to 31
  uint8_t dow;   // DoW     1 to  7,  1 = Monday, 7 = Sunday.
   int8_t mon;   // month   1 to 12
 uint16_t year;  // year    0 to 2099
  int16_t yday;  // days since January 1st
  uint8_t isdst; // hours of daylight savings time
};


class Time {
public:
       int8_t sec;
       int8_t min;
       int8_t hour;
      uint8_t dow;
       int8_t date;
       int8_t mon;
      int16_t year;
      uint8_t yearShrt;
//   Time();
};



// RTC based on the DS3234 chip connected via SPI and the SPI library
class DS3234BW {
  public:
     DS3234BW();
           uint8_t oldSPISettings;
              void begin(uint8_t _cs_pin);
           uint8_t oscFailure();

           uint8_t readSec();
           uint8_t readMin();
           uint8_t readHour();
           uint8_t readDate();
           uint8_t readMon();
           int16_t readYear();
              void setSec( uint8_t sec);
              void readTIME();
              void setTIME(uint8_t hour, uint8_t min, uint8_t sec);
              void readDATE();
              void setDATE(int8_t date, int8_t mon, int16_t year);
           uint8_t dow;
           uint8_t readDow();
           uint8_t dowCal(uint8_t, uint8_t, int16_t);
              void setDow();

              void setLocTimZonDst( int8_t locTimeZoneHr, int8_t locTimeZoneMin, uint8_t locDayLightSav );

           uint8_t readDst();
//           uint8_t dstCal();
           uint8_t dstCal(uint8_t, int8_t, int8_t, int8_t, int8_t hour, int8_t min, int8_t sec);



//              void updSysTime();                                        
           int32_t unixTime(uint8_t, uint8_t, int16_t, uint8_t, uint8_t, uint8_t);
           int32_t unixSec;
          uint32_t readUnixLoc();
          uint32_t readUnixGmt();




              void unixMinusTz( int8_t, uint8_t, int8_t );
              void breakTime( uint32_t timeStamp);

           uint8_t regContRead();
              void regContWrite ( uint8_t contData);

           uint8_t regContStatRead();
              void regContStatWrite(uint8_t contStatData);

           uint8_t regReadBCD(uint8_t regName);
           uint8_t regReadDEC(uint8_t regName);

             float readInternTemp();
           uint8_t regAgeRead();
              void regAgeWrite(uint8_t age);

              byte error;

              void alarmRepeat( uint8_t data);


  protected:
           uint8_t regReadData;

  private:
           uint8_t ds3234SelPin;
              void csLow();
              void csHigh();
              void csOff();
              void csOn();
 static    uint8_t dec2bcd(uint8_t num);
 static    uint8_t bcd2dec(uint8_t num);

 volatile  int32_t timeUnixGmt;
 volatile uint32_t prevGmtMillis;
              void timeUnixGmtSet();
              void timeUnixGmtUpd();

            int8_t dateFormatChk();
            int8_t timeFormatChk();
volatile    int8_t locTzHr;
 volatile  uint8_t locTzMin;
 volatile  uint8_t locDLightSav;


};

extern DS3234BW RTC;

#endif