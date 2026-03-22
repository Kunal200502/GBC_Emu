#include "GameBoy.h"

GameBoy::GameBoy(){
    bus = new Bus();
    ppu = new PPU(bus);
    cpu = new Cpu();
}

void GameBoy::startGameboy(std::string fileString){
    bus->connectCartridge(fileString);
    cpu->connectBus(bus);
    SDL_Event event;

    while(true){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                return;
            }
            bus->processJoyPadInput(event);
        }
        int cycles = cpu->emulateCycle();
        bus->stepTimer(cycles);

        ppu->step(4*cycles);
    }
}