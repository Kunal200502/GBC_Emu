#pragma once
#include <vector>
#include <stdint.h>

extern std::vector<uint8_t> opocode_cycles_branch1;
extern std::vector<uint8_t> opocode_cycles_branch2;
extern std::vector<uint8_t> CB_opcode_cycles;

uint8_t getOpcodeCycle(uint8_t opcode, bool branch);
uint8_t getCBOpcodeCycle(uint8_t opcode);