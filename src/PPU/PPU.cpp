#include "PPU.h"

PPUSnapshot::PPUSnapshot(
    uint8_t mode_state, uint16_t dots_state, int8_t clock_sync_state,
    FIFO_Pixel* fifoPixel_state, FIFO_Pixel* spriteFIFOPixel_state,
    LCD_Renderer* renderer_state, OAM_Buffer* OAM_Buffer_state,
    uint8_t oamObjectNum_state, uint8_t oam_scan_buffer_state, uint16_t pixelCol_state,
    uint8_t fetcherX_state, uint8_t fetcherY_state, enum FetcherState fetcherState_state,
    bool switchFetcherState_state, uint8_t tileNumber_state, uint8_t tileIndex_state,
    uint8_t tileLow_state, uint8_t tileHigh_state, uint8_t scxDiscard_state, uint8_t spriteTile_state,
    bool fetchingSprite_state
){
    mode = mode_state,
    dots = dots_state,
    clock_sync = clock_sync_state;
    fifoPixel = fifoPixel_state->create_FIFO_Pixel_Snapshot();
    spriteFIFOPixel = spriteFIFOPixel_state->create_FIFO_Pixel_Snapshot();
    renderer = renderer_state->create_LCD_Renderer_Snapshot();
    oam_buffer = OAM_Buffer_state->createSnapshot();
    oamObjectNum = oamObjectNum_state;
    oam_scan_buffer = oam_scan_buffer_state;
    pixelCol = pixelCol_state;
    fetcherX = fetcherX_state;
    fetcherY = fetcherY_state;
    fetcherState = fetcherState_state;
    switchFetcherState = switchFetcherState_state;
    tileNumber = tileNumber_state;
    tileIndex = tileIndex_state;
    tileLow = tileLow_state;
    tileHigh = tileHigh_state;
    scxDiscard = scxDiscard_state;
    spriteTile = spriteTile_state;
    fetchingSprite = fetchingSprite_state;
}


