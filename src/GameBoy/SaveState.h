#pragma once
#include "../Bus/Bus.h"
#include "../Cpu/Cpu.h"
#include "../PPU/PPU.h"
#include <fstream>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/priority_queue.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/utility.hpp>

class SaveState{
    public:
        BusSnapshot busSnapshot;
        CpuSnapshot cpuSnapshot;
        PPUSnapshot ppuSnapshot;
        SaveState(Bus*, Cpu*, PPU*);
        SaveState(){}
         
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & cpuSnapshot;
            ar & ppuSnapshot;
            ar & busSnapshot;
        }
};

void createSaveState(SaveState&);
void loadSaveState(SaveState&);