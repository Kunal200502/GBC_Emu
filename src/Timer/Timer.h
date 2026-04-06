#pragma once
#include <stdint.h>
#include <iostream>
#include <iomanip>

class TimerSnapshot{
    public:
        uint16_t dividerRegister;
        bool prevBit;
        uint8_t timerCounter;
        bool overflow;
        uint8_t timerModulo;
        uint8_t timerControl;

        TimerSnapshot(uint16_t, bool, uint8_t, bool, uint8_t, uint8_t);
        TimerSnapshot(){}
        
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & dividerRegister;
            ar & prevBit;
            ar & timerCounter;
            ar & overflow;
            ar & timerModulo;
            ar & timerControl;
        }
};

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
        TimerSnapshot createSnapshot();
        void restoreSnapshot(TimerSnapshot*);
};