#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>

const uint8_t width = 160;
const uint8_t height = 144;

class LCD_Renderer{
    std::vector<uint32_t> pixelMapper = {0xFFFFFFFF, 0xD3D3D3FF, 0xA9A9A9FF, 0x000000FF};;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t pixelBuffer[160*144];
    uint16_t pixelBufferPointer = 0;
    void drawFrame();

    public:
        LCD_Renderer();
        void pushPixel(uint8_t);
};