uint8_t reverseNum(uint8_t num){
    uint8_t result = 0;
    for(int i = 0; i<8; i++){
        result = (result << 1) | (num & 1);
        num = num >> 1;
    }
    return result;
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

// mode switcher function
void PPU::switch_mode_3(){
    fetcherX = 0;
    fetcherState = GET_TILE;
    switchFetcherState = true;
    mode = 3;
    scxDiscard = 0;
    pixelCol = 0;
    fifoPixel.clear();
}

void PPU::switch_mode_2(){
    mode = 2;
    oam_buffer->clear();
    oamObjectNum = 0;

    // requesting STAT interrupt
    if(checkBit(bus->read(0xFF41), 5)){
        bus->request_interrupt(STAT_Interrupt);
    }
}

void PPU::switch_mode_1(){
    mode = 1;

    // requesting STAT interrupt
    if(checkBit(bus->read(0xFF41), 4)){
        bus->request_interrupt(STAT_Interrupt);
    }
}

void PPU::switch_mode_0(){
    mode = 0;

    // requesting STAT interrupt
    if(checkBit(bus->read(0xFF41), 3)){
        bus->request_interrupt(STAT_Interrupt);
    }
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
    dots += 2;
    clock_sync -= 2;

    oam_scan_buffer = bus->read(0xFE00+oamObjectNum*4); // reading the Y index value (1st byte) of the object
    
    uint8_t spriteY = oam_scan_buffer-16;
    uint8_t obj_height = 8 + 8*((getLCDC() >> 2) & 1);
    uint8_t LY = getLY();
    if(LY >= spriteY && LY < spriteY+obj_height){
        oam_buffer->push({oamObjectNum, bus->read(0xFE00+oamObjectNum*4+1)}); // storing the index of the object in OAM (0 - 39)
    }
    oamObjectNum++;
}

void PPU::fetcher(){
    switchFetcherState = !switchFetcherState;
    switch(fetcherState){
        case GET_TILE: {
            if(switchFetcherState){
                fetcherState = GET_TILE_DATA_LOW;
                break;
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
            
            uint16_t tileMapBase = tileMap ? 0x9C00 : 0x9800;

            fetcherY = getLY();

            uint8_t tileY;
            uint8_t tileX;
            if(window_tile){
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

            uint8_t LCDC = getLCDC();

            // checking the adressing mode (true = 0x8000, false = 0x8800)
            bool addressingMode = checkBit(LCDC, 4);

            uint16_t tileAddress;
            uint8_t lineInTile = (fetcherY+getSCY())%8;
            if(addressingMode){
                tileAddress = 0x8000 + 16*tileIndex + (lineInTile*2);
            }else{
                int8_t signedIndex = tileIndex;
                tileAddress = 0x9000 + 16*signedIndex + (lineInTile*2);
            }
            tileLow = bus->read(tileAddress);
            break;
        }
        case GET_TILE_DATA_HIGH: {
            if(switchFetcherState){
                fetcherState = SLEEP;
                break;
            }

            uint8_t LCDC = getLCDC();

            // checking the adressing mode (true = 0x8000, false = 0x8800)
            bool addressingMode = checkBit(LCDC, 4);

            uint16_t tileAddress;
            uint8_t lineInTile = (fetcherY+getSCY())%8;
            if(addressingMode){
                tileAddress = 0x8000 + 16*tileIndex + (lineInTile*2)+1;
            }else{
                int8_t signedIndex = tileIndex;
                tileAddress = 0x9000 + 16*signedIndex + (lineInTile*2)+1;
            }
            tileHigh = bus->read(tileAddress);

            // filling the FIFO with pixels
            for(int i = 7; i >= 0; i--){
                uint8_t pixel = (((tileHigh >> i) & 1) << 1)| ((tileLow >> i) & 1);
                fifoPixel.push(pixel);
            }
            fetcherX++;

            break;
        }
        case SLEEP:{
            if(switchFetcherState){
                fetcherState = GET_TILE;
            }
            break;
        }
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

    for(int i = 7; i >= 0; i--){
        uint8_t pixel = (((tileHigh >> i) & 1) << 1)| ((tileLow >> i) & 1);
        spriteFIFOPixel.push(pixel);
    }

    fetchingSprite = false;
}

void PPU::emulateCycle(){
    
    switch(mode){
        case 2:{
            // OAM scan
            OAM_scan();
            if(dots == 80){
                switch_mode_3();
            }
            break;
        }
        case 3:{
            dots++;
            clock_sync--;

            if(oam_buffer->check(pixelCol)){
                fetchingSprite = true;
            }

            if(fetchingSprite){
                spriteFetcher(oam_buffer->pop().first);
            }

            fetcher();

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
                switch_mode_0();
            }

            break;
        }
        case 0:{
            uint8_t steps = clock_sync;
            if((456 - dots) < steps){
                steps = 456 - dots;
            }

            dots += steps;
            clock_sync -= steps;

            if(dots == 456){
                dots = 0;
                uint8_t LY = getLY();
                uint8_t newLY = LY+1;
                bus->write(0xFF44, newLY);

                // requesting STAT interrupt on LY == LYC
                if(checkBit(bus->read(0xFF41), 6) && bus->read(0xFF45) == newLY){
                    bus->request_interrupt(STAT_Interrupt);
                }
                
                if(newLY <= 143){
                    switch_mode_2();
                }else{
                    bus->request_interrupt(V_Blank_Interrupt); 
                    switch_mode_1();
                }
            }
            
            break;
        }
        case 1:{
            uint8_t steps = clock_sync;
            if((456 - dots) < steps){
                steps = 456 - dots;
            }

            dots += steps;
            clock_sync -= steps;

            uint8_t LY = getLY();
            if(LY <= 153){
                if(dots == 456){
                    dots = 0;
                    bus->write(0xFF44, getLY()+1);
                }
                return;
            }
            dots = 0;

            // switching mode to 2
            switch_mode_2();

            bus->write(0xFF44, 0);
            break;
        }
    }
}

void PPU::step(uint8_t cycles){
    if(!checkBit(getLCDC(), 7)){
        dots = 0;
        clock_sync = 0;
        switch_mode_2();
        bus->write(0xFF44, 0);
        return;
    }

    clock_sync += cycles;
    while(clock_sync > 0){
        emulateCycle();
    }
}

PPUSnapshot PPU::createSnapshot(){
    PPUSnapshot ppu_snapshot(
        mode, dots, clock_sync, &fifoPixel, &spriteFIFOPixel, renderer, oam_buffer, oamObjectNum, oam_scan_buffer, pixelCol,
        fetcherX, fetcherY, fetcherState, switchFetcherState, tileNumber, tileIndex, tileLow, tileHigh, scxDiscard, 
        spriteTile, fetchingSprite
    );
    return ppu_snapshot;
}

void PPU::restoreSnapshot(PPUSnapshot* ppu_snapshot){
    mode = ppu_snapshot->mode;
    dots = ppu_snapshot->dots;
    clock_sync = ppu_snapshot->clock_sync;
    fifoPixel.restore_FIFO_Pixel_Snapshot(&ppu_snapshot->fifoPixel);
    spriteFIFOPixel.restore_FIFO_Pixel_Snapshot(&ppu_snapshot->spriteFIFOPixel);
    renderer->restore_LCD_Renderer_Snapshot(&ppu_snapshot->renderer);
    oam_buffer->restoreSnapshot(&ppu_snapshot->oam_buffer);
    oamObjectNum = ppu_snapshot->oamObjectNum;
    oam_scan_buffer = ppu_snapshot->oam_scan_buffer;
    pixelCol = ppu_snapshot->pixelCol;
    fetcherX = ppu_snapshot->fetcherX;
    fetcherY = ppu_snapshot->fetcherY;
    fetcherState = ppu_snapshot->fetcherState;
    switchFetcherState = ppu_snapshot->switchFetcherState;
    tileNumber = ppu_snapshot->tileNumber;
    tileIndex = ppu_snapshot->tileIndex;
    tileLow = ppu_snapshot->tileLow;
    tileHigh = ppu_snapshot->tileHigh;
    scxDiscard = ppu_snapshot->scxDiscard;
    spriteTile = ppu_snapshot->spriteTile;
    fetchingSprite = ppu_snapshot->fetchingSprite;
}