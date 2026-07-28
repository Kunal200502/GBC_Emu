#include "Cartridge.h"

CartridgeSnapshot::CartridgeSnapshot(
    std::vector<uint8_t> ram_state, 
    std::vector<bool> bool_arr_state,
    std::vector<uint8_t> uint8_arr_state
){
    ram = ram_state;
    bool_arr = bool_arr_state;
    uint8_arr = uint8_arr_state;
}

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

CartridgeSnapshot NoMBC::createSnapshot(){
    return CartridgeSnapshot();
}

void NoMBC::restoreSnapshot(CartridgeSnapshot&){
    return;
}

// MBC1 class
MBC1::MBC1(uint8_t rom_code){
    is_bank_high_ram = (rom_code < 0x5);
}

uint8_t MBC1::read(uint16_t address) const{
    switch(address >> 12){
        case 0x0: case 0x1: case 0x2: case 0x3: { 
            if(is_bank_high_ram){
                return rom[address];
            }
            return rom[address + 0x4000*(bank_high2 << 5)];
         }
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
                return ram[(address - 0xA000) + 0x2000*bank_high2];
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
            if(is_bank_high_ram){
                ram[(address-0xA000) + 0x2000*bank_high2] = value;
            }else{
                ram[address-0xA000] = value;
            }
            return;
        }
        default: {
            std::cout << "Attemted to write at an unknown cartridge location" << (int)address << std::endl;
            exit(1);
        }
    }
}


CartridgeSnapshot MBC1::createSnapshot(){
    std::vector<bool> bool_map = {ram_enabled, is_bank_high_ram, banking_mode};
    std::vector<uint8_t> arr_map = {rom_bank_low5, bank_high2};

    return CartridgeSnapshot(ram, bool_map, arr_map);
}

void MBC1::restoreSnapshot(CartridgeSnapshot& snapshot){
    ram = snapshot.ram;
    
    ram_enabled = snapshot.bool_arr[0];
    is_bank_high_ram = snapshot.bool_arr[1];
    banking_mode = snapshot.bool_arr[2];

    rom_bank_low5 = snapshot.uint8_arr[0];
    bank_high2 = snapshot.uint8_arr[1];
}



// MBC2 Class
uint8_t MBC2::read(uint16_t address) const{
    switch(address >> 12){
        case 0x0: case 0x1: case 0x2: case 0x3: {
            return rom[address];
        }
        case 0x4: case 0x5: case 0x6: case 0x7: {
            return rom[(address - 0x4000) + rom_bank_num*0x4000];
        }
        case 0xA: case 0xB: {
            if(!ram_enable){
                return 0xFF;
            }
            return ram[address & 0x1FF] & 0xF;
        }
        default: {
            std::cout << "Attempted to read from unknow location" << (int)address << std::endl;
            exit(1);
        }
    }
}

void MBC2::write(uint16_t address, uint8_t value){
    switch(address >> 12){
        case 0x0: case 0x1: case 0x2: case 0x3: {
            // for selecting ROM bank number (0x0 - 0xF)
            if((address >> 8) & 1){
                rom_bank_num = value & 0xF;
            }
            // for enabling disablling RAM
            else{
                if((value & 0xF) == 0xA){
                    ram_enable = true;
                }else{
                    ram_enable = false;
                }
            }
        }
        case 0xA: case 0xB: {
            if(!ram_enable){
                return;
            }
            ram[address & 0x1FF] = value;
        }
    }
}

CartridgeSnapshot MBC2::createSnapshot(){
    std::vector<bool> bool_map = {ram_enable};
    std::vector<uint8_t> arr_map = {rom_bank_num};

    return CartridgeSnapshot(ram, bool_map, arr_map);
}

void MBC2::restoreSnapshot(CartridgeSnapshot& snapshot){
    ram_enable = snapshot.bool_arr[0];
    rom_bank_num = snapshot.uint8_arr[0];
}

