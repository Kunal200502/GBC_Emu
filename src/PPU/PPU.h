#pragma once
#include <stdint.h>
#include <vector>
#include <iostream>
#include "../Bus/Bus.h"

enum FetcherState{
    GET_TILE,
    GET_TILE_DATA_LOW,
    GET_TILE_DATA_HIGH,
    SLEEP
};

class FIFO_Pixel{
    std::vector<uint8_t> array;
    int left = -1;
    int right = 0;

    public:
        FIFO_Pixel();
        void push(uint8_t);
        uint8_t pop();
};

class FrameBuffer{
    public:
    uint16_t pointer = 0;
        bool drawFLag = false;
        std::vector<uint8_t> array;
        FrameBuffer();
        void push(uint8_t);
};

class PPU{
    private:
        uint8_t mode = 2;
        uint8_t dots = 0;

        Bus* bus;
        FIFO_Pixel fifoPixel;

        uint8_t getLCDC();
        uint8_t getLY();
        uint8_t getLYC();
        uint8_t getSTAT();
        uint8_t getSCX();
        uint8_t getSCY();
        uint8_t getWX();
        uint8_t getWY();

        std::vector<uint8_t> oam_buffer;
        uint8_t oam_scan_buffer;
        void OAM_scan();

        uint16_t pixelCol = 0;

        // fetcher logic
        uint8_t fetcherX = 0;
        uint8_t fetcherY = 0;
        uint16_t mode3Dots = 0;
        enum FetcherState fetcherState  = GET_TILE;
        bool switchFetcherState = true;
        uint8_t tileNumber = 0;
        uint8_t tileIndex = 0;

        bool tileLowFilled = false;
        bool tileHighFilled = false;
        uint8_t tileLow = 0;
        uint8_t tileHigh = 0;
        void fetcher(bool windowTile, bool tileMap, bool addressingMode);

        uint16_t mode1Dots = 0;

        void emulateCycle();
        
    public:
        FrameBuffer frameBuffer;
        PPU(Bus*);
        void step(uint8_t);
};