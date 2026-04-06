#pragma once
#include <stdint.h>
#include <vector>
#include <iostream>
#include "../Bus/Bus.h"
#include "LCDRenderer.h"
#include "FIFOPixel.h"
#include <queue>
#include "OAMBuffer.h"

enum FetcherState{
    GET_TILE,
    GET_TILE_DATA_LOW,
    GET_TILE_DATA_HIGH,
    SLEEP
};

class PPUSnapshot{
    public:
        uint8_t mode;
        uint16_t dots;
        int8_t clock_sync;

        FIFO_Pixel_Snapshot fifoPixel;
        FIFO_Pixel_Snapshot spriteFIFOPixel;
        LCD_Renderer_Snapshot renderer;

        OAM_Buffer_Snapshot oam_buffer;

        uint8_t oamObjectNum;
        uint8_t oam_scan_buffer;
        uint16_t pixelCol;

        uint8_t fetcherX;
        uint8_t fetcherY;

        enum FetcherState fetcherState;
        bool switchFetcherState;
        uint8_t tileNumber;
        uint8_t tileIndex;

        uint8_t tileLow;
        uint8_t tileHigh;

        uint8_t scxDiscard;

        uint8_t spriteTile;
        bool fetchingSprite;

        PPUSnapshot(uint8_t, uint16_t, int8_t, FIFO_Pixel*, 
            FIFO_Pixel*, LCD_Renderer*, OAM_Buffer*, uint8_t, uint8_t, uint16_t, uint8_t, uint8_t,
            enum FetcherState, bool, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, bool);
        PPUSnapshot(){}
 
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & mode;
            ar & dots;
            ar & clock_sync;
            ar & fifoPixel;
            ar & spriteFIFOPixel;
            ar & renderer;
            ar & oam_buffer;
            ar & oamObjectNum;
            ar & oam_scan_buffer;
            ar & pixelCol;
            ar & fetcherX;
            ar & fetcherY;
            ar & fetcherState;
            ar & switchFetcherState;
            ar & tileNumber;
            ar & tileIndex;
            ar & tileLow;
            ar & tileHigh;
            ar & scxDiscard;
            ar & spriteTile;
            ar & fetchingSprite;
        }
};

class PPU{
    private:
        uint8_t mode = 2;
        uint16_t dots = 0;

        int8_t clock_sync = 0;

        Bus* bus;
        FIFO_Pixel fifoPixel;
        FIFO_Pixel spriteFIFOPixel;
        LCD_Renderer* renderer;

        // mode switching logic
        void switch_mode_3();
        void switch_mode_2();
        void switch_mode_1();
        void switch_mode_0();

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
        
        enum FetcherState fetcherState  = GET_TILE;
        bool switchFetcherState = true;
        uint8_t tileNumber = 0;
        uint8_t tileIndex = 0;

        uint8_t tileLow = 0;
        uint8_t tileHigh = 0;
        void fetcher();

        void spriteFetcher(uint8_t);

        uint8_t scxDiscard = 0;

        void emulateCycle();

        // sprite fetching logic
        uint8_t spriteTile = 0;
        bool fetchingSprite = false;
        
    public:
        PPU(Bus*);
        void step(uint8_t);
        PPUSnapshot createSnapshot();
        void restoreSnapshot(PPUSnapshot*);
};