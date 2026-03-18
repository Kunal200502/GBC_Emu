#include "LCDRenderer.h"

LCD_Renderer::LCD_Renderer(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Could not initialize system" << std::endl;
        exit(1);
    }
    
    window = SDL_CreateWindow(
        "GameBoyColor",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        160*5,
        144*5,
        SDL_WINDOW_SHOWN
    );

    if(window == NULL){
        std::cout << "Could not create window" << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, 160, 144);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
}


void LCD_Renderer::pushPixel(uint8_t pixel){
    pixelBuffer[pixelBufferPointer] = pixelMapper[pixel & 0x3];
    pixelBufferPointer++;
    if(pixelBufferPointer == 160*144){
        pixelBufferPointer = 0;
        drawFrame();
    }
}

void LCD_Renderer::drawFrame(){
    SDL_UpdateTexture(texture, NULL, pixelBuffer, 160*4);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}