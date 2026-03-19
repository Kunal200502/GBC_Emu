#include "GameBoy.h"

GameBoy::GameBoy(){
    bus = new Bus();
    ppu = new PPU(bus);
    cpu = new Cpu();
}

void GameBoy::startGameboy(MBC1* cart){
    bus->connectCartridge(cart);
    cpu->connectBus(bus);
    SDL_Event event;

    while(true){
        SDL_PollEvent(&event);
        if(event.type == SDL_QUIT){
            return;
        }
        int cycles = cpu->emulateCycle();
        bus->stepTimer(cycles);

        ppu->step(4*cycles);
    }
}