//
// Created by Paxton on 2022-03-21.
//

#include "io.h"

void Io::initScreen() {
//    SDL_SetMainReady();
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
    SDL_Init(SDL_INIT_VIDEO);
    window_ = SDL_CreateWindow("JEFF", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1500, 1000, SDL_WINDOW_RESIZABLE);

    auto render_flags = SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC;
    renderer_ = SDL_CreateRenderer(window_, -1, render_flags);
    SDL_RenderSetScale(renderer_, 2, 2);

    SDL_Surface *surface;
}

void Io::drawPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
    SDL_SetRenderDrawColor(renderer_, red, green, blue, alpha);
//    SDL_Rect rect = {x*2, y*2, 4, 4};
//    SDL_RenderDrawRect(renderer_, &rect);
//    SDL_RenderFillRect(renderer_, &rect);
    SDL_RenderDrawPoint(renderer_, x, y);
}

void Io::update() {
    SDL_RenderPresent(renderer_);
}
