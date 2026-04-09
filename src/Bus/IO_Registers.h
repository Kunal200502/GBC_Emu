#pragma once
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <iostream>

class IO_Registers_Snapshot{
    public:
        std::vector<uint8_t> registers;
        IO_Registers_Snapshot(std::vector<uint8_t>);
        IO_Registers_Snapshot(){};

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & registers;
        }
};

class IO_Registers{
    private:
        std::vector<uint8_t> registers;
        static std::unordered_map<uint8_t, uint8_t> initialValues;
        static std::unordered_map<uint8_t, uint8_t> openValues;
    public:
        IO_Registers();
        uint8_t read(uint16_t);
        void write(uint16_t, uint8_t);
        IO_Registers_Snapshot createSnapshot();
        void restoreSnapshot(IO_Registers_Snapshot*);
};