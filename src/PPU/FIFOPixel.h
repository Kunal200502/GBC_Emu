#pragma once
#include <vector>
#include <stdint.h>
#include <iostream>

class FIFO_Pixel{
    int left = 0;
    int right = 0;
    std::vector<uint8_t> array; 
    
    public:
        FIFO_Pixel();
        void push(uint8_t);
        uint8_t pop();
        void clear();
};