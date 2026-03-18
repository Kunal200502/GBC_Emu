#pragma once
#include <vector>
#include <stdint.h>
#include <iostream>

class FIFO_Pixel{
    std::vector<uint8_t> array;
    int left = 0;
    int right = 0;

    public:
        FIFO_Pixel();
        void push(uint8_t);
        uint8_t pop();
        void clear();
};