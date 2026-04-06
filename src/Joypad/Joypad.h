#pragma once
#include <stdint.h>
#include <SDL2/SDL.h>
#include <iostream>

class JoypadSnapshot{
    public:
        uint8_t JOYP;
        uint8_t buttons;
        JoypadSnapshot(uint8_t, uint8_t);
        JoypadSnapshot(){}
           
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version){
            ar & JOYP;
            ar & buttons;
}
};

class Joypad{
    private:
        uint8_t JOYP = 0xC0;
        uint8_t buttons = 0xFF;
        void handleButton(SDL_Event&, uint8_t);
    public:
        uint8_t read() const;
        void write(uint8_t);
        bool processButtonEvent(SDL_Event&);
        JoypadSnapshot create_Joypad_Snapshot();
        void restore_Joypad_Snapshot(JoypadSnapshot*);
};