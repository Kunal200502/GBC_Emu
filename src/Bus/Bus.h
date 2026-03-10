#pragma once
#include <stdint.h>
#include <vector>
#include "../Cartridge/Cartridge.h"
#include "../Timer/Timer.h"

class Cartridge;

class Bus{
    private:
        MBC1* cartridge; // 0x0000 - 0x7FFF
        std::vector<uint8_t> vram; // 0x8000 - 0x9FFF
        std::vector<uint8_t> wram; // 0xC000 - 0xDFFF
        std::vector<uint8_t> hram; // 0xFF80 - 0xFFFE
        std::vector<uint8_t> IO_registers; // 0xFF00 - 0xFF7F
        std::vector<uint8_t> OAM_memory; // 0xFE00 - 0xFE9F
        uint8_t IE_Register = 0; // 0xFFFF

        Timer timer;
    public:
        Bus();
        uint8_t read(uint16_t) const;
        uint16_t read16(uint16_t) const;
        void write(uint16_t, uint8_t);
        void write16(uint16_t, uint16_t);
        void connectCartridge(MBC1*);
        void stepTimer(uint8_t);
};