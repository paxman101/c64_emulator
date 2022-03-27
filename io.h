//
// Created by Paxton on 2022-03-21.
//

#ifndef C64_EMULATOR_IO_H
#define C64_EMULATOR_IO_H

#include "SDL.h"

class Io {
private:
    SDL_Window *window_;
    SDL_Renderer *renderer_;

public:
    void initScreen();
    void clearScreen() { SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255); SDL_RenderClear(renderer_); }

    void drawPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);

    void update();
};


#endif //C64_EMULATOR_IO_H
