#pragma once
#include <vector>
#include <stdint.h>
#include <iostream>

class FIFO_Pixel_Snapshot{
    public:
        int left;
        int right;
        std::vector<uint8_t> array;
        FIFO_Pixel_Snapshot(int, int, std::vector<uint8_t>);
        FIFO_Pixel_Snapshot(){}

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & left;
            ar & right;
            ar & array;
        }
};

class FIFO_Pixel{
    int left = 0;
    int right = 0;
    std::vector<uint8_t> array; 
    
    public:
        FIFO_Pixel();
        void push(uint8_t);
        uint8_t pop();
        void clear();
        FIFO_Pixel_Snapshot create_FIFO_Pixel_Snapshot();
        void restore_FIFO_Pixel_Snapshot(FIFO_Pixel_Snapshot*);
};