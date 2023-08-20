/*

   DS3234BW.cpp - library for DS3234 RTC  Ver  0.45
   Assembled by Bernd Wachs

  This library is intended to be used with library functions

  The library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.



  1)  DS3234 RTC
  1.1) Vcc = 5V
  1.2) SPI bus
       Speed 2MHz, 4MHz max
       Mode 1. Mode 3 should also work.
       MSB first.
       Single and multiple byte data transfers permitted.



  2)  Daylight savings

  2.1) Australian states and territorial and their timezone names

       AEST AEDT 10:00 +1:00 Victoria,
       AEST AEDT 10:00 +1:00 New South Wales,
       AEST AEDT 10:00 +1:00 Australian Capitol Territory
       AEST AEDT 10:00 +1:00 Tasmania
       AEST      10:00       Queensland
       ACST       9:30       Northern Territory
       ACST ACDT  9:30 +1:00 South Australia
       ACST       9:30       Broken Hill, Yancowinna County. NSW
       AWST ACWT  8:45       Eucla
       AWST AWDT  8:00       Western Australia

                  6:30       Cocos ( Keeling ) island
       CXT        7:00       Christmas island
       NFT       11:30       Norfolk
       LHST LHDT 11:00 +1:00 Lord Howe

       Antartica:
                  5:00       Heard and McDonald islands
       MAWT       5:00       Mawson station
       DAVT       7:00       Davis
       CAST       8:00       Casey
                  8:00       Fuso Horário de Casey.  Is same place as line above ?
       

  2.2) Must be able to handle 16 timezones.
       2.2.1)  Time zone names comprises of a maximum 4 characters.
       2.2.2)  DST starts  2:00 on first Sunday in October. Plus 1 hour
                   DST stops   3:00 DST on first Sunday in April. Minus 1 hour. 

  2.3) Daylight savings start and finish times

  2.4) European timezones

  2.4.1)       England
  2.4.2)  CET  Eastern Europe time
  2.4.3)  C    Central Europe time
  2.4.4)  C    Western Europe time

  2.5)  Date and time format verification routines.
  2.6) 
  2.7) 
  2.8) 
  2.9) 

*/




   #include "Arduino.h"
   #include "DS3234BW.h"
   #include <SPI.h>

//  const int MODE = 0x60;                                  //  60 = disable Oscillator and Battery SQ wave @1Hz, temp compensation, Alarms disabled

  DS3234BW::DS3234BW() {
             SPI.begin();
             }

void DS3234BW::csHigh() {
     digitalWrite(ds3234SelPin, HIGH);                      //  Disable the device for SPI communication
     SPCR = oldSPISettings;                                 //  Return the old SPI settings.
//   Serial.println( " cpp1  CS gone High" );
     }

void DS3234BW::csLow() {
//   Serial.println( " cpp2  CS going Low" );
     oldSPISettings = SPCR;                                 //  Store the old SPI control register settings
     SPI.setBitOrder(MSBFIRST);                             //  Set the SPI settings for the DS3234
     SPI.setDataMode(SPI_MODE1);
     digitalWrite(ds3234SelPin, LOW);                       //  Enable the device for communication
     }

void DS3234BW::csOff() {
     digitalWrite(ds3234SelPin, HIGH );                     //  Disable the device for communication
//   Serial.println( " cpp3  CS gone High" );
     }

void DS3234BW::csOn() {
//   Serial.println( " cpp4  CS going Low" );
     digitalWrite(ds3234SelPin, LOW );                      //  Enable the device for SPI communication
     }

void DS3234BW::begin(uint8_t _cs_pin) {
     ds3234SelPin = _cs_pin;
     pinMode(ds3234SelPin,OUTPUT);
     digitalWrite( ds3234SelPin, HIGH );
//   Serial.print("\n cpp5  Cont/Stat reg = "); Serial.println(regContStatRead(), BIN);
//   Serial.println(" cpp6  RTC begin finished." );
     }


   uint8_t DS3234BW::oscFailure() {                        //  Checks Con/Stat reg 0F if Osc has stopped.
        uint8_t oscStatus;
        oscStatus = regContStatRead();
//      Serial.print("\n\n cpp7  Cont/Stat reg = "); Serial.print(oscStatus, BIN); Serial.println("\n");
        if ( (oscStatus & 0x80) == 0x80 ) {                 //  Power / Battery faulty.
//            Serial.println(" cpp8  OSC stopped.\n");
              regContStatWrite( oscStatus &= regContStatDefault ); 
              return 1;
              }
          else {
//              Serial.println(" cpp9  OSC working.\n");
                return  0;                                  //  Power and Battery are OK.
               }
        }


