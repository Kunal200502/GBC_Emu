#pragma once
#include "../Cpu/Cpu.h"
#include "../Bus/Bus.h"
#include "../Timer/Timer.h"
#include "../Cartridge/Cartridge.h"
#include "../PPU/PPU.h"
#include "SaveState.h"
#include <iostream>
#include <memory>
#include <fstream>
#include <string>

class GameBoy{
    private:
        void restoreSaveState(SaveState&);
        std::string cartString;
        int counter = 0;
    public:
        Cpu* cpu;
        Bus* bus;
        PPU* ppu;

        GameBoy();

        void startGameboy(std::string);
};