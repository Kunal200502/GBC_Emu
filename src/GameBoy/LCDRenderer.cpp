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
        160*10,
        144*10,
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

void LCD_Renderer::drawFrame(std::vector<uint8_t>& array){
    for(int i = 0; i<array.size(); i++){
        switch(array[i] & 0x3){
            case 0x0:
                pixelBuffer[i] = 0xFFFFFFFF;
                break;
            case 0x1:
                pixelBuffer[i] = 0xD3D3D3FF;
                break;
            case 0x2:
                pixelBuffer[i] = 0xA9A9A9FF;
                break;
            case 0x3:
                pixelBuffer[i] = 0x000000FF;
                break;
        }
    }

    SDL_UpdateTexture(texture, NULL, pixelBuffer, 160*4);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}