uint8_t DS3234BW::readSec() {
//        dstCal();
//        timeUnixGmtUpd();
        return bcd2dec( regReadBCD(regSec) );
        }

   void DS3234BW::setSec( uint8_t sec ) {
        if ( (sec >= 0) && (sec < 60)) {
              csLow();
              SPI.transfer(regSec | 0x80);
              SPI.transfer(dec2bcd(sec));
              csHigh();
              prevGmtMillis = millis();
//      uint8_t val = regContStatRead();       //  ?
              timeUnixGmtSet();
              }
        }

uint8_t DS3234BW::readMin() {
//        timeUnixGmtUpd();
        return bcd2dec( regReadBCD(regMin) );
        }

uint8_t DS3234BW::readHour() {
        timeUnixGmtUpd();
        uint8_t hr;
        hr = regReadDEC(regHr) & 0b00111111;
//      Serial.print(" cppx  hour = "); Serial.println(hr, BIN);
        return hr;
        }

uint8_t DS3234BW::readDate() {
//      Serial.print(" cppx  date = "); Serial.println(regReadDEC(regDate));
//        timeUnixGmtUpd();
        return regReadDEC(regDate);
        }

uint8_t DS3234BW::readMon() {
//        timeUnixGmtUpd();
        return bcd2dec((regReadBCD(regMon)) & 0x1F);
        }

int16_t DS3234BW::readYear() {
        uint8_t mon, yearShrt;
        int16_t year;
//        timeUnixGmtUpd();
        mon = regReadBCD(regMon);
//      Serial.print(" cppx  reg mon = "); Serial.println(mon, BIN);
        if ( mon > 0x12 ) {
//           Serial.println(" cppx  year > 2000");
             return  (regReadDEC(regYr)+2000);
             }
          else {
                return  regReadDEC(regYr)+1900;
               }
         }

void DS3234BW::readTIME() {
     uint8_t sec, min, hour;
     csLow();
     SPI.transfer(regSec);
     sec  = bcd2dec(SPI.transfer(-1));
     min  = bcd2dec(SPI.transfer(-1));
     hour = bcd2dec(SPI.transfer(-1));
     csHigh();
//   Serial.print(" cppx  RTC time - "); Serial.print(hour); Serial.print(":");  Serial.print(min); Serial.print(":"); Serial.println(sec);
//     timeUnixGmtUpd();
     }

void DS3234BW::setTIME(uint8_t hour, uint8_t min, uint8_t sec) {
     uint8_t dataTemp1;
     if (((hour>=0) && (hour<24)) && ((min>=0) && (min<60)) && ((sec>=0) && (sec<60))) {
//        Serial.print(" cppx  TIME = "); Serial.println( hour, DEC );
          csLow();
          SPI.transfer( regSec | 0x80);
          SPI.transfer(dec2bcd(sec));
          SPI.transfer(dec2bcd(min));
          SPI.transfer(dec2bcd(hour) & 0b00111111);         //  24 Hr mode.
          csHigh();
          prevGmtMillis = millis();
          timeUnixGmtSet();
          dataTemp1 = regContStatRead();
          dataTemp1 &= 0x7F;
          regContStatWrite( dataTemp1 );
//        Serial.print(" cppx  Time entered - "); Serial.print(hour); Serial.print(":");  Serial.print(min); Serial.print(":");  Serial.println(sec);
          }
      }