uint8_t MBC3::read(uint16_t address) const{
    switch(address >> 12){
        case 0x0: case 0x1: case 0x2: case 0x3: {
            return rom[address];
        }
        case 0x4: case 0x5: case 0x6: case 0x7: {
            return rom[(address - 0x4000) + 0x4000*rom_bank_num];
        }
        case 0xA: case 0xB: {
            // reading from the RAM
            if(ram_rtc_register <= 0x7){
                if(!ram_enable){
                    return 0xFF;
                }
                return ram[(address - 0xA000) + 0x2000*ram_rtc_register];
            }
            // reading from the RTC register
            else{
                switch(ram_rtc_register){
                    case 0x8: { return rtc_s; }
                    case 0x9: { return rtc_m; }
                    case 0xA: { return rtc_h; }
                    case 0xB: { return rtc_dl; }
                    case 0xC: { return rtc_dh; }
                }
            }
        }
        default: {
            std::cout << "Attempted to read from unknow location" << (int)address << std::endl;
            exit(1);
        }
    }
}

void MBC3::write(uint16_t address, uint8_t value){
    switch(address >> 12){
        case 0x0: case 0x1: {
            if((value & 0xF) == 0xA){
                ram_enable = true;
            }else{
                ram_enable = false;
            }
            break;
        }
        case 0x2: case 0x3: {
            rom_bank_num = value;
            break;
        }
        case 0x4: case 0x5: {
            ram_rtc_register = value;
            break;
        }
        case 0x6: case 0x7: {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            uint64_t time = std::chrono::duration_cast<std::chrono::seconds>(end - clock).count();
            uint8_t temp = rtc_s;
            
            // updating seconds
            rtc_s = (temp + time)%60;
            time = (temp + time)/60;

            // updating minutes
            temp = rtc_m;
            rtc_m = (temp + time)%60;
            time = (temp + time)/60;

            // updating hours
            temp = rtc_h;
            rtc_h = (temp + time)%24;
            time = (temp + time)/60;
            
            break;
        }
        case 0xA: case 0xB: {
            if(ram_rtc_register < 0x7){
                ram[(address - 0xA000) + 0x2000*ram_rtc_register] = value;
            }else{
                switch(ram_rtc_register){
                    case 0x8: { rtc_s  = value; }
                    case 0x9: { rtc_m  = value; }
                    case 0xA: { rtc_h  = value; }
                    case 0xB: { rtc_dl  = value; }
                    case 0xC: { rtc_dh  = value; }
                }
            }
            return;
        }
    }
}

CartridgeSnapshot MBC3::createSnapshot(){
    std::vector<bool> bool_map = {ram_enable};
    std::vector<uint8_t> arr_map = {rom_bank_num, ram_rtc_register, rtc_s, rtc_m, rtc_h, rtc_dl, rtc_dh};

    return CartridgeSnapshot(ram, bool_map, arr_map);
}

void MBC3::restoreSnapshot(CartridgeSnapshot& snapshot){
    ram_enable = snapshot.bool_arr[0];

    uint8_t rom_bank_num = snapshot.uint8_arr[0];
    uint8_t ram_rtc_register = snapshot.uint8_arr[1];
    uint8_t rtc_s = snapshot.uint8_arr[2];
    uint8_t rtc_m = snapshot.uint8_arr[3];
    uint8_t rtc_h = snapshot.uint8_arr[4];
    uint8_t rtc_dl = snapshot.uint8_arr[5];
    uint8_t rtc_dh = snapshot.uint8_arr[6];
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

CartridgeSnapshot MBC5::createSnapshot(){
    std::vector<bool> bool_map = {latch_clock_data};
    std::vector<uint8_t> arr_map = {rom_bank_low, rom_bank_high, ram_bank_num};

    return CartridgeSnapshot(ram, bool_map, arr_map);
}

void MBC5::restoreSnapshot(CartridgeSnapshot& snapshot){
    rom_bank_low = snapshot.uint8_arr[0];
    rom_bank_high = snapshot.uint8_arr[1];
    ram_bank_num = snapshot.uint8_arr[2];

    latch_clock_data = snapshot.bool_arr[0];
}
