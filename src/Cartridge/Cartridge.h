#pragma once
#include <vector>
#include <stdint.h>
#include <string>


class Cartridge{
    public:
        std::vector<uint8_t> rom;
        std::vector<uint8_t> ram;
        void loadROM(std::string);
        // virtual uint8_t read(uint16_t) const;
        // virtual void write(uint16_t, uint8_t);
};

class MBC1: public Cartridge{
    private:
        bool ram_enabled;

        uint8_t rom_bank_low5 = 1;
        uint8_t bank_high2;

        uint8_t ram_bank_number;
        bool banking_mode;

    public:
        MBC1();
        uint8_t read(uint16_t) const;
        void write(uint16_t, uint8_t);
};

class MBC2: public Cartridge{
    private:
        uint8_t rom_bank_number = 1;
    public:
        uint8_t read(uint16_t) const;
        void write(uint16_t, uint8_t);
};