void DS3234BW::readDATE() {
     uint8_t dow;
     uint8_t date, mon, yearShrt, century;
     int16_t year;
     csLow();
     SPI.transfer(regDoW);
     dow  = SPI.transfer(-1);                               //  3
     date = bcd2dec(SPI.transfer(-1));                      //  4
     mon  = SPI.transfer(-1);                               //  5
     yearShrt = bcd2dec(SPI.transfer(-1));                  //  6
     csHigh();
//   Serial.print(" cppx  RTC Month = "); Serial.println( mon, BIN );
     if ( mon > 0x12 ) {
          year = 2000 + yearShrt;
//        Serial.println(" cppx  2000 + "); 
          }
       else {
             year = 1900 + yearShrt;
//           Serial.println(" cppx  1900 + "); 
             }
//   Serial.print(" cppx  Year = "); Serial.println( year );
     mon = bcd2dec(mon & 0x1F);
//   Serial.print(" cppx  Month = "); Serial.println( mon );
//     timeUnixGmtUpd();
     }

 void DS3234BW::setDATE(int8_t date, int8_t mon, int16_t year) {
      uint8_t dow;
      uint8_t i, yearShrt, century;
      if (((date>0) && (date<=31)) && ((mon>0) && (mon<=12)) && ((year>1900) && (year<2199))) {
          dow = dowCal( date, mon, year);
//        Serial.print(" cppx  DoW = "); Serial.println( dow );
          csLow();
          if (year > 2000) {
              century = 0x80;
              yearShrt = year - 2000;
              }
             else {
                   century = 0x00;
                   yearShrt = year - 1900;
                   }
          SPI.transfer( regDoW | 0x80);
          SPI.transfer(dow);
          SPI.transfer(dec2bcd(date));
          SPI.transfer(dec2bcd(mon) | century);
          SPI.transfer(dec2bcd(yearShrt));
          csHigh();
          prevGmtMillis = millis();
          timeUnixGmtSet();
          setDow();
//        Serial.print(" cppx  Date entered - "); Serial.print(date); Serial.print("/");  Serial.print(mon); Serial.print("/");  Serial.println(year);
          }
      }


 void DS3234BW::setLocTimZonDst( int8_t locTimeZoneHr, int8_t locTimeZoneMin, uint8_t locDayLightSav ) {
      if ( ((locTimeZoneHr>-12) && (locTimeZoneHr<12)) && ((locTimeZoneMin>-60) && (locTimeZoneMin<60)) && ((locDayLightSav == 0) || (locDayLightSav == 1)) ) {
            locTzHr = locTimeZoneHr;
            locTzMin = locTimeZoneMin;
            locDLightSav = locDayLightSav;
           }
       }


uint8_t DS3234BW::readDow() {
//      Serial.print(" cpp9  DoW read "); Serial.println( regReadDEC(regDoW) );
        return regReadDEC(regDoW);
        }

uint8_t DS3234BW::dowCal( uint8_t date, uint8_t mon, int16_t year) {  //  Dow 1 = Monday, 7 = Sunday.
//      date = 23; mon = 7; year = 2017;
        uint8_t dow;
        if ( ((date > 0) && (date <= 31)) && ((mon > 0) && (mon <= 12)) && ((year >= 0) && (year < 3000)) ) {
               if (mon < 3) {                                                  // convert months to 2...14
                   mon += 12;
                   year -= 1;
                  }
               dow = (((date + (((mon+1)*26) / 10) + year + (year/4) + (6 * (year/100)) + (year/400)) -1) % 7);
               if ( dow == 0 ) {
                    dow = 7;
                    }
             }
          else {
                dow = -1;
                }
//      Serial.print(" cppx  Dow cal Date "); Serial.print( date ); Serial.print( "  Mon "); Serial.print( mon );
//      Serial.print(" cppx  Year "); Serial.print( year ); Serial.print( "  DoW cal "); Serial.println( dow );
        return dow;
        }

 void DS3234BW::setDow() {
      uint8_t dow;
      dow = dowCal( readDate(), readMon(), readYear() );
//    Serial.print(" cppx  DoW set "); Serial.println( dow );
      csLow();
      SPI.transfer( regDoW | 0x80);
      SPI.transfer(dec2bcd(dow));
      csHigh();
      }


