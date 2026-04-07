#pragma once
#include <vector>
#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>


class Cartridge{
    public:
        std::vector<uint8_t> rom;
        std::vector<uint8_t> ram;
        void loadROM(std::string);
        void initializeRAM(uint8_t);

        virtual uint8_t read(uint16_t) const;
        virtual void write(uint16_t, uint8_t);
};

class NoMBC: public Cartridge{
    public:
        NoMBC();
        uint8_t read(uint16_t) const override;
        void write(uint16_t, uint8_t) override;
};

class MBC1: public Cartridge{
    private:
        bool ram_enabled;

        uint8_t rom_bank_low5 = 1;
        uint8_t bank_high2;
        bool is_bank_high_ram;

        uint8_t ram_bank_number;
        bool banking_mode;

    public:
        MBC1(uint8_t = 0);
        uint8_t read(uint16_t) const override;
        void write(uint16_t, uint8_t) override;
};

class MBC3: public Cartridge{
    private:
        uint8_t rom_bank_num = 1;
        bool ram_enable = false;
        uint8_t ram_rtc_selecter = 0;
    public:
        MBC3();
        uint8_t read(uint16_t) const override;
        void write(uint16_t, uint8_t) override;
};

class MBC5: public Cartridge{
    private:
        uint8_t rom_bank_low = 1;
        uint8_t rom_bank_high = 0;
        uint8_t ram_bank_num = 0;

        bool latch_clock_data = false;

        // RTC Registers
        uint8_t RTC_S;
        uint8_t RTC_M;
        uint8_t RTC_H;
        uint8_t RTC_DL;
        uint8_t RTC_DH;
    public:
        MBC5();
        uint8_t read(uint16_t) const override;
        void write(uint16_t, uint8_t) override;
};

// class MBC2: public Cartridge{
//     private:
//         uint8_t rom_bank_number = 1;
//     public:
//         uint8_t read(uint16_t) const;
//         void write(uint16_t, uint8_t);
// };