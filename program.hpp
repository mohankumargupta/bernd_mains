#pragma once

class ProgramState
{
};

struct Program_Wrapper
{
    byte system_status = 0b00000100;
    const double version = 0.44;

    enum ArduinoPins { 
        MAINS_RELAY = 3,
        STANDBY_RELAY = 4,
        SOURCE_SELECTION_RELAY = 6,
        DS3234_INTERRUPT = 7,
        MAINS_DC_SUPPLY_CONTROLLER = 8,
        DS3234_SPI_CS = 10,
        SPI_MOSI = 11,
        SPI_MISO = 12,
        SPI_CLK = 13        
    };
    
    enum Mains {
      
    };
    
    enum Battery {

    };

    enum Standby {

    };

    void setup_pins() {
        pinMode(MAINS_DC_SUPPLY_CONTROLLER, OUTPUT);
        digitalWrite(MAINS_DC_SUPPLY_CONTROLLER, HIGH);
        // This is handled by rtc.begin(pin);
        //pinMode(DS3234_SPI_CS, OUTPUT); 

    }

};