/*
// uint8_t DS3234BW::dstCal( uint8_t date, int8_t mon, int8_t year, uint8_t doW, int8_t hour, int8_t min, int8_t sec) {  //  Dst 0 = DST off, 1 = DST on.

 uint8_t DS3234BW::dstCal() {  //  DST Off = 0, DST On > 0.

         int8_t tempData1;
//      uint8_t doW;
//       int8_t year;
//       int8_t mon;
//       int8_t date;
//       int8_t hour;
//       int8_t min;
//       int8_t sec;
       
//   Serial.println("  Got here.");

      if ( (mon == 10) && (locDLightSav == 0) ) {                                //  DST Off to On routine.
            tempData1 = dowCal( 1, 10, year );                                   //  Day of week for 1st day of month.
            tempData1 = 1 + (7 - tempData1);                                     //  Date of first Sunday.
            if ( ((date == tempData1) && (hour >= 2)) || (date > tempData1) ) {  //  DST turn On.
                  setTIME( (hour + 1), min, sec );
                  unixSec = unixTime( readDate(), readMon(), readYear(), readHour(), readMin(), readSec() );  //  Local time.
                  locDLightSav = 1;                                             //  Store DST state.
                  }
            }
        else if ( (mon > 10) || (mon < 4) ) {                                    //  DST On.
                   locDLightSav = 1;
                   }

      if ( (mon == 4) && (locDLightSav > 0) ) {                                  //  DST On to Off transition.
            tempData1 = dowCal( 1, 4, year );                                    //  Day of week for 1st day of month.
            tempData1 = 1 + (7 - tempData1);                                     //  Date of first Sunday.
            if ( ((date == tempData1) && (hour >= 3)) || (date > tempData1) ) {  //  DST turn Off.
                  setTIME( (hour - 1), min, sec );
                  unixSec = unixTime( readDate(), readMon(), readYear(), readHour(), readMin(), readSec() );  //  Local time.
                  locDLightSav = 0;                                              //  Store DST state.
                  }
            }
        else if ( (mon > 4) && (mon < 10) ) {                                    //  DST Off.
                   locDLightSav = 1;
                   }
        return locDLightSav;

  }

*/


 uint32_t DS3234BW::readUnixLoc() {
          unixSec = unixTime( readDate(), readMon(), readYear(), readHour(), readMin(), readSec() );    //  Local time.
          return unixSec;
          }

 uint32_t DS3234BW::readUnixGmt() {
          timeUnixGmtUpd();
          return timeUnixGmt;
          }

 void DS3234BW::timeUnixGmtSet() {
      prevGmtMillis = millis();
      timeUnixGmt = readUnixLoc();    //  Local time.
      timeUnixGmt = timeUnixGmt - ( (locTzHr * 3600) + (locTzMin * 60) + (locDLightSav * 3600) );         //  Local - ( TZ offset + DST )
//    Serial.print(" cppx  Unix UTC set = "); Serial.println(timeUnixGmt);
      }







 void DS3234BW::unixMinusTz(int8_t tZHour, uint8_t tZMin, int8_t dayLightSavMin ) {                    //  ?
      timeUnixGmt = timeUnixGmt - ( (tZHour * 3600) + (tZMin * 60) + (dayLightSavMin * 3600) ); 
//    Serial.print(" cppx  Sys UTC cal = "); Serial.println(timeUnixGmt);
      }



uint8_t DS3234BW::regContRead() {                          //  Read binary data from Control Register.
        uint8_t dataBin;
        csLow();
        SPI.transfer(regCont);
        dataBin = SPI.transfer(-1);
        csHigh();
        return dataBin;
        }

 void DS3234BW::regContWrite ( uint8_t dataBin) {          //  Write binary data to Control Register.
      csLow();
      SPI.transfer(regCont | 0x80);
      SPI.transfer(dataBin);
      csHigh();
      }


