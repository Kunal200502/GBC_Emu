#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <chrono>

class LCD_Renderer_Snapshot{
    public:
        uint8_t width;
        uint8_t height;
        uint8_t pixelBuffer[160*144];
        uint32_t pixelBufferPointer;

        LCD_Renderer_Snapshot(uint8_t, uint8_t, uint32_t[], uint32_t);
        LCD_Renderer_Snapshot(){}
        
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & width;
            ar & height;
            ar & pixelBufferPointer;
            ar & pixelBuffer;
        }
};

class LCD_Renderer{
    std::chrono::steady_clock::time_point clock;
    uint8_t width;
    uint8_t height;
    std::vector<uint32_t> pixelMapper = {0xFFFFFFFF, 0xD3D3D3FF, 0xA9A9A9FF, 0x000000FF};;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t pixelBuffer[160*144];
    uint32_t pixelBufferPointer = 0;
    void drawFrame();

    public:
        LCD_Renderer(uint8_t, uint8_t, uint8_t);
        void pushPixel(uint8_t);
        LCD_Renderer_Snapshot create_LCD_Renderer_Snapshot();
        void restore_LCD_Renderer_Snapshot(LCD_Renderer_Snapshot*);
        
};
