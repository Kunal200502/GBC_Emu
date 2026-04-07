#include "Bus.h"
#include <iostream>
#include <iomanip>

BusSnapshot::BusSnapshot(std::vector<uint8_t> vram_state, std::vector<uint8_t> wram_state, std::vector<uint8_t> hram_state, 
    IO_Registers* io_registers_state,  std::vector<uint8_t> OAM_memory_state, uint8_t IE_Register_state, uint8_t IF_Register_state, 
    Joypad joypad_state, Timer timer_state, bool start_DMA_transfer_state, uint8_t OAM_DMA_pointer_state){
        vram = vram_state;
        wram = wram_state;
        hram = hram_state;
        io_registers = io_registers_state->createSnapshot();
        OAM_memory = OAM_memory_state;
        IE_Register = IE_Register_state;
        IF_Register = IF_Register_state;
        joypad = joypad_state.create_Joypad_Snapshot();
        timer = timer_state.createSnapshot();
        start_DMA_transfer = start_DMA_transfer_state;
        OAM_DMA_pointer = OAM_DMA_pointer_state;
}

Bus::Bus(){
        vram = std::vector<uint8_t>(0x2000, 0);
        wram = std::vector<uint8_t>(0x2000, 0);
        hram = std::vector<uint8_t>(0x80, 0);
        io_registers = new IO_Registers();
        OAM_memory = std::vector<uint8_t>(0xA0, 0);
        joypad = Joypad();
}

void Bus::request_interrupt(InterruptType interrupt){
    switch(interrupt){
        case V_Blank_Interrupt: { IF_Register |= 0x1; break; }
        case STAT_Interrupt: { IF_Register |= 0x2; break; }
        case Timer_Interrupt: { IF_Register |= 0x4; break; }
        case Serial_Interrupt: { IF_Register |= 0x8; break; }
        case Joypad_Interrupt: { IF_Register |= 0x10; break; }
    }
}

void Bus::connectCartridge(std::string fileString){
    std::ifstream romFile(fileString, std::ios::binary);
    char buffer[0x14F];

    romFile.read(buffer, 0x14F);

    uint8_t Cartridge_type = buffer[0x147];
    uint8_t ram_code = buffer[0x149];
    uint8_t rom_code = buffer[0x148];

    romFile.close();

    if(Cartridge_type == 0x00){
        cartridge =  std::make_unique<NoMBC>();
    }else if(Cartridge_type >= 0x01 && Cartridge_type <= 0x03){
        cartridge = std::make_unique<MBC1>();
    }else if(Cartridge_type >= 0x0F && Cartridge_type <= 0x13){
        cartridge = std::make_unique<MBC5>();
    }else if(Cartridge_type >= 0x19 && Cartridge_type <= 0x1E){
        cartridge = std::make_unique<MBC5>();
    }else{
        std::cout << "Cartridge Not Implemented" << std::endl;
        exit(1);
    }

    cartridge->loadROM(fileString);
    cartridge->initializeRAM(ram_code);
}

uint8_t Bus::read(uint16_t address) const{
    switch(address >> 12){
        // Reading from the Cartridge ROM
        case 0x0: { return cartridge->read(address); }
        case 0x1: { return cartridge->read(address); }
        case 0x2: { return cartridge->read(address); }
        case 0x3: { return cartridge->read(address); }
        case 0x4: { return cartridge->read(address); }
        case 0x5: { return cartridge->read(address); }
        case 0x6: { return cartridge->read(address); }
        case 0x7: { return cartridge->read(address); }

        // Reading from the VRAM
        case 0x8: { return vram[address - 0x8000]; }
        case 0x9: { return vram[address - 0x8000]; }

        // Reading from the Cartridge RAM
        case 0xA: { return cartridge->read(address); }
        case 0xB: { return cartridge->read(address); }

        // Reading from the WRAM
        case 0xC: { return wram[address - 0xC000]; }
        case 0xD: { return wram[address - 0xC000]; }

        // Reading from the Echo RAM
        case 0xE: { return wram[address & 0x1FFF];}
        case 0xF: {
            // Reading from the Echo RAM
            if(address <= 0xFDFF){
                return wram[address & 0x1FFF];
            }
            switch(address & 0xFF00){
                case 0xFE00: {
                    // Reading from the OAM Memory
                    if(address <= 0xFE9F){
                        return OAM_memory[address - 0xFE00];
                    }

                    // Not Usable area
                    return 0xFF;
                }
                case 0xFF00: {
                    // Reading from the IO register
                    if(address <= 0xFF7F){
                        switch(address){
                            // reading from the joypad register
                            case 0xFF00: { return joypad.read(); }
                            
                            // reading from the timer registers
                            case 0xFF04: { return timer.read(address); }
                            case 0xFF05: { return timer.read(address); }
                            case 0xFF06: { return timer.read(address); }
                            case 0xFF07: { return timer.read(address); }

                            // reading from IF register
                            case 0xFF0F: { return IF_Register | 0xE0; }

                            default: {
                                return io_registers->read(address-0xFF00);
                            }
                        }
                    }

                    // reading from the hram
                    if(address <= 0xFFFE){
                        return hram[address-0xFF80];
                    }

                    // reading the IE register 
                    return IE_Register;
                }
            }
        }
    }

    std::cout << "Attempted to read at an unknown location: " << (int)address << std::endl;
    exit(1);
}

