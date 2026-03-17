#pragma once
#include <stdint.h>
#include <iostream>
#include <iomanip>

class Timer{
    private:
        uint16_t dividerRegister = 0;

        bool prevBit = false;

        uint8_t timerCounter = 0;
        bool overflow = false;

        uint8_t timerModulo = 0;
        uint8_t timerControl = 0xF8;

        uint8_t getMuxBitPos();
        void stepDividerRegister();
        bool stepTimerCounter();
    public:
        bool step(uint8_t);
        uint8_t read(uint16_t) const;
        void write(uint16_t, uint8_t);
};