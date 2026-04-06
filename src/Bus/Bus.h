#pragma once
#include <stdint.h>
#include <vector>
#include "../Cartridge/Cartridge.h"
#include "../Timer/Timer.h"
#include "../Joypad/Joypad.h"
#include "IO_Registers.h"
#include <memory>
#include <iomanip>

enum InterruptType{
    V_Blank_Interrupt,
    STAT_Interrupt,
    Timer_Interrupt,
    Serial_Interrupt,
    Joypad_Interrupt
};

class BusSnapshot{
    public:
        std::vector<uint8_t> vram;
        std::vector<uint8_t> wram;
        std::vector<uint8_t> hram;

        IO_Registers_Snapshot io_registers;
        std::vector<uint8_t> OAM_memory;
        uint8_t IE_Register;
        uint8_t IF_Register;

        JoypadSnapshot joypad;
        TimerSnapshot timer;

        bool start_DMA_transfer;
        uint8_t OAM_DMA_pointer;

        BusSnapshot(std::vector<uint8_t>, std::vector<uint8_t>, std::vector<uint8_t>, IO_Registers*, std::vector<uint8_t>, uint8_t, uint8_t, Joypad, Timer, bool, uint8_t);
        BusSnapshot(){}

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & vram;
            ar & wram;
            ar & hram;
            ar & io_registers;
            ar & OAM_memory;
            ar & IE_Register;
            ar & IF_Register;
            ar & joypad;
            ar & timer;
            ar & start_DMA_transfer;
            ar & OAM_DMA_pointer;
        }
        
};

class Bus{
    private:
        std::unique_ptr<Cartridge> cartridge ; // 0x0000 - 0x7FFF
        std::vector<uint8_t> vram; // 0x8000 - 0x9FFF
        std::vector<uint8_t> wram; // 0xC000 - 0xDFFF
        std::vector<uint8_t> hram; // 0xFF80 - 0xFFFE
        IO_Registers* io_registers; // 0xFF00 - 0xFF7F
        std::vector<uint8_t> OAM_memory; // 0xFE00 - 0xFE9F
        uint8_t IE_Register = 0; // 0xFFFF
        uint8_t IF_Register = 0xE1; // 0xFF0F

        Joypad joypad;
        Timer timer;

        bool start_DMA_transfer = false;
        uint8_t OAM_DMA_pointer = 0;
    public:
        Bus();
        void request_interrupt(InterruptType);
        uint8_t read(uint16_t) const;
        uint16_t read16(uint16_t) const;
        void write(uint16_t, uint8_t);
        void write16(uint16_t, uint16_t);
        void connectCartridge(std::string);
        void stepTimer(uint8_t);
        void processJoyPadInput(SDL_Event&);

        BusSnapshot createSnapshot();
        void restoreSnapshot(BusSnapshot*);
};