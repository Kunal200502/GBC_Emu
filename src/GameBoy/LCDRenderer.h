#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>

const uint8_t width = 160;
const uint8_t height = 144;

class LCD_Renderer{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint32_t pixelBuffer[160*144];

    public:
        LCD_Renderer();
        void drawFrame(std::vector<uint8_t>&);
};

