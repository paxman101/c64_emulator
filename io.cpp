//
// Created by Paxton on 2022-03-21.
//

#include "io.h"

void Io::initScreen() {
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
    SDL_Init(SDL_INIT_VIDEO);
    window_ = SDL_CreateWindow("JEFF", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, text_width*4, text_height*4, SDL_WINDOW_RESIZABLE);

    auto render_flags = SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC;
    renderer_ = SDL_CreateRenderer(window_, -1, render_flags);
//    SDL_RenderSetScale(renderer_, 2, 2);

    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, text_width, text_height);
    SDL_LockTexture(texture_, nullptr, reinterpret_cast<void **>(&pixels_), &pitch);
}

void Io::drawPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
//    SDL_SetRenderDrawColor(renderer_, red, green, blue, alpha);
//    SDL_Rect rect = {x*2, y*2, 4, 4};
//    SDL_RenderDrawRect(renderer_, &rect);
//    SDL_RenderFillRect(renderer_, &rect);
    int offset = (text_width*4*y) + x * 4;
    pixels_[offset] = red;
    pixels_[offset + 1] = green;
    pixels_[offset + 2] = blue;
    pixels_[offset + 3] = alpha;
//    SDL_RenderDrawPoint(renderer_, x, y);
}

void Io::drawPixelStrip(int x, int y, int len, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha) {
    SDL_SetRenderDrawColor(renderer_, red, green, blue, alpha);
    SDL_Rect rect = {x, y, len, 1};
    SDL_RenderDrawRect(renderer_, &rect);
    SDL_RenderFillRect(renderer_, &rect);
}

void Io::update() {
    SDL_UnlockTexture(texture_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
    SDL_LockTexture(texture_, nullptr, reinterpret_cast<void **>(&pixels_), &pitch);
    pitch /= 4;
}
