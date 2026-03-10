#include "GameBoy.h"

GameBoy::GameBoy(){
    bus = new Bus();
    ppu = new PPU(bus);
    cpu = new Cpu();
}

void GameBoy::startGameboy(MBC1* cart){
    bus->connectCartridge(cart);
    cpu->connectBus(bus);

    while(true){
        int cycles = cpu->emulateCycle();
        bus->stepTimer(cycles);
        ppu->step(cycles);
        if(ppu->frameBuffer.drawFLag){
            ppu->frameBuffer.drawFLag = false;
        }
    }
}