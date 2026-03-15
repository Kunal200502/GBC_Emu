#include "Cpu.h"
#include "iostream"

// flag helper methods;
// zero flag (Z)
bool Cpu::getZ(){
    return (F & 0x80) != 0;
}
void Cpu::setZ(bool z){
    if(z){
        F |= 0x80;
    }else{
        F &= 0x7F;
    }
}

// Subtraction Flag (N)
bool Cpu::getN(){
    return (F & 0x40) != 0;
}
void Cpu::setN(bool n){
    if(n){
        F |= 0x40;
    }else{
        F &= 0xBF;
    }
}

// Half Carry FLag (H)
bool Cpu::getH(){
    return (F & 0x20) != 0;
}
void Cpu::setH(bool n){
    if(n){
        F |= 0x20;
    }else{
        F &= 0xDF;
    }
}

// carry flag (C)
bool Cpu::getC(){
    return (F & 0x10) != 0;
}
void Cpu::setC(bool n){
    if(n){
        F |= 0x10;
    }else{
        F &= 0xEF;
    }
}

void Cpu::stackPush16(uint16_t value){
    bus->write(--SP, value >> 8);
    bus->write(--SP, value & 0xFF);
}
void Cpu::stackPush16(uint8_t high, uint8_t low){
    bus->write(--SP, high);
    bus->write(--SP, low);
}

uint8_t Cpu::stackPop8(){
    return bus->read(SP++);
}
uint16_t Cpu::stackPop16(){
    uint16_t value = bus->read(SP++) | (bus->read(SP++) << 8);
    return value;
}

uint16_t Cpu::getHL(){
    return (H << 8) | L;
}
uint16_t Cpu::getBC(){
    return (B << 8) | C;
}
uint16_t Cpu::getDE(){
    return (D << 8) | E;
}

void Cpu::addA(uint8_t value){
    uint16_t result = value+A;
    setH(((A & 0xF) + (value & 0xF)) > 0xF);
    setC(result > 0xFF);
    setN(false);
    A = (result & 0xFF);
    setZ(A == 0);
}
void Cpu::adcA(uint8_t value){
    int carry = getC();
    uint16_t result = value+A+carry;
    setH(((A & 0xF) + (value & 0xF) + carry) > 0xF);
    setC(result > 0xFF);
    setN(false);
    A = (result & 0xFF);
    setZ(A == 0);
}

void Cpu::subA(uint8_t value){
    uint8_t result = A - value;
    setZ(result == 0);
    setH((A & 0xF) < (value & 0xF));
    setC(A < value);
    setN(true);
    A = result;
}

void Cpu::sbcA(uint8_t value){
    int carry = getC();
    uint8_t result = A - value - carry;
    setZ(result == 0);
    setH((A & 0xF) < ((value & 0xF) + carry));
    setC(A < (uint16_t)value+carry);
    setN(true);
    A = result;
}

void Cpu::andA(uint8_t value){
    A &= value;
    setC(false);
    setH(true);
    setN(false);
    setZ(A == 0);
}

void Cpu::orA(uint8_t value){
    A |= value;
    setH(false);
    setC(false);
    setN(false);
    setZ(A == 0);
}

void Cpu::xorA(uint8_t value){
    A ^= value;
    setC(false);
    setH(false);
    setN(0); 
    setZ(A == 0);
}

void Cpu::cpA(uint8_t value){
    uint8_t temp = value;
    setC(A < value);
    setH((A & 0xF) < (value & 0xF));
    setN(true);
    setZ(A == value);
}

void Cpu::inc8(uint8_t &value){
    setH(((value & 0xF) + 1) > 0xF);
    setN(false);
    value++;
    setZ(value == 0);
}

void Cpu::inc16(uint8_t& high, uint8_t& low){
    if(low == 0xFF) high++;
    low++;
}

void Cpu::dec8(uint8_t &value){
    setH((value & 0xF) == 0x0);
    setN(true);
    value--;
    setZ(value == 0);
}

void Cpu::dec16(uint8_t& high, uint8_t& low){
    if(low == 0) high--;
    low--;
}

void Cpu::addHL(uint16_t value){
    uint16_t hl = getHL();
    uint32_t result = hl + value;
    setH(((hl & 0xFFF) + (value & 0xFFF)) > 0xFFF);
    setC(result > 0xFFFF);
    setN(false);
    L = result & 0xFF;
    H = (result >> 8) & 0xFF;
}
void Cpu::addSP(int8_t value){
    uint32_t result = SP + value;
    setH(((SP & 0xF) + (value & 0xF)) > 0xF);
    setC(((SP & 0xFF) + (value & 0xFF)) > 0xFF);
    setN(false);
    setZ(false);
    SP = result & 0xFFFF;
}

void Cpu::rlca(){
    uint8_t MSB = (A >> 7);
    A = (A << 1) | MSB;
    setC(MSB);
    setH(false);
    setN(false);
    setZ(false);
}

void Cpu::rla(){
    uint8_t MSB = (A >> 7); 
    A = (A << 1) | getC(); 
    setC(MSB);
    setH(false);
    setN(false);
    setZ(false);
}

void Cpu::rrca(){
    uint8_t LSB = A & 0x1;
    A = (LSB << 7) | (A >> 1);
    setC(LSB);
    setH(false);
    setN(false);
    setZ(false);
}

void Cpu::rra(){
    uint8_t LSB = A & 0x1;
    uint8_t C_flag = getC();
    A = (C_flag << 7) | (A >> 1);
    setC(LSB);
    setH(false);
    setN(false);
    setZ(false);
}

void Cpu::rlc(uint8_t& value){
    uint8_t MSB = (value >> 7); 
    value = (value << 1) | MSB; 
    setC(MSB);
    setH(false);
    setN(false);
    setZ(value == 0);
}

void Cpu::rl(uint8_t& value){
    uint8_t MSB = (value >> 7); 
    value = (value << 1) | getC(); 
    setC(MSB);
    setH(false);
    setN(false);
    setZ(value == 0);
}

