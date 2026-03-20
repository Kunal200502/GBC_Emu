#pragma once
#include <stdint.h>
#include <vector>
#include <iostream>
#include "../Bus/Bus.h"
#include "LCDRenderer.h"
#include "FIFOPixel.h"
#include <queue>

// compare function for the priority queue
struct Compare{
    bool operator()(std::pair<uint8_t, uint8_t> a, std::pair<uint8_t, uint8_t> b){
        return a.second > b.second;
    }
};

class OAM_Buffer{
    private:
        std::priority_queue<std::pair<uint8_t, uint8_t>, std::vector<std::pair<uint8_t, uint8_t>>, Compare> pq;
    public:
        int size;
        OAM_Buffer();
        void push(std::pair<uint8_t, uint8_t>);
        bool check(uint8_t pixelCol);
        std::pair<uint8_t, uint8_t> pop();
        void clear();
};

enum FetcherState{
    GET_TILE,
    GET_TILE_DATA_LOW,
    GET_TILE_DATA_HIGH,
    SLEEP
};

class PPU{
    private:
        uint8_t mode = 2;
        uint16_t dots = 0;

        Bus* bus;
        FIFO_Pixel fifoPixel;
        FIFO_Pixel spriteFIFOPixel;
        LCD_Renderer* renderer;

        uint8_t getLCDC();
        uint8_t getLY();
        uint8_t getLYC();
        uint8_t getSTAT();
        uint8_t getSCX();
        uint8_t getSCY();
        uint8_t getWX();
        uint8_t getWY();

        uint8_t oamObjectNum = 0;
        OAM_Buffer* oam_buffer;
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

        void spriteFetcher(uint8_t);

        uint8_t scxDiscard = 0;

        uint16_t mode1Dots = 1;

        void emulateCycle();

        // sprite fetching logic
        uint8_t spriteTile = 0;
        bool fetchingSprite = false;
        
    public:
        PPU(Bus*);
        void step(uint8_t);
};