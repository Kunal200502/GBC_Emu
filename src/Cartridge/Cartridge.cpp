#include <fstream>
#include <iostream>
#include "Cartridge.h"

void Cartridge::loadROM(std::string fileString){
    std::ifstream romFile(fileString, std::ios::binary);
    if(!romFile){
        std::cout << "Cannot open ROM file" << std::endl;
        exit(1);
    }

    // calculating the file size
    romFile.seekg(0, std::ios::end);
    std::streamsize size = romFile.tellg();
    romFile.seekg(0, std::ios::beg);

    rom = std::vector<uint8_t>(size);

    char buffer[4096];
    int romPointer = 0;
    
    // loading the ROM
    while(romFile.read(buffer, 4096) || romFile.gcount() > 0){
        std::streamsize bytesRead = romFile.gcount();
        for(int i = 0; i<bytesRead; i++){
            rom[romPointer+i] = buffer[i]; 
        }
        romPointer += bytesRead;
    }
}

// MBC1 class
// MBC1::MBC1(){
//     rom = std::vector<uint8_t>(2097152, 0);
//     ram = std::vector<uint8_t>(32766, 0);
// }

uint8_t MBC1::read(uint16_t address) const{
    // ROM Bank X0 [read-only]
    if(address >= 0x0000 && address <= 0x3FFF){
        return rom[address];
    }

    // setting the bank to 1 if it is set to 0
    uint8_t bank = rom_bank_low5;
    if(bank == 0){
        bank = 1;
    }

    // ROM BANK 01-7f [read-only switchable]
    if(address >= 0x4000 && address <= 0x7FFF){
        return rom[(address - 0x4000) + 0x4000*bank];
    }

    // RAM Bank 00-03 if any
    if(address >= 0xA000 && address <= 0xBFFF){
        return ram[(address-0xA000) + 0x2000*ram_bank_number];
    }

    std::cout << "Attempted to read from unknow location" << std::endl;
    exit(1);
}

void MBC1::write(uint16_t address, uint8_t value){
    // RAM enable [write only]
    if(address >= 0x0000 && address <= 0x1FFF){
        if((value & 0xF) == 0x4){
            ram_enabled = true;
        }else{
            ram_enabled = false;
        }
        return;
    }

    // ROM bank number [Write only]
    if(address >= 0x2000 && address <= 0x3FFF){
        if((value & 0x1F) == 0x0){
            rom_bank_low5 = 0x1;
        }else{
            rom_bank_low5 = value & 0x1F;
        }
        return;
    }

    // RAM bank number or ROM bank upper 2 bits
    if(address >= 0x4000 && address <= 0x5FFF){
        bank_high2 = value & 3;
        return;
    }

    // ROM or RAM mode select
    if(address >= 0x6000 && address <= 0x7FFF){
        banking_mode = value & 1;
        return;
    }

    // Writing to the RAM 
    if(address >= 0xA000 && address <= 0xBFFF){
        ram[(address-0xA000) + 0x2000*ram_bank_number] = value;
    }

    std::cout << "Attemted to write at an unknown location" << std::endl;
    exit(1);
}


// MBC2 
// uint8_t MBC2::read(uint16_t address) const{
//     // ROM Bank 00 [read-only]
//     if(address >= 0x0000 && address <= 0x3FFF){
//         return rom[address];
//     }

//     // ROM Bank 01-0F (same as MBC1 but only 16 banks are supported)
//     // setting the bank to 1 if it is set to 0
//     uint8_t bank = rom_bank_number;
//     if(bank == 0){
//         bank = 1;
//     }

//     // ROM BANK 01-7f [read-only switchable]
//     if(address >= 0x4000 && address <= 0x7FFF){
//         return rom[(address - 0x4000) + 0x4000*bank];
//     }

// }