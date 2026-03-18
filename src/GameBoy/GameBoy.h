#pragma once
#include "../Cpu/Cpu.h"
#include "../Bus/Bus.h"
#include "../Timer/Timer.h"
#include "../Cartridge/Cartridge.h"
#include "../PPU/PPU.h"
#include <iostream>

class GameBoy{
    public:
        Cpu* cpu;
        Bus* bus;
        PPU* ppu;

        GameBoy();

        void startGameboy(MBC1*);
};