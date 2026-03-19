#include "LCDRenderer.h"

LCD_Renderer::LCD_Renderer(uint8_t w, uint8_t h, uint8_t scale){
    width = w;
    height = h;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Could not initialize system" << std::endl;
        exit(1);
    }
    
    window = SDL_CreateWindow(
        "GameBoyColor",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        w*scale,
        h*scale,
        SDL_WINDOW_SHOWN
    );

    if(window == NULL){
        std::cout << "Could not create window" << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
}


void LCD_Renderer::pushPixel(uint8_t pixel){
    pixelBuffer[pixelBufferPointer] = pixelMapper[pixel & 0x3];
    pixelBufferPointer++;
    if(pixelBufferPointer == width*height){
        pixelBufferPointer = 0;
        drawFrame();
    }
}

void LCD_Renderer::drawFrame(){
    SDL_UpdateTexture(texture, NULL, pixelBuffer, width*4);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}