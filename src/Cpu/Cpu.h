#pragma once
#include <stdint.h>
#include "Opcode_info.h"
#include <iomanip>

#include "../Bus/Bus.h"

class Cpu{
    uint8_t i = 0;
    // constants
    const uint32_t CLOCK_FREQUENCY = 4194304; 
    const uint32_t DIV_REGISTER_FREQUENCY = 16384;

    // registers
    uint8_t A = 0x11;
    uint8_t B = 0x00;
    uint8_t C = 0x00;
    uint8_t D = 0xFF;
    uint8_t E = 0x56;
    uint8_t H = 0x00;
    uint8_t L = 0x0D;
    uint16_t SP = 0xFFFE; // stack pointer
    uint16_t PC = 0x0100; // program counter
    uint8_t F = 0x80; // flags

    bool IME = false;
    bool EI_pending = false;

    uint32_t clock = 0;

    Bus* bus;

    // flag helper methods
    bool getZ();
    void setZ(bool);

    bool getN();
    void setN(bool);

    bool getH();
    void setH(bool);

    bool getC();
    void setC(bool);

    void stackPush16(uint16_t);
    void stackPush16(uint8_t high, uint8_t low);
    uint8_t stackPop8();
    uint16_t stackPop16();

    uint16_t getHL();
    uint16_t getBC();
    uint16_t getDE();

    void addA(uint8_t);
    void adcA(uint8_t);
    void subA(uint8_t);
    void sbcA(uint8_t);

    void andA(uint8_t);
    void orA(uint8_t);
    void xorA(uint8_t);
    void cpA(uint8_t);

    void inc8(uint8_t&);
    void inc16(uint8_t&, uint8_t&);
    void dec8(uint8_t&);
    void dec16(uint8_t&, uint8_t&);

    void addHL(uint16_t);
    void addSP(int8_t);

    void rlca();
    void rla();
    void rrca();
    void rra();

    void rlc(uint8_t&);
    void rl(uint8_t&);
    void rrc(uint8_t&);
    void rr(uint8_t&);

    void sla(uint8_t&);
    void sra(uint8_t&);
    void srl(uint8_t&);

    void res(uint8_t&, uint8_t);
    void set(uint8_t&, uint8_t);

    void swap(uint8_t&);

    void jp(bool);
    void jr(bool);
    void call(bool);

    void copyBitComplement(uint8_t, uint8_t);

    void daa();

    void serviceInterrupt(uint8_t);
    void clearIFBit(uint8_t);

    public:
        void connectBus(Bus* bus);
        uint8_t emulateCycle();
};