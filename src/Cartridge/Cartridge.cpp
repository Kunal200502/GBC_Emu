#include "Cartridge.h"

uint8_t Cartridge::read(uint16_t address) const{
    return rom[address];
}
void Cartridge::write(uint16_t address, uint8_t value){
    return;
}

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

void Cartridge::initializeRAM(uint8_t code){
    switch(code){
        case 00: { break; }
        case 02: {  ram = std::vector<uint8_t>(8*1024, 0); break; }
        case 03: {  ram = std::vector<uint8_t>(32*1024, 0); break; }
        case 04: {  ram = std::vector<uint8_t>(128*1024, 0); break; }
        case 05: {  ram = std::vector<uint8_t>(64*1024, 0); break; }
    }
}

// NoMBC class
NoMBC::NoMBC(){}

uint8_t NoMBC::read(uint16_t address) const{
    return rom[address];
}

void NoMBC::write(uint16_t address, uint8_t value){
    return;
}

// MBC1 class
MBC1::MBC1(uint8_t rom_code){
    is_bank_high_ram = (rom_code >= 0x5);
}

uint8_t MBC1::read(uint16_t address) const{
    switch(address >> 12){
        case 0x0: case 0x1: case 0x2: case 0x3: { return rom[address]; }
        case 0x4: case 0x5: case 0x6: case 0x7: { 
            if(is_bank_high_ram){
                return rom[(address - 0x4000) + 0x4000*rom_bank_low5];
            }
            return rom[(address - 0x4000) + 0x4000*((bank_high2 << 5) | rom_bank_low5)];
        }
        case 0xA: case 0xB: { 
            if(!ram_enabled){
                return 0xFF;
            }
            if(is_bank_high_ram){
                return ram[(address - 0xA000) + 0x2000*ram_bank_number];
            }
            return ram[address - 0xA000];
        }
        default:{
            std::cout << "Attempted to read from unknow location" << (int)address << std::endl;
            exit(1);
        }
    }
}

void MBC1::write(uint16_t address, uint8_t value){
    switch(address >> 12){
        case 0x0: case 0x1: {
            if((value & 0xF) == 0xA){
                ram_enabled = true;
            }else{
                ram_enabled = false;
            }
            return;
        }
        case 0x2: case 0x3: {
            rom_bank_low5 = std::max(value & 0x1F, 1);
            return;
        }
        case 0x4: case 0x5: {
            bank_high2 = value & 3;
            return;
        }
        case 0x6: case 0x7: {
            banking_mode = value & 1;
            return;
        }
        case 0xA: case 0xB: {
            if(!ram_enabled){
                return;
            }
            ram[(address-0xA000) + 0x2000*bank_high2] = value;
            return;
        }
        default: {
            std::cout << "Attemted to write at an unknown cartridge location" << (int)address << std::endl;
            exit(1);
        }
    }
}


// MBC3 class
MBC3::MBC3(){
}

uint8_t MBC3::read(uint16_t address) const{
    // ROM Bank 00
    if(address <= 0x3FFF){
        return rom[address];
    }

    // ROM Bank 01-7F
    if(address <= 0x7FFF){
        return rom[(address - 0x4000) + rom_bank_num*0x4000];
    }

    // RAM bank or RTC Register
    if(address >= 0xA000 && address <= 0xBFFF){
        // TODO - setup rtc
        return rom[(address - 0x4000) + ram_rtc_selecter*0x2000];
    }

    std::cout << "Attempted to read from unknow location" << (int)address << std::endl;
    exit(1);
}

void MBC3::write(uint16_t address, uint8_t value){
    // RAM and Timer enable
    if(address <= 0x1FFF){
        if(value && 0xF == 0xA){
            ram_enable = true;
        }else if(value == 0){
            ram_enable = false;
        }
        return;
    }

    // ROM Bank Number
    if(address <= 0x3FFF){
        rom_bank_num = value & 0x7F;
        return;
    }

    // RAM Bank Number - or - RTC Register Select
    if(address <= 0x5FFF){
        ram_rtc_selecter = value;
    }

    // Latch Clock Data
    if(address <= 0x7FFF){
        // TODO
        return;
    }
}


MBC5::MBC5(){
}

uint8_t MBC5::read(uint16_t address) const{
    switch(address & 0xF000){
        case 0x0000: { return rom[address]; }
        case 0x1000: { return rom[address]; }
        case 0x2000: { return rom[address]; }
        case 0x3000: { return rom[address]; }
        case 0x4000: { return rom[((rom_bank_high << 8) | rom_bank_low)*0x4000 + (address - 0x4000)]; }
        case 0x5000: { return rom[((rom_bank_high << 8) | rom_bank_low)*0x4000 + (address - 0x4000)]; }
        case 0x6000: { return rom[((rom_bank_high << 8) | rom_bank_low)*0x4000 + (address - 0x4000)]; }
        case 0x7000: { return rom[((rom_bank_high << 8) | rom_bank_low)*0x4000 + (address - 0x4000)]; }
        case 0xA000: { return ram[(address - 0xA000) + 0x2000*ram_bank_num]; }
        case 0xB000: { return ram[(address - 0xA000) + 0x2000*ram_bank_num]; }
        default: {
            std::cout << "Attempted to read from unknow location" << (int)address << std::endl;
            exit(1);
        }
    }

}

void MBC5::write(uint16_t address, uint8_t value){
    switch(address & 0xF000){
        case 0x0000: { return; }
        case 0x1000: { return; }
        case 0x2000: { rom_bank_low = value; return; }
        case 0x3000: { rom_bank_high = (value & 1); return; }
        case 0x4000: { ram_bank_num = value & 0xF; return; }
        case 0x5000: { ram_bank_num = value & 0xF; return; }

        case 0xA000: { ram[(address-0xA000) + 0x2000*ram_bank_num] = value; return; }
        case 0xB000: { ram[(address-0xA000) + 0x2000*ram_bank_num] = value; return; }
    }
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