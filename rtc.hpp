#pragma once

#include "DS3234BW.h"

class RTC_Wrapper {

    private:

    public:

    void begin(uint8_t pin) {
        RTC.begin(pin);
    }

};

