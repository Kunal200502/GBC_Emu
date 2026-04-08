#include "LCDRenderer.h"

LCD_Renderer_Snapshot::LCD_Renderer_Snapshot(uint8_t width_state, uint8_t height_state, uint32_t pixelBuffer_state[], uint32_t pixelBufferPointer_state){
    width = width_state;
    height = height_state;
    pixelBufferPointer = pixelBufferPointer_state;

    for(int i = 0; i<pixelBufferPointer; i++){
        switch(pixelBuffer_state[i]){
            case 0xFFFFFFFF: { pixelBuffer[i] = 0; break; }
            case 0xD3D3D3FF: { pixelBuffer[i] = 1; break; }
            case 0xA9A9A9FF: { pixelBuffer[i] = 2; break; }
            case 0x000000FF: { pixelBuffer[i] = 3; break; }
        }
    }
}

LCD_Renderer::LCD_Renderer(uint8_t w, uint8_t h, uint8_t scale){
    clock = std::chrono::steady_clock::now();
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
    pixelBuffer[pixelBufferPointer] = pixelMapper[pixel];
    pixelBufferPointer++;
    if(pixelBufferPointer == width*height){
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << 1000/(std::chrono::duration_cast<std::chrono::milliseconds>(end - clock).count()) << std::endl;
        clock = end;
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


LCD_Renderer_Snapshot LCD_Renderer::create_LCD_Renderer_Snapshot(){
    LCD_Renderer_Snapshot lcd_renderer_snapshot(width, height, pixelBuffer, pixelBufferPointer);
    return lcd_renderer_snapshot;
}

void LCD_Renderer::restore_LCD_Renderer_Snapshot(LCD_Renderer_Snapshot* snapshot){
    width = snapshot->width;
    height = snapshot->height;
    pixelBufferPointer = snapshot->pixelBufferPointer;

    for(int i = 0; i<pixelBufferPointer; i++){
        switch(snapshot->pixelBuffer[i]){
            case 0x00: { pixelBuffer[i] = 0xFFFFFFFF; break; }
            case 0x01: { pixelBuffer[i] = 0xD3D3D3FF; break; } 
            case 0x02: { pixelBuffer[i] = 0xA9A9A9FF; break; }
            case 0x03: { pixelBuffer[i] = 0x000000FF; break; }
        }
    }
}