void Cpu::rrc(uint8_t& value){
    uint8_t LSB = value & 0x1;
    value = (LSB << 7) | (value >> 1);
    setC(LSB);
    setH(false);
    setN(false);
    setZ(value == 0);
}

void Cpu::rr(uint8_t& value){
    uint8_t LSB = value & 0x1;
    uint8_t C_flag = getC();
    value = (C_flag << 7) | (value >> 1);
    setC(LSB);
    setH(false);
    setN(false);
    setZ(value == 0);
}

void Cpu::sla(uint8_t& value){
    uint8_t MSB = (value >> 7);
    value <<= 1;
    setC(MSB);
    setH(false);
    setN(false);
    setZ(value == 0);
}

void Cpu::sra(uint8_t& value){
    uint8_t LSB = (value & 0x1);
    value = (value >> 1) | (value & 0x80);
    setC(LSB);
    setH(false);
    setN(false);
    setZ(value == 0);
}

void Cpu::srl(uint8_t& value){
    uint8_t LSB = (value & 0x1);
    value >>= 1;
    setC(LSB);
    setH(false);
    setN(false);
    setZ(value == 0);
}

void Cpu::swap(uint8_t& value){
    value = (value >> 4) | (value << 4);
    setC(false);
    setH(false);
    setN(false);
    setZ(value == 0);
}

void Cpu::copyBitComplement(uint8_t value, uint8_t bit){
    bool flag = (value >> bit) & 0x1;
    setZ(!flag);
    setH(true);
    setN(false);
}

void Cpu::daa(){
    uint8_t offset = 0;
    if(!getN()){
        if(((A & 0xF) > 0x09) || getH()){
            offset |= 0x06;
        }
        if(A > 0x99 || getC()){
            offset |= 0x60;
            setC(true);
        }
        A += offset;
    }else{
        if(getH()){
            offset |= 0x06;
        }
        if(getC()){
            offset |= 0x60;
        }
        A -= offset;
    }
    setH(false);
    setZ(A == 0);
}

void Cpu::res(uint8_t& value, uint8_t bit){
    value = value & ~(1 << bit);
}

void Cpu::set(uint8_t& value, uint8_t bit){
    value = value | (1 << bit);
}

void Cpu::jp(bool condition){
    uint16_t addr = bus->read16(PC);
    PC += 2;
    if(condition){
        PC = addr;
    }
}

void Cpu::jr(bool condition){
    int8_t value = bus->read(PC++);
    if(condition){
        PC += value;
    }
}

void Cpu::call(bool condition){
    uint16_t addr = bus->read16(PC);
    PC += 2;
    if(condition){
        stackPush16(PC);
        PC = addr;
    }
}

void Cpu::connectBus(Bus* bus){
    this->bus = bus;
}

void Cpu::clearIFBit(uint8_t bit){
    uint8_t IF = bus->read(0xFF0F);
    res(IF, bit);
    bus->write(0xFF0F, IF);
}

void Cpu::serviceInterrupt(uint8_t IF){
    std::vector<uint8_t> interruptVector = {0x40, 0x48, 0x50, 0x58, 0x60};
    for(int i = 0; i<5; i++){
        if(IF & (1 << i)){
            IME = false;
            stackPush16(PC);
            PC = interruptVector[i];
            clearIFBit(i);
            clock += 5;
            break;
        }
    }
}

