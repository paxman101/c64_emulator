//
// Created by Paxton on 2022-03-21.
//

#ifndef C64_EMULATOR_IO_H
#define C64_EMULATOR_IO_H

#include <vector>

#include "SDL.h"

class Io {
private:
    SDL_Window *window_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;
    SDL_Texture *texture_ = nullptr;
    const int text_width ;
    const int text_height;
    int pitch = 0;
    uint8_t *pixels_ = nullptr;

public:
    Io(int horizontal_pixels, int vertical_pixels) : text_width{horizontal_pixels}, text_height{vertical_pixels} {}

    void initScreen();
    void clearScreen() { SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255); SDL_RenderClear(renderer_); }

    void drawPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
    void drawPixelStrip(int x, int y, int len, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);

    void update();
};


#endif //C64_EMULATOR_IO_H