uint8_t DS3234BW::regContStatRead() {                      //  Read binary data from Control / Status Register.
	uint8_t contStat = 0;
	csLow();
	SPI.transfer(regContStat);
	contStat = SPI.transfer(-1);
	csHigh();
	return contStat;
        }

 void DS3234BW::regContStatWrite(uint8_t contStatData) {   //  Write binary data to Control / Status Register.
      csLow();
      SPI.transfer(regContStat | 0x80);
      SPI.transfer(contStatData);
      csHigh();
      }


 float DS3234BW::readInternTemp() {                        //  Read DS3234 internal temperature.
       float rtcTemp;
       uint8_t temp_msb, temp_lsb;
       int8_t nint;
       csLow();
       SPI.transfer(regTempMSB);
       temp_msb =  SPI.transfer(-1);
       temp_lsb =  (SPI.transfer(-1)) >> 6;
       csHigh();
       if ((temp_msb & 0x80) != 0)
            nint = temp_msb | ~((1 << 8) - 1);              // if negative get two's complement
       else
            nint = temp_msb;
       rtcTemp = 0.25 * temp_lsb + nint;
       return rtcTemp;
       }

uint8_t DS3234BW::regAgeRead() {                           //  Read binary data from Age register.
        uint8_t age;
        csLow();
        SPI.transfer(regContStat);
        age = SPI.transfer(-1);
        csHigh();
        return age;
        }

 void DS3234BW::regAgeWrite(uint8_t age) {                 //  Write binary data to Age register.
      csLow();
      SPI.transfer(regAgeOffset | 0x80);
      SPI.transfer(age);
      csHigh();
      }

uint8_t DS3234BW::regReadDEC(uint8_t regName) {
        uint8_t dataDec;
        csLow();
        SPI.transfer(regName);
        dataDec = bcd2dec(SPI.transfer(-1));
        csHigh();
        return dataDec;
        }

uint8_t DS3234BW::regReadBCD(uint8_t regName) {
        uint8_t dataBCD;
        csLow();
        SPI.transfer(regName);
        dataBCD = (SPI.transfer(-1));
        csHigh();
        return dataBCD;
        }




 void DS3234BW::alarmRepeat( uint8_t data) {               //  1 sec / 1 min  repeat alarm.
      uint8_t data2 = 0;
      csLow();
      if ( data == 0 ) {                                    //  Sec interrupt
           Serial.println("  Cpp Sec Alm data");
           SPI.transfer(regAlarm1Sec | 0x80);
           SPI.transfer(0x80);
           SPI.transfer(0x80);
           SPI.transfer(0x80);
           SPI.transfer(0x80);
           SPI.transfer(regAlarm2_Min | 0x80);
           SPI.transfer(0x00);
           SPI.transfer(0x00);
           SPI.transfer(0x00);
           }
        else if ( data == 1 ) {                             //  Min interrupt
                  Serial.println("  Cpp Min Alm data");
                  SPI.transfer(regAlarm2_Min | 0x80);
                  SPI.transfer(0x80);
                  SPI.transfer(0x80);
                  SPI.transfer(0x80);
                  SPI.transfer(regAlarm1Sec | 0x80);
                  SPI.transfer(0x00);
                  SPI.transfer(0x00);
                  SPI.transfer(0x00);
                  SPI.transfer(0x00);
                  }
      SPI.transfer( regCont );
      data2 = (SPI.transfer(-1));
//    Serial.print(" cppx  Cont Reg "); Serial.println(data2, BIN);
      if ( data == 0 ) {
           data2 |= 0x05;
           }
        else if ( data == 1 ) {
                  data2 |= 0x06;
                  }
//    Serial.print(" cppx  Reg Alm data "); Serial.println(data2, BIN);

      SPI.transfer(regCont);
      SPI.transfer(data2);
      csHigh();
      }






// PRIVATE FUNCTIONS

uint8_t DS3234BW::dec2bcd(uint8_t dec) {                   //  Convert Decimal to Binary Coded Decimal (BCD)
        return ((dec/10 * 16) + (dec % 10));
        }

uint8_t DS3234BW::bcd2dec(uint8_t bcd) {                   //  Convert Binary Coded Decimal (BCD) to Decimal
        return ((bcd/16 * 10) + (bcd % 16));
        }




void DS3234BW::timeUnixGmtUpd() {
     while( millis() - prevGmtMillis >= 1000){      
            ++timeUnixGmt;
            prevGmtMillis += 1000;
//          Serial.print(" cppx  Sys time = "); Serial.println(timeUnixGmt);
            }
      }