uint16_t Bus::read16(uint16_t address) const{
    uint16_t value = read(address);
    value |= (read(address+1) << 8);
    return value;
}

void Bus::write(uint16_t address, uint8_t value){
    switch(address >> 12){
        // writing to the cartridge ROM
        case 0x0: { cartridge->write(address, value); return; }
        case 0x1: { cartridge->write(address, value); return; }
        case 0x2: { cartridge->write(address, value); return; }
        case 0x3: { cartridge->write(address, value); return; }
        case 0x4: { cartridge->write(address, value); return; }
        case 0x5: { cartridge->write(address, value); return; }
        case 0x6: { cartridge->write(address, value); return; }
        case 0x7: { cartridge->write(address, value); return; }

        // Writing to the VRAM
        case 0x8: { vram[address - 0x8000] = value; return; }
        case 0x9: { vram[address - 0x8000] = value; return; }

        // Writing to the cartridge RAM
        case 0xA: { cartridge->write(address, value); return; }
        case 0xB: { cartridge->write(address, value); return; }

        // Writing to the WRAM
        case 0xC: { wram[address - 0xC000] = value; return; }
        case 0xD: { wram[address - 0xC000] = value; return; }

        // Writing to the ECHO RAM
        case 0xE: { wram[address & 0x1FFF] = value; return; }

        case 0xF: {
            // Writing to the ECHO RAM
            if(address <= 0xFDFF){
                wram[address & 0x1FFF] = value;
                return;
            }
            switch(address & 0xFF00){
                case 0xFE00:{
                    // writing to the OAM
                    if(address <= 0xFE9F){
                        OAM_memory[address-0xFE00] = value;
                    }
                    // ignoring the not usable area
                    return;
                }
                case 0xFF00:{
                    // writing to the I/O Registers
                    if(address <= 0xFF7F){
                        switch(address){
                            // writing to the joypad register
                            case 0xFF00: { joypad.write(value); return; }

                            // writing to the timer registers
                            case 0xFF04: { timer.write(address, value); return; }
                            case 0xFF05: { timer.write(address, value); return; }
                            case 0xFF06: { timer.write(address, value); return; }
                            case 0xFF07: { timer.write(address, value); return; }

                            // writing to the IF register
                            case 0xFF0F: { IF_Register = value; return; }

                            default: {
                                if(address == 0xFF46){
                                    start_DMA_transfer = true;
                                }
                                io_registers->write(address-0xFF00, value);
                                return;
                            }
                        }
                    }
                    
                    // writing to the HRAM
                    if(address <= 0xFFFE){
                        hram[address-0xFF80] = value;
                        return;
                    }

                    // writing to the IE Register
                    IE_Register = value;
                    return;
                }
            }
            return;
        }
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
        request_interrupt(Timer_Interrupt);
    }
    if(start_DMA_transfer){
        uint16_t source = read(0xFF46) << 8;
        for(int i = 0; i < steps; i++){
            uint8_t sourceValue = read(source+OAM_DMA_pointer);
            write(0xFE00+OAM_DMA_pointer, sourceValue);
            OAM_DMA_pointer++;
            if(OAM_DMA_pointer > 0x9F){
                OAM_DMA_pointer = 0;
                start_DMA_transfer = false;
            }
        }
    }
}

void Bus::processJoyPadInput(SDL_Event& event){
    if(joypad.processButtonEvent(event)){
        request_interrupt(Joypad_Interrupt);
    }
}

BusSnapshot Bus::createSnapshot(){
    BusSnapshot bus_snapshot(vram, wram, hram, io_registers, OAM_memory, IE_Register, IF_Register, joypad, timer, 
    start_DMA_transfer, OAM_DMA_pointer);
    return bus_snapshot;
}

void Bus::restoreSnapshot(BusSnapshot* snapshot){
    vram = snapshot->vram;
    wram = snapshot->wram;
    hram = snapshot->hram;
    io_registers->restoreSnapshot(&snapshot->io_registers);
    OAM_memory = snapshot->OAM_memory;
    IE_Register = snapshot->IE_Register;
    IF_Register = snapshot->IF_Register;
    joypad.restore_Joypad_Snapshot(&snapshot->joypad);
    timer.restoreSnapshot(&snapshot->timer);
    start_DMA_transfer = snapshot->start_DMA_transfer;
    OAM_DMA_pointer = snapshot->OAM_DMA_pointer;
}