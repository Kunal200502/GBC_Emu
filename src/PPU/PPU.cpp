#include "PPU.h"

uint8_t reverseNum(uint8_t num){
    uint8_t result = 0;
    for(int i = 0; i<8; i++){
        result = (result << 1) | (num & 1);
        num >> 1;
    }
    return result;
}

OAM_Buffer::OAM_Buffer(){
    size = 0;
}

void OAM_Buffer::push(std::pair<uint8_t, uint8_t> object){
    if(size == 10){
        return;
    }
    pq.push(object);
    size++;
}

bool OAM_Buffer::check(uint8_t pixelCol){
    if(size == 0){
        return false;
    }
    while(size != 0 && pq.top().second < pixelCol){
        pq.pop();
        size--;
    }
    uint8_t x = pq.top().second;
    return (x-8 <= pixelCol && x > pixelCol);
}

void OAM_Buffer::clear(){
    while(!pq.empty()){
        pq.pop();
    }
    size = 0;
}

std::pair<uint8_t, uint8_t> OAM_Buffer::pop(){
    std::pair<uint8_t, uint8_t> output = pq.top();
    pq.pop();
    size--;
    return output;
}


bool checkBit(uint8_t value, uint8_t bit){
    return (value >> bit) & 1;
}

PPU::PPU(Bus* b){
    bus = b;
    fifoPixel = FIFO_Pixel();
    spriteFIFOPixel = FIFO_Pixel();
    renderer = new LCD_Renderer(160, 144, 5);
    oam_buffer = new OAM_Buffer();
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
    return bus->read(0xFF43);
}
uint8_t PPU::getSCY(){
    return bus->read(0xFF42);
}
uint8_t PPU::getWY(){
    return bus->read(0xFF4A);
}
uint8_t PPU::getWX(){
    return bus->read(0xFF4B);
}

void PPU::OAM_scan(){
    if((dots % 2) == 1){
        oam_scan_buffer = bus->read(0xFE00+oamObjectNum*4); // reading the Y index value (1st byte) of the object
    }else{
        uint8_t spriteY = oam_scan_buffer-16;
        uint8_t obj_height = 8 + 8*((getLCDC() >> 2) & 1);
        uint8_t LY = getLY();
        if(LY >= spriteY && LY < spriteY+obj_height){
            oam_buffer->push({oamObjectNum, bus->read(0xFE00+oamObjectNum*4+1)}); // storing the index of the object in OAM (0 - 39)
        }
        oamObjectNum++;
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

            fetcherY = getLY();

            uint8_t tileY;
            uint8_t tileX;
            if(windowTile){
                tileY = fetcherY/8;
                tileX = fetcherX;
            }else{
                tileY = ((fetcherY + getSCY()) & 255)/8;
                tileX = ((getSCX()/8)+fetcherX) & 0x1F;
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
            uint8_t lineInTile = (fetcherY+getSCY())%8;
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
            uint8_t lineInTile = (fetcherY+getSCY())%8;
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
        fetcherX++;
        tileHighFilled = false;
        tileLowFilled = false;
    }
}

void PPU::spriteFetcher(uint8_t objectNum){
    uint16_t objAddress = 0xFE00+objectNum*4;

    uint8_t yPosition = bus->read(objAddress);
    uint8_t xPosition = bus->read(objAddress+1);
    uint8_t tileIndex = bus->read(objAddress+2);
    uint8_t objAttr = bus->read(objAddress+3);

    
    uint8_t lineInTile = getLY()-(yPosition-16);
    if(checkBit(objAttr, 6)){
        lineInTile = 7-lineInTile;
    }

    uint16_t byteOffset = 16*tileIndex+(lineInTile*2);

    uint8_t tileLow = bus->read(0x8000+byteOffset);
    uint8_t tileHigh = bus->read(0x8000+byteOffset+1);

    if(checkBit(objAttr, 5)){
        tileLow = reverseNum(tileLow);
        tileHigh = reverseNum(tileHigh);
    }

    uint8_t noOfPixels = xPosition-pixelCol-getSCX();

    for(int i = noOfPixels-1; i >= 0; i--){
        uint8_t pixel = (((tileHigh >> i) & 1) << 1)| ((tileLow >> i) & 1);
        spriteFIFOPixel.push(pixel);
    }

    fetchingSprite = false;
}

void PPU::emulateCycle(){
    if(!checkBit(getLCDC(), 7)){
        mode = 2;
        dots = 0;
        bus->write(0xFF44, 0);
        return;
    }
    dots++;
    switch(mode){
        case 2:{
            // OAM scan
            OAM_scan();
            if(dots == 80){
                // if(oam_buffer->size != 0){
                //     while(oam_buffer->size != 0){
                //         std::cout << (int)oam_buffer->pop().second << " ";
                //     }
                //     std::cout << std::endl;
                // }
                fetcherX = 0;
                fetcherState = GET_TILE;
                tileHighFilled = false;
                tileLowFilled = false;
                switchFetcherState = true;
                mode = 3;
                scxDiscard = 0;
                pixelCol = 0;
                fifoPixel.clear();
            }
            break;
        }
        case 3:{
            if(oam_buffer->check(pixelCol+getSCX())){
                fetchingSprite = true;
            }

            if(fetchingSprite){
                spriteFetcher(oam_buffer->pop().first);
            }
            
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
            if(scxDiscard < (getSCX()%8)){
                scxDiscard++;
                return;
            }

            uint8_t spritePopPixel = spriteFIFOPixel.pop();

            if(spritePopPixel != 0xFF){
                renderer->pushPixel(spritePopPixel);
            }else{
                renderer->pushPixel(popPixel);
            }

            pixelCol++;

            if(pixelCol >= 160){
                mode = 0;
            }

            break;
        }
        case 0:{
            if(dots == 456){
                dots = 0;
                uint8_t LY = getLY();
                uint8_t newLY = LY+1;
                bus->write(0xFF44, newLY);
                
                if(newLY <= 143){
                    mode = 2;
                    oam_buffer->clear();
                    oamObjectNum = 0;
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
            uint8_t LY = getLY();
            if(LY <= 153){
                if(dots == 456){
                    dots = 0;
                    bus->write(0xFF44, getLY()+1);
                }
                return;
            }
            dots = 0;
            mode = 2;
            oam_buffer->clear();
            oamObjectNum = 0;
            bus->write(0xFF44, 0);
            break;
        }
    }
}

void PPU::step(uint8_t cycles){
    for(int i = 0; i<cycles; i++){
        emulateCycle();
    }
}