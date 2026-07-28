#pragma once
#include <vector>
#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>
#include <chrono>

class CartridgeSnapshot{
    public:
        std::vector<uint8_t> ram;
        std::vector<bool> bool_arr;
        std::vector<uint8_t> uint8_arr;

        CartridgeSnapshot(
            std::vector<uint8_t>, 
            std::vector<bool> = {},
            std::vector<uint8_t> = {}
        );
        CartridgeSnapshot(){}

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & ram;
            ar & bool_arr;
            ar * uint8_arr;
        }
};

class Cartridge{
    public:
        std::vector<uint8_t> rom;
        std::vector<uint8_t> ram;
        void loadROM(std::string);
        void initializeRAM(uint8_t);
        
        virtual CartridgeSnapshot createSnapshot(){ return CartridgeSnapshot(); };
        virtual void restoreSnapshot(CartridgeSnapshot&){};

        virtual uint8_t read(uint16_t) const;
        virtual void write(uint16_t, uint8_t);
};

class NoMBC: public Cartridge{
    public:
        NoMBC();
        uint8_t read(uint16_t) const override;
        void write(uint16_t, uint8_t) override;

        CartridgeSnapshot createSnapshot() override;
        void restoreSnapshot(CartridgeSnapshot&) override;
};

class MBC1: public Cartridge{
    private:
        bool ram_enabled;

        uint8_t rom_bank_low5 = 1;
        uint8_t bank_high2;
        bool is_bank_high_ram;

        bool banking_mode;

    public:
        MBC1(uint8_t = 0);
        uint8_t read(uint16_t) const override;
        void write(uint16_t, uint8_t) override;

        CartridgeSnapshot createSnapshot() override;
        void restoreSnapshot(CartridgeSnapshot&) override;
};

class MBC2: public Cartridge{
    private:
        uint8_t rom_bank_num = 1;
        bool ram_enable;
    public:
        MBC2(){}
        uint8_t read(uint16_t) const override;
        void write(uint16_t, uint8_t) override;

        CartridgeSnapshot createSnapshot() override;
        void restoreSnapshot(CartridgeSnapshot&) override;
};

class MBC3: public Cartridge{
    private:
        std::chrono::steady_clock::time_point clock;
        bool ram_enable;
        uint8_t rom_bank_num = 1;
        uint8_t ram_rtc_register = 0;
        uint8_t rtc_s;
        uint8_t rtc_m;
        uint8_t rtc_h;
        uint8_t rtc_dl;
        uint8_t rtc_dh;
    public:
        MBC3(){ clock = std::chrono::steady_clock::now(); }
        uint8_t read(uint16_t) const override;
        void write(uint16_t, uint8_t) override;

        CartridgeSnapshot createSnapshot() override;
        void restoreSnapshot(CartridgeSnapshot&) override;

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

        CartridgeSnapshot createSnapshot() override;
        void restoreSnapshot(CartridgeSnapshot&) override;
};

// class MBC2: public Cartridge{
//     private:
//         uint8_t rom_bank_number = 1;
//     public:
//         uint8_t read(uint16_t) const;
//         void write(uint16_t, uint8_t);
// };