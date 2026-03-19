#pragma once
#include <stdint.h>
#include <vector>
#include <unordered_map>

class IO_Registers{
    private:
        std::vector<uint8_t> registers;
        static std::unordered_map<uint8_t, uint8_t> initialValues;
        static std::unordered_map<uint8_t, uint8_t> openValues;
    public:
        IO_Registers();
        uint8_t read(uint16_t);
        void write(uint16_t, uint8_t);
};