// Convert Date and Time into Unix time.
  int32_t DS3234BW::unixTime( uint8_t date, uint8_t mon, int16_t year, uint8_t hour, uint8_t min, uint8_t sec ) {
          uint32_t timeUnix, days;
          uint8_t yOffset, i, monthLength;
          if ( year > 1970 ) {
               yOffset = year - 1970;
             }
          timeUnix = yOffset * (secInDay * 365);       // seconds from 1970 till 1 jan 00:00:00 of the given year
          for (i = 0; i < yOffset; i++) {
               if (LEAP_YEAR(i)) {
               timeUnix +=  secInDay;                  // add extra days for leap years
               }
          }
          for (i = 1; i < mon; i++) {                       // add days for this year, months start from 1
               if ( (i == 2) && LEAP_YEAR(yOffset)) { 
                    timeUnix += 29 * secInDay;
                   }
                 else {
                       timeUnix += daysInMonth[i-1] * secInDay;   // monthDay array starts from 0
                       }
          }
          timeUnix += (date-1) * secInDay;
          timeUnix += hour * secInHr;
          timeUnix += min * secInMin;
          timeUnix += sec;
//        Serial.print(" cppx  Date/Time -> Unix "); Serial.print(date); Serial.print("/"); Serial.print(mon); Serial.print("/"); Serial.print(year); Serial.print("  "); Serial.print(hour); Serial.print("/"); Serial.print(min); Serial.print("/"); Serial.print(sec); Serial.print(", Unix time  "); Serial.println(timeUnix);
          return timeUnix; 
          }




/*

 void DS3234BW::breakTime( uint32_t timeStamp ) {

//  Break the given time_t into time components
//  this is a more compact version of the C library localtime function
//  note that year is offset from 1970 !!!

    uint8_t year;
    uint8_t month, monthLength;
    uint32_t time;
    unsigned long days;

    time = (uint32_t)timeStamp;
    tm.sec = time % 60;
    time /= 60;                                             // now it is minutes
    tm.Minute = time % 60;
    time /= 60;                                             // now it is hours
    tm.Hour = time % 24;
    time /= 24;                                             // now it is days
    tm.Wday = ((time + 4) % 7) + 1;                         // Sunday is day 1 

    year = 0;  
    days = 0;
    while( (unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
            year++;
            }
    tm.Year = year;                                         // year is offset from 1970 

    days -= LEAP_YEAR(year) ? 366 : 365;
    time  -= days;                                          // now it is days in this year, starting at 0

    days=0;
    month=0;
    monthLength=0;
    for ( month=0; month<12; month++ ) {
          if ( month==1) {                                  // February
               if ( LEAP_YEAR(year)) {
                    monthLength=29;
                  } 
                 else {
                       monthLength=28;
                       }
              }
            else {
                  monthLength = monthDays[month];
                  }
    
          if ( time >= monthLength ) {
               time -= monthLength;
               }
            else {
                  break;
                  }
        }
    tm.Month = month + 1;                                   // Jan is month 1  
    tm.Day = time + 1;                                      // day of month
    }

*/


DS3234BW RTC = DS3234BW(); // create an instance for the user






//   Development code ---------------------------------------------------------------  

/*


  1)  Daylight Savings routines

      Calculates the dates for start / finish for the occurrence of Sundays in a month

//  --------------------------------------------------------------------------
//   FUNC: 6/11/11 - Returns the date for Nth day of month. For instance,
//                   it will return the numeric date for the 2nd Sunday of April
//  PARAMS: year, month, day of week, Nth occurence of that day in that month
//  RETURNS: date
//  NOTES: There is no error checking for invalid inputs.
// ---------------------------------------------------------------------------
char NthDate(int year, char month, char DOW, char NthWeek){
  char targetDate = 1;
  char firstDOW = dow(year,month,targetDate);
  while (firstDOW != DOW){
    firstDOW = (firstDOW+1)%7;
    targetDate++;
  }
  //Adjust for weeks
  targetDate += (NthWeek-1)*7;
  return targetDate;
}
 
int main(void){
  //Used to test on a computer
  sprintf(buff,"%i",NthDate(myYear,myMonth,myDOW,myNthWeek));
  printf("%s\n",buff);
}


--------------------------

*/   








