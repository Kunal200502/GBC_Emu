#include "SaveState.h"

SaveState::SaveState(Bus* bus, Cpu* cpu, PPU* ppu){
    busSnapshot = bus->createSnapshot();
    cpuSnapshot = cpu->create_CPU_snapshot();
    ppuSnapshot = ppu->createSnapshot();
}

void createSaveState(SaveState& saveState){
    std::ofstream file("save.sav", std::ios::binary);
    boost::archive::binary_oarchive oa(file);
    oa << saveState;
}

void loadSaveState(SaveState& saveState){
    std::ifstream file("save.sav", std::ios::binary);
    boost::archive::binary_iarchive ia(file);
    ia >> saveState;
}