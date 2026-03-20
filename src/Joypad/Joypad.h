#pragma once
#include <stdint.h>
#include <SDL2/SDL.h>
#include <iostream>

class Joypad{
    private:
        uint8_t JOYP = 0xC0;
        uint8_t buttons = 0xFF;
        void handleButton(SDL_Event&, uint8_t);
    public:
        uint8_t read() const;
        void write(uint8_t);
        bool processButtonEvent(SDL_Event&);
};