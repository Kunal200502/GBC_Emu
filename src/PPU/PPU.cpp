#include "PPU.h"

bool checkBit(uint8_t value, uint8_t bit){
    return (value >> bit) & 1;
}

FIFO_Pixel::FIFO_Pixel(){
    array = std::vector<uint8_t>(16, 0xFF);
}
void FIFO_Pixel::push(uint8_t value){
    array[right] = value;
    right = (right+1)%16;
}
uint8_t FIFO_Pixel::pop(){
    uint8_t value = array[left];
    if(value == 0xFF){
        return 0xFF;
    }
    array[left] = 0xFF;
    left = (left+1)%16;
    return value;
}

FrameBuffer::FrameBuffer(){
    array = std::vector<uint8_t>(160*144, 0);
}
void FrameBuffer::push(uint8_t value){
    array[pointer] = value;
    pointer++;
    if(pointer == 160*144){
        drawFLag = true;
        pointer = 0;
    }
}

PPU::PPU(Bus* b){
    bus = b;
    fifoPixel = FIFO_Pixel();
    frameBuffer = FrameBuffer();
}

uint8_t PPU::getLCDC(){
    return bus->read(0xFF40);
}

uint8_t PPU::getLY(){
    return bus->read(0xFF44);
}

uint8_t PPU::getLYC(){
    return bus->read(0xFF45);
}

uint8_t PPU::getSTAT(){
    return bus->read(0xFF41);
}

uint8_t PPU::getSCX(){
    return bus->read(0xFF42);
}
uint8_t PPU::getSCY(){
    return bus->read(0xFF43);
}
uint8_t PPU::getWX(){
    return bus->read(0xFF4A);
}
uint8_t PPU::getWY(){
    return bus->read(0xFF4B);
}

void PPU::OAM_scan(){
    if((dots % 2) == 1){
        oam_scan_buffer = bus->read(0xFE00+dots*2); // reading the Y index value (1st byte) of the object
    }else{
        uint8_t spriteY = oam_scan_buffer-16;
        uint8_t obj_height = 8 + 8*((getLCDC() >> 2) & 1);
        uint8_t LY = getLY();
        if(LY >= spriteY && LY < spriteY+obj_height){
            oam_buffer.push_back(dots/2); // storing the index of the object in OAM (0 - 39)
        }
    }
}

void PPU::fetcher(bool windowTile, bool tileMap, bool addressingMode){
    switchFetcherState = !switchFetcherState;
    switch(fetcherState){
        case GET_TILE: {
            if(switchFetcherState){
                fetcherState = GET_TILE_DATA_LOW;
                break;
            }
            uint16_t tileMapBase = tileMap ? 0x9C00 : 0x9800;

            uint8_t tileY;
            uint8_t tileX;
            if(windowTile){
                tileY = getLY()/8;
                tileX = pixelCol/8;
            }else{
                tileY = (getLY() + getSCY())/8;
                tileX = (pixelCol+getSCX())/8;
            }

            tileIndex = bus->read(tileMapBase+(tileY*32)+tileX);
            break;
        }
        case GET_TILE_DATA_LOW: {
            if(switchFetcherState){
                fetcherState = GET_TILE_DATA_HIGH;
                break;
            }
            uint16_t tileAddress;
            uint8_t lineInTile = (getLY() + getSCY())%8;
            if(addressingMode){
                tileAddress = 0x8000 + 16*tileIndex + (lineInTile*2);
            }else{
                int8_t signedIndex = tileIndex;
                tileAddress = 0x9000 + 16*signedIndex + (lineInTile*2);
            }
            tileLowFilled = true;
            tileLow = bus->read(tileAddress);
            break;
        }
        case GET_TILE_DATA_HIGH: {
            if(switchFetcherState){
                fetcherState = SLEEP;
                break;
            }
            uint16_t tileAddress;
            uint8_t lineInTile = (getLY() + getSCY())%8;
            if(addressingMode){
                tileAddress = 0x8000 + 16*tileIndex + (lineInTile*2)+1;
            }else{
                int8_t signedIndex = tileIndex;
                tileAddress = 0x9000 + 16*signedIndex + (lineInTile*2)+1;
            }
            tileHighFilled = true;
            tileHigh = bus->read(tileAddress);
            break;
        }
        case SLEEP:{
            if(switchFetcherState){
                fetcherState = GET_TILE;
            }
            break;
        }
    }
    if(tileLowFilled && tileHighFilled){
        for(int i = 7; i >= 0; i--){
            uint8_t pixel = (((tileHigh >> i) & 1) << 1)| ((tileLow >> i) & 1);
            fifoPixel.push(pixel);
        }
        pixelCol += 8;
        tileHighFilled = false;
        tileLowFilled = false;
    }
}

void PPU::emulateCycle(){
    dots++;
    switch(mode){
        case 2:{
            // OAM scan
            OAM_scan();
            if(dots == 80){
                mode = 3;
            }
            break;
        }
        case 3:{
            mode3Dots++;
            uint8_t LCDC = getLCDC();
            uint8_t WX = getWX();
            uint8_t WY = getWY();

            // checking if the tile is a window tile or a background tile
            bool window_tile = false;
            if(checkBit(LCDC, 5) && getLY() >= WY && pixelCol >= (WX-7)){
                window_tile = true;
            }

            // checking the tile map address (false = 0x9800, true = 0x9C00)
            bool tileMap = false; 
            if((checkBit(LCDC, 3) && !window_tile) || (checkBit(LCDC, 6) && window_tile)){
                tileMap = true;
            }

            // checking the adressing mode (true = 0x8000, false = 0x8800)
            bool addressingMode = checkBit(LCDC, 4);

            fetcher(window_tile, tileMap, addressingMode);

            uint8_t popPixel = fifoPixel.pop();
            
            if(popPixel == 0xFF){
                return;
            }
            frameBuffer.push(popPixel);

            if(pixelCol == 160){
                mode = 0;
            }

            break;
        }
        case 0:{
            mode3Dots++;
            if(mode3Dots == 376){
                mode3Dots = 0;
                uint8_t LY = getLY();
                uint8_t newLY = LY+1;
                bus->write(0xFF44, newLY);
                
                if(LY <= 143){
                    pixelCol = 0;
                    mode = 2;
                }else{
                    uint8_t IF = bus->read(0xFF0F);
                    IF |= 1;
                    bus->write(0xFF0F, IF);
                    mode = 1;
                }
            }
            
            break;
        }
        case 1:{
            if(mode1Dots != 4560){
                if((mode1Dots % 456) == 0){
                    bus->write(0xFF44, getLY()+1);
                }
                mode1Dots++;
                return;
            }
            mode1Dots = 0;
            mode = 2;
            bus->write(0xFF44, 0);
        }
    }
}

void PPU::step(uint8_t cycles){
    for(int i = 0; i<cycles; i++){
        emulateCycle();
    }
}