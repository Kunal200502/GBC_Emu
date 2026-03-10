#pragma once
#include <stdint.h>

class Timer{
    private:

        uint16_t dividerRegisterBuffer = 0;
        uint8_t dividerRegister = 0;

        uint16_t timerCounterBuffer = 0;
        uint8_t timerCounter = 0;

        uint8_t timerModulo = 0;
        uint8_t timerControl = 0;

        uint32_t getMachineCycleFrequency();
        void stepDividerRegister(uint8_t);
        bool stepTimerCounter(uint8_t);
    public:
        bool step(uint8_t);
        uint8_t read(uint16_t) const;
        void write(uint16_t, uint8_t);
};