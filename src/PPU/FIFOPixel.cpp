#include "FIFOPixel.h"

FIFO_Pixel::FIFO_Pixel(){
    array = std::vector<uint8_t>(16, 0);
}
void FIFO_Pixel::push(uint8_t value){
    array[right] = value;
    right = (right+1)%16;
}
uint8_t FIFO_Pixel::pop(){
    if(left == right){
        return 0xFF;
    }
    uint8_t output = array[left];
    left = (left+1)%16;
    return output;
}

void FIFO_Pixel::clear(){
    left = 0;
    right = 0;
}