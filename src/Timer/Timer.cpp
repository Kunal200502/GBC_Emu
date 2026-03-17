#include "Timer.h"

uint8_t Timer::getMuxBitPos(){
    switch(timerControl & 0x3){
        case 1:
            return 3;
        case 2:
            return 5;
        case 3:
            return 7;
        default:
            return 9;
    }
}

void Timer::stepDividerRegister(){
    dividerRegister += 4;
}

bool Timer::stepTimerCounter(){
    bool muxBit = (dividerRegister >> getMuxBitPos()) & 0x1;
    muxBit = muxBit & ((timerControl & 0x4) >> 2);
    uint8_t timerTick = false;
    if(prevBit && !muxBit){
        timerTick = true;
    }
    prevBit = muxBit;
    if(overflow){
        timerCounter = timerModulo;
        overflow = false;
        return true;
    }
    if(timerTick){
        if(timerCounter == 0xFF){
            overflow = true;
        }
        timerCounter++;
    }
    return false;
}

bool Timer::step(uint8_t steps){
    bool timerOverflow = false;
    for(int i = 0; i<steps; i++){
        stepDividerRegister();
        timerOverflow |= stepTimerCounter();
    }
    return timerOverflow;
}

uint8_t Timer::read(uint16_t address) const{
    switch(address){
        case 0xFF04: return (dividerRegister >> 8); 
        case 0xFF05: return timerCounter; 
        case 0xFF06: return timerModulo;
        case 0xFF07: return timerControl;
    }
    return 0xFF;
}

void Timer::write(uint16_t address, uint8_t value){
    switch(address){
        case 0xFF04: { dividerRegister = 0; break; }
        case 0xFF05: { timerCounter = value; overflow = false; break; }
        case 0xFF06: { timerModulo = value; break; }
        case 0xFF07: { timerControl = value; break; }
    }
}