uint8_t Cpu::emulateCycle(){
    uint8_t IE = bus->read(0xFFFF);
    uint8_t IF = bus->read(0xFF0F);
    if(halted){
        if(IF && IE){
            halted = false;
            if(IME){
                serviceInterrupt(IF);
            }
        }else{
            return 0;
        }
    }
    if(EI_pending){
        IME = EI_pending;
        EI_pending = false;
    }
    uint8_t opcode = bus->read(PC++);

    bool branch = true;
    uint8_t opcode_cycles = 0;

    switch(opcode){
        case 0x00: { break; }
        case 0xCC: { branch = getZ(); call(branch); break; } // CALL Z
        case 0xD4: { branch = !getC(); call(branch); break; } // CALL NC
        case 0xDC: { branch = getC(); call(branch); break; } // CALL C

        case 0x7F: { break;} // LD A, A
        case 0x78: {A = B; break;} // LD A, B
        case 0x79: {A = C; break;} // LD A, C
        case 0x7A: {A = D; break;} // LD A, D
        case 0x7B: {A = E; break;} // LD A, E
        case 0x7C: {A = H; break;} // LD A, H
        case 0x7D: {A = L; break;} // LD A, L

        case 0x47: {B = A; break;} // LD B, A
        case 0x40: {break;} // LD B, B
        case 0x41: {B = C; break;} // LD B, C
        case 0x42: {B = D; break;} // LD B, D
        case 0x43: {B = E; break;} // LD B, E
        case 0x44: {B = H; break;} // LD B, H
        case 0x45: {B = L; break;} // LD B, L

        case 0x4F: {C = A; break;} // LD C, A
        case 0x48: {C = B; break;} // LD C, B
        case 0x49: {break;} // LD C, C
        case 0x4A: {C = D; break;} // LD C, D
        case 0x4B: {C = E; break;} // LD C, E
        case 0x4C: {C = H; break;} // LD C, H
        case 0x4D: {C = L; break;} // LD C, L

        case 0x57: {D = A; break;} // LD D, A
        case 0x50: {D = B; break;} // LD D, B
        case 0x51: {D = C; break;} // LD D, C
        case 0x52: {break;} // LD D, D
        case 0x53: {D = E; break;} // LD D, E
        case 0x54: {D = H; break;} // LD D, H
        case 0x55: {D = L; break;} // LD D, L

        case 0x5F: {E = A; break;} // LD E, A
        case 0x58: {E = B; break;} // LD E, B
        case 0x59: {E = C; break;} // LD E, C
        case 0x5A: {E = D; break;} // LD E, D
        case 0x5B: {break;} // LD E, E
        case 0x5C: {E = H; break;} // LD E, H
        case 0x5D: {E = L; break;} // LD E, L

        case 0x67: {H = A; break;} // LD H, A
        case 0x60: {H = B; break;} // LD H, B
        case 0x61: {H = C; break;} // LD H, C
        case 0x62: {H = D; break;} // LD H, D
        case 0x63: {H = E; break;} // LD H, E
        case 0x64: {break;} // LD H, H
        case 0x65: {H = L; break;} // LD H, L

        case 0x6F: {L = A; break;} // LD L, A
        case 0x68: {L = B; break;} // LD L, B
        case 0x69: {L = C; break;} // LD L, C
        case 0x6A: {L = D; break;} // LD L, D
        case 0x6B: {L = E; break;} // LD L, E
        case 0x6C: {L = H; break;} // LD L, H
        case 0x6D: {break;} // LD L, L

        case 0x3E: {A = bus->read(PC++); break;} // LD A, n
        case 0x06: {B = bus->read(PC++); break;} // LD B, n
        case 0x0E: {C = bus->read(PC++); break;} // LD C, n
        case 0x16: {D = bus->read(PC++); break;} // LD D, n
        case 0x1E: {E = bus->read(PC++); break;} // LD E, n
        case 0x26: {H = bus->read(PC++); break;} // LD H, n
        case 0x2E: {L = bus->read(PC++); break;} // LD L, n

        case 0x7E: {uint16_t HL_address = getHL(); A = bus->read(HL_address); break;} // LD A, (HL)
        case 0x46: {uint16_t HL_address = getHL(); B = bus->read(HL_address); break;} // LD B, (HL)
        case 0x4E: {uint16_t HL_address = getHL(); C = bus->read(HL_address); break;} // LD C, (HL)
        case 0x56: {uint16_t HL_address = getHL(); D = bus->read(HL_address); break;} // LD D, (HL)
        case 0x5E: {uint16_t HL_address = getHL(); E = bus->read(HL_address); break;} // LD E, (HL)
        case 0x66: {uint16_t HL_address = getHL(); H = bus->read(HL_address); break;} // LD H, (HL)
        case 0x6E: {uint16_t HL_address = getHL(); L = bus->read(HL_address); break;} // LD L, (HL)
        
        case 0x77: {uint16_t HL_address = getHL(); bus->write(HL_address, A); break;} // LD (HL), A
        case 0x70: {uint16_t HL_address = getHL(); bus->write(HL_address, B); break;} // LD (HL), B
        case 0x71: {uint16_t HL_address = getHL(); bus->write(HL_address, C); break;} // LD (HL), C
        case 0x72: {uint16_t HL_address = getHL(); bus->write(HL_address, D); break;} // LD (HL), D
        case 0x73: {uint16_t HL_address = getHL(); bus->write(HL_address, E); break;} // LD (HL), E
        case 0x74: {uint16_t HL_address = getHL(); bus->write(HL_address, H); break;} // LD (HL), H
        case 0x75: {uint16_t HL_address = getHL(); bus->write(HL_address, L); break;} // LD (HL), L

        case 0x36: {uint16_t HL_address = getHL(); uint8_t value = bus->read(PC++); bus->write(HL_address, value); break;} // LD (HL), d8
        case 0x0A: {uint16_t BC_address = (B << 8) | C; A = bus->read(BC_address); break;} // LD A, (BC)
        case 0x1A: {uint16_t DE_address = (D << 8) | E; A = bus->read(DE_address); break;} // LD A, (DE)
        case 0xF2: {A = bus->read(0xFF00 + C); break;} // LD A, (C)
        case 0xE2: {bus->write(0xFF00+C, A); break;} // LD (C), A
        case 0xF0: {
            uint16_t address = 0xFF00+bus->read(PC++); 
            A = bus->read(address); 
            break;} // LD A, (d8)
        case 0xE0: {uint8_t d8 = bus->read(PC++); bus->write(0xFF00+d8, A); break;} // LD (d8), A
        case 0xFA: {uint16_t address = bus->read(PC++) | (bus->read(PC++) << 8); A = bus->read(address); break;} // LD A, (d16);
        case 0xEA: {uint16_t address = bus->read(PC++) | (bus->read(PC++) << 8); bus->write(address, A); break;} // LD (a16), A
        case 0x2A: {uint16_t address = getHL(); A = bus->read(address); L++; if(L == 0){H++;} break;} // LD A, (HL+)
        case 0x3A: {uint16_t address = getHL(); A = bus->read(address); address--; H = ((address >> 8) & 0xFF); L = (address & 0xFF); break;} // LD A, (HL-)
        case 0x02: {uint16_t address = (B << 8) | C; bus->write(address, A); break;} // LD (BC), A
        case 0x12: {uint16_t address = (D << 8) | E; bus->write(address, A); break;} // LD (DE), A
        case 0x22: {uint16_t address = getHL(); bus->write(address, A); address++; H = (address >> 8) & 0xFF; L = address & 0xFF; break;} // LD (HL+), A
        case 0x32: {uint16_t address = getHL(); bus->write(address, A); address--; H = (address >> 8) & 0xFF; L = address & 0xFF; break;} // LD (HL-), A
        case 0x01: {C = bus->read(PC++); B = bus->read(PC++); break;} // LD BC, d16
        case 0x11: {E = bus->read(PC++); D = bus->read(PC++); break;} // LD DE, d16
        case 0x21: {L = bus->read(PC++); H = bus->read(PC++); break;} // LD HL, d16
        case 0x31: {SP = (bus->read(PC++) | (bus->read(PC++) << 8)); break;} // LD SP, d16
        case 0xF9: {SP = (H << 8) | L; break;} // LD SP, HL

        case 0xC5: {stackPush16(B, C); break;}// PUSH BC
        case 0xD5: {stackPush16(D, E); break;} // PUSH DE
        case 0xE5: {stackPush16(H, L); break;} // PUSH HL
        case 0xF5: {stackPush16(A, F); break;} // PUSH AF
        case 0xC1: {C = stackPop8(); B = stackPop8(); break;} // POP BC
        case 0xD1: {E = stackPop8(); D = stackPop8(); break;} // POP DE
        case 0xE1: {L = stackPop8(); H = stackPop8(); break;} // POP HL
        case 0xF1: {F = stackPop8() & 0xF0; A = stackPop8(); break;} // POP AF  

        case 0xF8: {
            int8_t offset = bus->read(PC++);
            uint8_t d8 = offset;
            uint16_t result =  SP + offset;
            H = (result >> 8) & 0xFF;
            L = (result & 0xFF);
            setC(((SP & 0xFF) + d8) > 0xFF);
            setH(((SP & 0xF) + (d8 & 0xF)) > 0xF);
            setN(0);
            setZ(0);
            break;
        } // LD HL, SP+s8
        case 0x08: { uint16_t address = bus->read16(PC); PC += 2; bus->write16(address, SP); break;} // LD (a16), SP

        case 0x87: { addA(A); break; } // ADD A, A
        case 0x80: { addA(B); break; } // ADD A, B
        case 0x81: { addA(C); break; } // ADD A, C
        case 0x82: { addA(D); break; } // ADD A, D
        case 0x83: { addA(E); break; } // ADD A, E
        case 0x84: { addA(H); break; } // ADD A, H
        case 0x85: { addA(L); break; } // ADD A, L
        case 0xC6: { addA(bus->read(PC++)); break; } // ADD A, d8
        case 0x86: { uint16_t HL_address = (H << 8) | L; addA(bus->read(HL_address)); break; } // ADD A, (HL) 
        
        case 0x8F: { adcA(A); break; } // ADC A, A
        case 0x88: { adcA(B); break; } // ADC A, B
        case 0x89: { adcA(C); break; } // ADC A, C
        case 0x8A: { adcA(D); break; } // ADC A, D
        case 0x8B: { adcA(E); break; } // ADC A, E
        case 0x8C: { adcA(H); break; } // ADC A, H
        case 0x8D: { adcA(L); break; } // ADC A, L
        case 0xCE: { adcA(bus->read(PC++)); break; } // ADC A, d8
        case 0x8E: { uint16_t HL_address = getHL(); adcA(bus->read(HL_address)); break; } // ADC A, (HL)

        case 0x97: { subA(A); break; }; // SUB A 
        case 0x90: { subA(B); break; }; // SUB B
        case 0x91: { subA(C); break; }; // SUB C
        case 0x92: { subA(D); break; }; // SUB D
        case 0x93: { subA(E); break; }; // SUB E
        case 0x94: { subA(H); break; }; // SUB H
        case 0x95: { subA(L); break; }; // SUB L   
        case 0xD6: { subA(bus->read(PC++)); break; } // SUB d8
        case 0x96: { uint16_t HL_address = getHL(); subA(bus->read(HL_address)); break; } // SUB (HL)   

        case 0x9F: { sbcA(A); break; } // SBC A, A
        case 0x98: { sbcA(B); break; } // SBC A, B
        case 0x99: { sbcA(C); break; } // SBC A, C
        case 0x9A: { sbcA(D); break; } // SBC A, D
        case 0x9B: { sbcA(E); break; } // SBC A, E
        case 0x9C: { sbcA(H); break; } // SBC A, H
        case 0x9D: { sbcA(L); break; } // SBC A, L
        case 0xDE: { sbcA(bus->read(PC++)); break; } // SBC A, (d8)
        case 0x9E: { uint16_t HL_address = getHL(); sbcA(bus->read(HL_address)); break; } // SBC A, (HL)

        case 0xA7: { andA(A); break; } // AND A
        case 0xA0: { andA(B); break; } // AND B
        case 0xA1: { andA(C); break; } // AND C
        case 0xA2: { andA(D); break; } // AND D
        case 0xA3: { andA(E); break; } // AND E
        case 0xA4: { andA(H); break; } // AND H
        case 0xA5: { andA(L); break; } // AND L
        case 0xE6: { andA(bus->read(PC++)); break; } // AND d8
        case 0xA6: { uint16_t HL_address = getHL(); andA(bus->read(HL_address)); break; } // AND (HL)

        case 0xB7: { orA(A); break; } // OR A
        case 0xB0: { orA(B); break; } // OR B
        case 0xB1: { orA(C); break; } // OR C
        case 0xB2: { orA(D); break; } // OR D
        case 0xB3: { orA(E); break; } // OR E
        case 0xB4: { orA(H); break; } // OR H
        case 0xB5: { orA(L); break; } // OR L
        case 0xF6: { orA(bus->read(PC++)); break; } // OR d8
        case 0xB6: { uint16_t HL_address = getHL(); orA(bus->read(HL_address)); break; } // OR (HL)

        case 0xAF: { xorA(A); break; } // XOR A
        case 0xA8: { xorA(B); break; } // XOR B
        case 0xA9: { xorA(C); break; } // XOR C
        case 0xAA: { xorA(D); break; } // XOR D
        case 0xAB: { xorA(E); break; } // XOR E
        case 0xAC: { xorA(H); break; } // XOR H
        case 0xAD: { xorA(L); break; } // XOR L
        case 0xEE: { xorA(bus->read(PC++)); break; } // XOR d8
        case 0xAE: { uint16_t HL_address = getHL(); xorA(bus->read(HL_address)); break; } // XOR (HL)

        case 0xBF: { cpA(A); break; } // CP A
        case 0xB8: { cpA(B); break; } // CP B
        case 0xB9: { cpA(C); break; } // CP C
        case 0xBA: { cpA(D); break; } // CP D
        case 0xBB: { cpA(E); break; } // CP E
        case 0xBC: { cpA(H); break; } // CP H
        case 0xBD: { cpA(L); break; } // CP L
        case 0xFE: { cpA(bus->read(PC++)); break; } // CP d8
        case 0xBE: { uint16_t HL_address = getHL(); cpA(bus->read(HL_address)); break; } // CP (HL)

        case 0x3C: { inc8(A); break; } // INC A
        case 0x04: { inc8(B); break; } // INC B
        case 0x0C: { inc8(C); break; } // INC C
        case 0x14: { inc8(D); break; } // INC D
        case 0x1C: { inc8(E); break; } // INC E
        case 0x24: { inc8(H); break; } // INC H
        case 0x2C: { inc8(L); break; } // INC L
        case 0x34: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); inc8(value); bus->write(HL_address, value); break; } // INC (HL)
            
        case 0x3D: { dec8(A); break; } // DEC A
        case 0x05: { dec8(B); break; } // DEC B
        case 0x0D: { dec8(C); break; } // DEC C
        case 0x15: { dec8(D); break; } // DEC D
        case 0x1D: { dec8(E); break; } // DEC E
        case 0x25: { dec8(H); break; } // DEC H
        case 0x2D: { dec8(L); break; } // DEC L
        case 0x35: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); dec8(value); bus->write(HL_address, value); break; } // DEC (HL)

        case 0x09: { addHL(getBC()); break; } // ADD HL, BC
        case 0x19: { addHL(getDE()); break; } // ADD HL, DE
        case 0x29: { addHL(getHL()); break; } // ADD HL, HL
        case 0x39: { addHL(SP); break; } // ADD HL, SP
        case 0xE8: { addSP(bus->read(PC++)); break; } // ADD SP, (s8)

        case 0x03: { inc16(B, C); break; } // INC BC
        case 0x13: { inc16(D, E); break; } // INC DE
        case 0x23: { inc16(H, L); break; } // INC HL
        case 0x33: { SP++; break; } // INC SP

        case 0x0B: { dec16(B, C); break; } // DEC BC
        case 0x1B: { dec16(D, E); break; } // DEC DE
        case 0x2B: { dec16(H, L); break; } // DEC HL
        case 0x3B: { SP--; break; } // DEC SP

        case 0x07: { rlca(); break; } // RLCA
        case 0x17: { rla(); break; } // RLA
        case 0x0F: { rrca(); break; } // RRCA
        case 0x1F: { rra(); break; } // RRA

        case 0xCB: {
            uint8_t CB_opcode = bus->read(PC++);
            opcode_cycles += getCBOpcodeCycle(CB_opcode);
            switch(CB_opcode){
                case 0x07: { rlc(A); break; } // RLC A
                case 0x00: { rlc(B); break; } // RLC B
                case 0x01: { rlc(C); break; } // RLC C
                case 0x02: { rlc(D); break; } // RLC D
                case 0x03: { rlc(E); break; } // RLC E
                case 0x04: { rlc(H); break; } // RLC H
                case 0x05: { rlc(L); break; } // RLC L
                case 0x06: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); rlc(value); bus->write(HL_address, value); break; } // RLC (HL)
                
                case 0x17: { rl(A); break; } // RL A
                case 0x10: { rl(B); break; } // RL B
                case 0x11: { rl(C); break; } // RL C
                case 0x12: { rl(D); break; } // RL D
                case 0x13: { rl(E); break; } // RL E
                case 0x14: { rl(H); break; } // RL H
                case 0x15: { rl(L); break; } // RL L
                case 0x16: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); rl(value); bus->write(HL_address, value); break; } // RL (HL)

                case 0x0F: { rrc(A); break; } // RRC A
                case 0x08: { rrc(B); break; } // RRC B
                case 0x09: { rrc(C); break; } // RRC C
                case 0x0A: { rrc(D); break; } // RRC D
                case 0x0B: { rrc(E); break; } // RRC E
                case 0x0C: { rrc(H); break; } // RRC H
                case 0x0D: { rrc(L); break; } // RRC L
                case 0x0E: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); rrc(value); bus->write(HL_address, value); break; } // RRC (HL)

                case 0x1F: { rr(A); break; } // RR A
                case 0x18: { rr(B); break; } // RR B
                case 0x19: { rr(C); break; } // RR C
                case 0x1A: { rr(D); break; } // RR D
                case 0x1B: { rr(E); break; } // RR E
                case 0x1C: { rr(H); break; } // RR H
                case 0x1D: { rr(L); break; } // RR L
                case 0x1E: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); rr(value); bus->write(HL_address, value); break; } // RR (HL)

                case 0x27: { sla(A); break; } // SLA A
                case 0x20: { sla(B); break; } // SLA B
                case 0x21: { sla(C); break; } // SLA C
                case 0x22: { sla(D); break; } // SLA D
                case 0x23: { sla(E); break; } // SLA E
                case 0x24: { sla(H); break; } // SLA H
                case 0x25: { sla(L); break; } // SLA L
                case 0x26: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); sla(value); bus->write(HL_address, value); break; } // SLA (HL)

                case 0x2F: { sra(A); break; } // SRA A
                case 0x28: { sra(B); break; } // SRA B
                case 0x29: { sra(C); break; } // SRA C
                case 0x2A: { sra(D); break; } // SRA D
                case 0x2B: { sra(E); break; } // SRA E
                case 0x2C: { sra(H); break; } // SRA H
                case 0x2D: { sra(L); break; } // SRA L
                case 0x2E: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); sra(value); bus->write(HL_address, value); break; } // SRA (HL)

                case 0x3F: { srl(A); break; } // SRL A
                case 0x38: { srl(B); break; } // SRL B
                case 0x39: { srl(C); break; } // SRL C
                case 0x3A: { srl(D); break; } // SRL D
                case 0x3B: { srl(E); break; } // SRL E
                case 0x3C: { srl(H); break; } // SRL H
                case 0x3D: { srl(L); break; } // SRL L
                case 0x3E: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); srl(value); bus->write(HL_address, value); break; } // SRL (HL)

                case 0x37: { swap(A); break; } // SWAP A
                case 0x30: { swap(B); break; } // SWAP B
                case 0x31: { swap(C); break; } // SWAP C
                case 0x32: { swap(D); break; } // SWAP D
                case 0x33: { swap(E); break; } // SWAP E
                case 0x34: { swap(H); break; } // SWAP H
                case 0x35: { swap(L); break; } // SWAP L
                case 0x36: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); swap(value); bus->write(HL_address, value); break; } // SWAP (HL)

                case 0x47: { copyBitComplement(A, 0); break; } // BIT 0, A
                case 0x40: { copyBitComplement(B, 0); break; } // BIT 0, B
                case 0x41: { copyBitComplement(C, 0); break; } // BIT 0, C
                case 0x42: { copyBitComplement(D, 0); break; } // BIT 0, D
                case 0x43: { copyBitComplement(E, 0); break; } // BIT 0, E
                case 0x44: { copyBitComplement(H, 0); break; } // BIT 0, H
                case 0x45: { copyBitComplement(L, 0); break; } // BIT 0, L
                case 0x46: { uint16_t HL_address = getHL(); copyBitComplement(bus->read(HL_address), 0); break; } // BIT 0, (HL)

                case 0x4F: { copyBitComplement(A, 1); break; } // BIT 1, A
                case 0x48: { copyBitComplement(B, 1); break; } // BIT 1, B
                case 0x49: { copyBitComplement(C, 1); break; } // BIT 1, C
                case 0x4A: { copyBitComplement(D, 1); break; } // BIT 1, D
                case 0x4B: { copyBitComplement(E, 1); break; } // BIT 1, E
                case 0x4C: { copyBitComplement(H, 1); break; } // BIT 1, H
                case 0x4D: { copyBitComplement(L, 1); break; } // BIT 1, L
                case 0x4E: { uint16_t HL_address = getHL(); copyBitComplement(bus->read(HL_address), 1); break; } // BIT 1, (HL)                

                case 0x57: { copyBitComplement(A, 2); break; } // BIT 2, A
                case 0x50: { copyBitComplement(B, 2); break; } // BIT 2, B
                case 0x51: { copyBitComplement(C, 2); break; } // BIT 2, C
                case 0x52: { copyBitComplement(D, 2); break; } // BIT 2, D
                case 0x53: { copyBitComplement(E, 2); break; } // BIT 2, E
                case 0x54: { copyBitComplement(H, 2); break; } // BIT 2, H
                case 0x55: { copyBitComplement(L, 2); break; } // BIT 2, L
                case 0x56: { uint16_t HL_address = getHL(); copyBitComplement(bus->read(HL_address), 2); break; } // BIT 2, (HL)  

                case 0x5F: { copyBitComplement(A, 3); break; } // BIT 3, A
                case 0x58: { copyBitComplement(B, 3); break; } // BIT 3, B
                case 0x59: { copyBitComplement(C, 3); break; } // BIT 3, C
                case 0x5A: { copyBitComplement(D, 3); break; } // BIT 3, D
                case 0x5B: { copyBitComplement(E, 3); break; } // BIT 3, E
                case 0x5C: { copyBitComplement(H, 3); break; } // BIT 3, H
                case 0x5D: { copyBitComplement(L, 3); break; } // BIT 3, L
                case 0x5E: { uint16_t HL_address = getHL(); copyBitComplement(bus->read(HL_address), 3); break; } // BIT 3, (HL)  

                case 0x67: { copyBitComplement(A, 4); break; } // BIT 4, A
                case 0x60: { copyBitComplement(B, 4); break; } // BIT 4, B
                case 0x61: { copyBitComplement(C, 4); break; } // BIT 4, C
                case 0x62: { copyBitComplement(D, 4); break; } // BIT 4, D
                case 0x63: { copyBitComplement(E, 4); break; } // BIT 4, E
                case 0x64: { copyBitComplement(H, 4); break; } // BIT 4, H
                case 0x65: { copyBitComplement(L, 4); break; } // BIT 4, L
                case 0x66: { uint16_t HL_address = getHL(); copyBitComplement(bus->read(HL_address), 4); break; } // BIT 4, (HL)   

                case 0x6F: { copyBitComplement(A, 5); break; } // BIT 5, A
                case 0x68: { copyBitComplement(B, 5); break; } // BIT 5, B
                case 0x69: { copyBitComplement(C, 5); break; } // BIT 5, C
                case 0x6A: { copyBitComplement(D, 5); break; } // BIT 5, D
                case 0x6B: { copyBitComplement(E, 5); break; } // BIT 5, E
                case 0x6C: { copyBitComplement(H, 5); break; } // BIT 5, H
                case 0x6D: { copyBitComplement(L, 5); break; } // BIT 5, L
                case 0x6E: { uint16_t HL_address = getHL(); copyBitComplement(bus->read(HL_address), 5); break; } // BIT 5, (HL)  

                case 0x77: { copyBitComplement(A, 6); break; } // BIT 6, A
                case 0x70: { copyBitComplement(B, 6); break; } // BIT 6, B
                case 0x71: { copyBitComplement(C, 6); break; } // BIT 6, C
                case 0x72: { copyBitComplement(D, 6); break; } // BIT 6, D
                case 0x73: { copyBitComplement(E, 6); break; } // BIT 6, E
                case 0x74: { copyBitComplement(H, 6); break; } // BIT 6, H
                case 0x75: { copyBitComplement(L, 6); break; } // BIT 6, L
                case 0x76: { uint16_t HL_address = getHL(); copyBitComplement(bus->read(HL_address), 6); break; } // BIT 6, (HL)  

                case 0x7F: { copyBitComplement(A, 7); break; } // BIT 7, A
                case 0x78: { copyBitComplement(B, 7); break; } // BIT 7, B
                case 0x79: { copyBitComplement(C, 7); break; } // BIT 7, C
                case 0x7A: { copyBitComplement(D, 7); break; } // BIT 7, D
                case 0x7B: { copyBitComplement(E, 7); break; } // BIT 7, E
                case 0x7C: { copyBitComplement(H, 7); break; } // BIT 7, H
                case 0x7D: { copyBitComplement(L, 7); break; } // BIT 7, L
                case 0x7E: { uint16_t HL_address = getHL(); copyBitComplement(bus->read(HL_address), 7); break; } // BIT 7, (HL)  

                case 0x87: { res(A, 0); break; } // RES 0, A
                case 0x80: { res(B, 0); break; } // RES 0, B
                case 0x81: { res(C, 0); break; } // RES 0, C
                case 0x82: { res(D, 0); break; } // RES 0, D
                case 0x83: { res(E, 0); break; } // RES 0, E
                case 0x84: { res(H, 0); break; } // RES 0, H
                case 0x85: { res(L, 0); break; } // RES 0, L
                case 0x86: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); res(value, 0); bus->write(HL_address, value); break; } // RES 0, (HL)

                case 0x8F: { res(A, 1); break; } // RES 1, A
                case 0x88: { res(B, 1); break; } // RES 1, B
                case 0x89: { res(C, 1); break; } // RES 1, C
                case 0x8A: { res(D, 1); break; } // RES 1, D
                case 0x8B: { res(E, 1); break; } // RES 1, E
                case 0x8C: { res(H, 1); break; } // RES 1, H
                case 0x8D: { res(L, 1); break; } // RES 1, L
                case 0x8E: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); res(value, 1); bus->write(HL_address, value); break; } // RES 1, (HL)
     
                case 0x97: { res(A, 2); break; } // RES 2, A
                case 0x90: { res(B, 2); break; } // RES 2, B
                case 0x91: { res(C, 2); break; } // RES 2, C
                case 0x92: { res(D, 2); break; } // RES 2, D
                case 0x93: { res(E, 2); break; } // RES 2, E
                case 0x94: { res(H, 2); break; } // RES 2, H
                case 0x95: { res(L, 2); break; } // RES 2, L
                case 0x96: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); res(value, 2); bus->write(HL_address, value); break; } // RES 2, (HL)           

                case 0x9F: { res(A, 3); break; } // RES 3, A
                case 0x98: { res(B, 3); break; } // RES 3, B
                case 0x99: { res(C, 3); break; } // RES 3, C
                case 0x9A: { res(D, 3); break; } // RES 3, D
                case 0x9B: { res(E, 3); break; } // RES 3, E
                case 0x9C: { res(H, 3); break; } // RES 3, H
                case 0x9D: { res(L, 3); break; } // RES 3, L
                case 0x9E: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); res(value, 3); bus->write(HL_address, value); break; } // RES 3, (HL)

                case 0xA7: { res(A, 4); break; } // RES 4, A
                case 0xA0: { res(B, 4); break; } // RES 4, B
                case 0xA1: { res(C, 4); break; } // RES 4, C
                case 0xA2: { res(D, 4); break; } // RES 4, D
                case 0xA3: { res(E, 4); break; } // RES 4, E
                case 0xA4: { res(H, 4); break; } // RES 4, H
                case 0xA5: { res(L, 4); break; } // RES 4, L
                case 0xA6: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); res(value, 4); bus->write(HL_address, value); break; } // RES 4, (HL)           

                case 0xAF: { res(A, 5); break; } // RES 5, A
                case 0xA8: { res(B, 5); break; } // RES 5, B
                case 0xA9: { res(C, 5); break; } // RES 5, C
                case 0xAA: { res(D, 5); break; } // RES 5, D
                case 0xAB: { res(E, 5); break; } // RES 5, E
                case 0xAC: { res(H, 5); break; } // RES 5, H
                case 0xAD: { res(L, 5); break; } // RES 5, L
                case 0xAE: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); res(value, 5); bus->write(HL_address, value); break; } // RES 5, (HL)

                case 0xB7: { res(A, 6); break; } // RES 6, A
                case 0xB0: { res(B, 6); break; } // RES 6, B
                case 0xB1: { res(C, 6); break; } // RES 6, C
                case 0xB2: { res(D, 6); break; } // RES 6, D
                case 0xB3: { res(E, 6); break; } // RES 6, E
                case 0xB4: { res(H, 6); break; } // RES 6, H
                case 0xB5: { res(L,6); break; } // RES 4, L
                case 0xB6: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); res(value, 6); bus->write(HL_address, value); break; } // RES 6, (HL)

                case 0xBF: { res(A, 7); break; } // RES 7, A
                case 0xB8: { res(B, 7); break; } // RES 7, B
                case 0xB9: { res(C, 7); break; } // RES 7, C
                case 0xBA: { res(D, 7); break; } // RES 7, D
                case 0xBB: { res(E, 7); break; } // RES 7, E
                case 0xBC: { res(H, 7); break; } // RES 7, H
                case 0xBD: { res(L, 7); break; } // RES 57, L
                case 0xBE: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); res(value, 7); bus->write(HL_address, value); break; } // RES 7, (HL)
                
                case 0xC7: { set(A, 0); break; } // SET 0, A
                case 0xC0: { set(B, 0); break; } // SET 0, B
                case 0xC1: { set(C, 0); break; } // SET 0, C
                case 0xC2: { set(D, 0); break; } // SET 0, D
                case 0xC3: { set(E, 0); break; } // SET 0, E
                case 0xC4: { set(H, 0); break; } // SET 0, H
                case 0xC5: { set(L, 0); break; } // SET 0, L
                case 0xC6: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); set(value, 0); bus->write(HL_address, value); break; } // SET 0, (HL)

                case 0xCF: { set(A, 1); break; } // SET 1, A
                case 0xC8: { set(B, 1); break; } // SET 1, B
                case 0xC9: { set(C, 1); break; } // SET 1, C
                case 0xCA: { set(D, 1); break; } // SET 1, D
                case 0xCB: { set(E, 1); break; } // SET 1, E
                case 0xCC: { set(H, 1); break; } // SET 1, H
                case 0xCD: { set(L, 1); break; } // SET 1, L
                case 0xCE: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); set(value, 1); bus->write(HL_address, value); break; } // SET 1, (HL)

                case 0xD7: { set(A, 2); break; } // SET 2, A
                case 0xD0: { set(B, 2); break; } // SET 2, B
                case 0xD1: { set(C, 2); break; } // SET 2, C
                case 0xD2: { set(D, 2); break; } // SET 2, D
                case 0xD3: { set(E, 2); break; } // SET 2, E
                case 0xD4: { set(H, 2); break; } // SET 2, H
                case 0xD5: { set(L, 2); break; } // SET 2, L
                case 0xD6: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); set(value, 2); bus->write(HL_address, value); break; } // SET 2, (HL)

                case 0xDF: { set(A, 3); break; } // SET 3, A
                case 0xD8: { set(B, 3); break; } // SET 3, B
                case 0xD9: { set(C, 3); break; } // SET 3, C
                case 0xDA: { set(D, 3); break; } // SET 3, D
                case 0xDB: { set(E, 3); break; } // SET 3, E
                case 0xDC: { set(H, 3); break; } // SET 3, H
                case 0xDD: { set(L, 3); break; } // SET 3, L
                case 0xDE: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); set(value, 3); bus->write(HL_address, value); break; } // SET 3, (HL)

                case 0xE7: { set(A, 4); break; } // SET 4, A
                case 0xE0: { set(B, 4); break; } // SET 4, B
                case 0xE1: { set(C, 4); break; } // SET 4, C
                case 0xE2: { set(D, 4); break; } // SET 4, D
                case 0xE3: { set(E, 4); break; } // SET 4, E
                case 0xE4: { set(H, 4); break; } // SET 4, H
                case 0xE5: { set(L, 4); break; } // SET 4, L
                case 0xE6: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); set(value, 4); bus->write(HL_address, value); break; } // SET 4, (HL)

                case 0xEF: { set(A, 5); break; } // SET 5, A
                case 0xE8: { set(B, 5); break; } // SET 5, B
                case 0xE9: { set(C, 5); break; } // SET 5, C
                case 0xEA: { set(D, 5); break; } // SET 5, D
                case 0xEB: { set(E, 5); break; } // SET 5, E
                case 0xEC: { set(H, 5); break; } // SET 5, H
                case 0xED: { set(L, 5); break; } // SET 5, L
                case 0xEE: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); set(value, 5); bus->write(HL_address, value); break; } // SET 5, (HL)

                case 0xF7: { set(A, 6); break; } // SET 6, A
                case 0xF0: { set(B, 6); break; } // SET 6, B
                case 0xF1: { set(C, 6); break; } // SET 6, C
                case 0xF2: { set(D, 6); break; } // SET 6, D
                case 0xF3: { set(E, 6); break; } // SET 6, E
                case 0xF4: { set(H, 6); break; } // SET 6, H
                case 0xF5: { set(L, 6); break; } // SET 6, L
                case 0xF6: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); set(value, 6); bus->write(HL_address, value); break; } // SET 6, (HL)

                case 0xFF: { set(A, 7); break; } // SET 7, A
                case 0xF8: { set(B, 7); break; } // SET 7, B
                case 0xF9: { set(C, 7); break; } // SET 7, C
                case 0xFA: { set(D, 7); break; } // SET 7, D
                case 0xFB: { set(E, 7); break; } // SET 7, E
                case 0xFC: { set(H, 7); break; } // SET 7, H
                case 0xFD: { set(L, 7); break; } // SET 7, L
                case 0xFE: { uint16_t HL_address = getHL(); uint8_t value = bus->read(HL_address); set(value, 7); bus->write(HL_address, value); break; } // SET 7, (HL)

                default: 
                    std::cout << "Unknown opcode" << (int)CB_opcode << std::endl;
                    exit(1);
            }
            break;
        }

        case 0xC3: { PC = bus->read16(PC); break; } // JP a16
        case 0xC2: { branch = !getZ(); jp(branch); break; } // JP NZ, a16
        case 0xCA: { branch = getZ(); jp(branch); break; } // JP Z, a16
        case 0xD2: { branch = !getC(); jp(branch); break; } // JP NC, a16
        case 0xDA: { branch = getC(); jp(branch); break; } // JP C, a16

        case 0x18: { jr(true); break; } // JR s8
        case 0x20: { branch = !getZ(); jr(branch); break; } // JR NZ, s8
        case 0x28: { branch = getZ(); jr(branch); break; } // JR Z, s8
        case 0x30: { branch = !getC(); jr(branch); break; } // JR NC, s8
        case 0x38: { branch = getC(); jr(branch); break; } // JR C, s8

        case 0xE9: { PC = getHL(); break; } // JP HL

        case 0xCD: { call(true); break; } // CALL a16
        case 0xC4: { branch = !getZ(); call(branch); break; } // CALL NZ, a16

        case 0xC9: { PC = stackPop16(); break; } // RET
        case 0xD9: { PC = stackPop16(); IME = true; break; } // RETI
        case 0xC0: { branch = !getZ(); if(branch){ PC = stackPop16(); } break; } // RET NZ
        case 0xC8: { branch = getZ(); if(branch){ PC = stackPop16(); } break; } // RET Z
        case 0xD0: { branch = !getC(); if(branch){ PC = stackPop16(); } break; } // RET NC
        case 0xD8: { branch = getC(); if(branch){ PC = stackPop16(); } break; } // RET C

        case 0xC7: { stackPush16(PC); PC = 0x0000; break; } // RST 0
        case 0xCF: { stackPush16(PC); PC = 0x0008; break; } // RST 1
        case 0XD7: { stackPush16(PC); PC = 0x0010; break; } // RST 2
        case 0xDF: { stackPush16(PC); PC = 0x0018; break; } // RST 3
        case 0xE7: { stackPush16(PC); PC = 0x0020; break; } // RST 4
        case 0xEF: { stackPush16(PC); PC = 0x0028; break; } // RST 5
        case 0xF7: { stackPush16(PC); PC = 0x0030; break; } // RST 6
        case 0xFF: { stackPush16(PC); PC = 0x0038; break; } // RST 7

        case 0x27: { daa(); break; } // DAA
        case 0x2F: { A = ~A; setH(1); setN(1); break; } // CPL
        case 0x3F: { setC(!getC()); setH(false); setN(false); break; } // CCF (flip the carry flag C)
        case 0x37: { setC(true); setH(false); setN(false); break; } // SCF

        case 0xF3: { IME = false; break; }; // DI
        case 0xFB: { EI_pending = true; break; }; // EI

        case 0x76: { halted = true; break; } // HALT
        case 0x10: {
            break;
        }

        // only HALT and STOP remains

        default:
            std::cout << "Unknown opcode" << (int)opcode << std::endl;
            exit(1);
    }

    opcode_cycles += getOpcodeCycle(opcode, branch);

    // adding clock cycles
    clock += opcode_cycles;

    if(IME && (IE & IF)){
        serviceInterrupt(IF);
    }

    return opcode_cycles;
}