#include "FIFOPixel.h"

FIFO_Pixel_Snapshot::FIFO_Pixel_Snapshot(int left_state, int right_state, std::vector<uint8_t> array_state){
    left = left_state;
    right = right_state;
    array = std::vector<uint8_t>(16, 0);
    for(int i = 0; i<16; i++){
        array[i] = array_state[i];
    }
}




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

FIFO_Pixel_Snapshot FIFO_Pixel::create_FIFO_Pixel_Snapshot(){
    FIFO_Pixel_Snapshot fifo_pixel_snapshot(left, right, array);
    return fifo_pixel_snapshot;
}

void FIFO_Pixel::restore_FIFO_Pixel_Snapshot(FIFO_Pixel_Snapshot* fifo_Pixel_Snapshot){
    left = fifo_Pixel_Snapshot->left;
    right = fifo_Pixel_Snapshot->right;
    for(int i = 0; i<16; i++){
        array[i] = fifo_Pixel_Snapshot->array[i];
    }
}