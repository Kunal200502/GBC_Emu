#include "Joypad.h"

void Joypad::handleButton(SDL_Event& event, uint8_t bit){
    if(event.type == SDL_KEYDOWN){
        buttons &= ~(1 << bit);
    }else if(event.type == SDL_KEYUP){
        buttons |= (1 << bit);
    }
}

bool Joypad::processButtonEvent(SDL_Event& event){
    switch(event.key.keysym.sym){
        case SDLK_d: { handleButton(event, 0); break; } // right
        case SDLK_a: { handleButton(event, 1); break; } // left
        case SDLK_w: { handleButton(event, 2); break; } // up
        case SDLK_s: { handleButton(event, 3); break; } // down
        case SDLK_l: { handleButton(event, 4); break; } // A
        case SDLK_j: { handleButton(event, 5); break; } // B
        case SDLK_i: { handleButton(event, 6); break; } // select
        case SDLK_k: { handleButton(event, 7); break; } // start
        default: { return false; };
    }
    return true;
}

uint8_t Joypad::read() const{
    if(((JOYP >> 5) & 1) ==  0){
        return (JOYP | 0xC0) | (buttons >> 4);
    }else if(((JOYP >> 4) & 1) == 0){
        return (JOYP | 0xC0) | (buttons & 0xF);
    }
    return 0xFF;
}

void Joypad::write(uint8_t value){
    JOYP = (value & 0xF0);
}