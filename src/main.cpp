#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <thread>

#include "Cartridge/Cartridge.h"
#include "Cpu/Cpu.h"
#include "Bus/Bus.h"
#include "GameBoy/GameBoy.h"
#include <SDL2/SDL.h>

int main(int argc, char** argv){
    std::string romFile = argv[1];

    MBC1 cart = MBC1();
    cart.loadROM(romFile);

    GameBoy gameBoy = GameBoy();
    gameBoy.startGameboy(&cart);
}