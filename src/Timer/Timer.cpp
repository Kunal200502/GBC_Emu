#include "Timer.h"

uint32_t Timer::getMachineCycleFrequency(){
    switch(timerControl & 0x3){
        case(0): { return 256; break; }
        case(1): { return 4; break; }
        case(2): { return 16; break; }
        case(3): { return 64; break; }
    }
    return 0;
}

void Timer::stepDividerRegister(uint8_t steps){
    dividerRegisterBuffer += steps;
    if(dividerRegister >= 256){
        dividerRegister++;
        dividerRegisterBuffer -= 256;
    }
}

bool Timer::stepTimerCounter(uint8_t steps){
    if((timerControl & 0x4) == 0){
        return false;
    }
    timerCounterBuffer += steps;
    uint16_t machineCycleFrequency = getMachineCycleFrequency();
    if(timerCounterBuffer >= machineCycleFrequency){
        if(timerCounter == 0xFF){
            timerCounter = timerModulo;
            return true;
        }
        timerCounter++;
        timerCounterBuffer -= machineCycleFrequency;
    }
    return false;
}

bool Timer::step(uint8_t steps){
    stepDividerRegister(steps);
    return stepTimerCounter(steps);
}

uint8_t Timer::read(uint16_t address) const{
    switch(address){
        case 0xFF04: return dividerRegister; 
        case 0xFF05: return timerCounter; 
        case 0xFF06: return timerModulo;
        case 0xFF07: return timerControl;
    }
    return 0xFF;
}

void Timer::write(uint16_t address, uint8_t value){
    switch(address){
        case 0xFF04: { dividerRegister = 0; dividerRegisterBuffer = 0; break; }
        case 0xFF05: { timerCounter = value; break; }
        case 0xFF06: { timerModulo = value; break; }
        case 0xFF07: { timerControl = value; break; }
    }
}