#include <fstream>
#include <string>
#include <iostream>
#include <thread>

#include "Cartridge/Cartridge.h"
#include "Cpu/Cpu.h"
#include "Bus/Bus.h"
#include "GameBoy/GameBoy.h"
#include <SDL2/SDL.h>

int main(int argc, char** argv){
    std::string romFile = argv[1];

    MBC1 cart;
    cart.loadROM(romFile);
    // cart.loadROM("../ROM/SuperMarioLand.gb");

    // int temp = cart.read(0x0100);

    // Bus bus;
    // bus.connectCartridge(&cart);

    // Cpu cpu;
    // cpu.connectBus(&bus);

    // while(true){
    //     cpu.emulateCycle();
    // }

    GameBoy gameBoy = GameBoy();
    gameBoy.startGameboy(&cart);
}