#include "Bus.h"
#include <iostream>
#include <iomanip>

Bus::Bus(){
        vram = std::vector<uint8_t>(0x2000, 0);
        wram = std::vector<uint8_t>(0x2000, 0);
        hram = std::vector<uint8_t>(0x80, 0);
        IO_registers = std::vector<uint8_t>(0x80, 0);
        OAM_memory = std::vector<uint8_t>(0xA0, 0);
}

void Bus::connectCartridge(MBC1* cartridge){
    this->cartridge = cartridge;
}

uint8_t Bus::read(uint16_t address) const{
    // reading from the cartridge
    if(address >= 0x0 && address <= 0x7FFF){
        return cartridge->read(address);
    }

    // reading from the video ram
    if(address >= 0x8000 && address <= 0x9FFF){
        return vram[address-0x8000];
    }

    // reading from the cartridge ram
    if(address >= 0xA000 && address <= 0xBFFF){
        return cartridge->read(address);
    }

    // reading from the work ram
    if(address >= 0xC000 && address <= 0xDFFF){
        return wram[address-0xC000];
    }

    // echo ram (maps to wram, nintendo forbids the use of this area)
    if(address >= 0xE000 && address <= 0xFDFF){
        return wram[(address & 0x1FFF)];
    }

    // reading from the OAM
    if(address >= 0xFE00 && address <= 0xFE9F){
        return OAM_memory[address-0xFE00];
    }

    // not usable area
    if(address >= 0xFEA0 && address <= 0xFEFF){
        return 0xFF;
    }

    // reading from the IO Registers
    if(address >= 0xFF00 && address <= 0xFF7F){
        // reading from the timer registers
        if(address >= 0xFF04 && address <= 0xFF07){
            return timer.read(address);
        }
        return IO_registers[address-0xFF00];
    }

    // reading from the hram
    if(address >= 0xFF80 && address <= 0xFFFE){
        return hram[address-0xFF80];
    }

    // reading the IF register 
    if(address == 0xFFFF){
        return IE_Register;
    }

    std::cout << "Attempted to read at an unknown location: " << (int)address << std::endl;
    exit(1);
}

uint16_t Bus::read16(uint16_t address) const{
    uint16_t value = read(address) | (read(address+1) << 8);
    return value;
}

void Bus::write(uint16_t address, uint8_t value){
    // writing to the cartridge ROM
    if(address >= 0x0 && address <= 0x7FFF){
        cartridge->write(address, value);
        return;
    }

    // writing to the VRAM
    if(address >= 0x8000 && address <= 0x9FFF){
        vram[address-0x8000] = value;
        return;
    }

    // writing to the cartridge RAM
    if(address >= 0xA000 && address <= 0xBFFF){
        cartridge->write(address, value);
        return;
    }

    // writing to the WRAM
    if(address >= 0xC000 && address <= 0xDFFF){
        wram[address-0xC000] = value;
        return;
    }

    // writing to the ECHO ram
    if(address >= 0xE000 && address <= 0xFDFF){
        wram[address & 0x1FFF] = value;
        return;
    }

    // writing to the OAM
    if(address >= 0xFE00 && address <= 0xFE9F){
        OAM_memory[address-0xFE00] = value;
        return;
    }
    
    // not usable
    if(address >= 0xFEA0 && address <= 0xFEFF){
        return;
    }

    // writing to the I/O Registers
    if(address >= 0xFF00 && address <= 0xFF7F){
        IO_registers[address-0xFF00] = value;
        return;
    }
    
    // writing to the HRAM
    if(address >= 0xFF80 && address <= 0xFFFE){
        hram[address-0xFF80] = value;
        return;
    }

    // writing to the IE Register
    if(address == 0xFFFF){
        IE_Register = value;
        return;
    }

    std::cout << "Attempted to write at an unknown locatiaon: " <<  (int)address << std::endl;
    exit(1);
}

void Bus::write16(uint16_t address, uint16_t value){
    write(address, value & 0xFF);
    write(address+1, value >> 8);
}

void Bus::stepTimer(uint8_t steps){
    bool overFlow = timer.step(steps);
    if(overFlow){
        uint8_t IF = read(0xFF0F);
        IF |= 0x4;
        write(0xFF0F, IF);
    }
}