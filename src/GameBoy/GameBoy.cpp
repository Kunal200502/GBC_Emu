#include "GameBoy.h"

GameBoy::GameBoy(){
    bus = new Bus();
    ppu = new PPU(bus);
    cpu = new Cpu();
}

void GameBoy::restoreSaveState(SaveState& saveState){
    bus->restoreSnapshot(&saveState.busSnapshot);
    cpu->restore_CPU_snapshot(&saveState.cpuSnapshot);
    ppu->restoreSnapshot(&saveState.ppuSnapshot);
}

void GameBoy::startGameboy(std::string fileString){
    bus->connectCartridge(fileString);
    cpu->connectBus(bus);
    SDL_Event event;
    cartString = fileString;

    while(true){
        if(counter > 10){
            counter = 0;
            while(SDL_PollEvent(&event)){
                if(event.type == SDL_QUIT){
                    return;
                }else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F1){
                    SaveState saveState(bus, cpu, ppu);
                    createSaveState(saveState);
                }else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F2){
                    SaveState saveState = SaveState();

                    loadSaveState(saveState);
                    restoreSaveState(saveState);
                }
                bus->processJoyPadInput(event);
            }
        }
        int cycles = cpu->emulateCycle();
        counter += cycles;
        bus->stepTimer(cycles);

        ppu->step(4*cycles);
    }
}