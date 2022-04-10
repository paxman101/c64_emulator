//
// Created by Paxton on 2022-03-21.
//

#include "io.h"

#include <iostream>

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

void Io::updateScreen() {
    SDL_UnlockTexture(texture_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
    SDL_LockTexture(texture_, nullptr, reinterpret_cast<void **>(&pixels_), &pitch);
    pitch /= 4;
}

void Io::checkKeyboard() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                updateKey(event.key.keysym.scancode, true);
                break;
            case SDL_KEYUP:
                updateKey(event.key.keysym.scancode, false);
                break;
        }
    }
}

void Io::updateKey(SDL_Scancode scancode, bool set) {
    auto key_vals = key_map.find(scancode);
    // Unmapped code
    if (key_vals == key_map.end()) {
        return;
    }

    if (set) {
        if (currently_pressed_keys_.size() == 2) {
            auto current_ele = *currently_pressed_keys_.begin();
            if (current_ele.first == key_vals->second.first ) {
                return;
            }
        }
        currently_pressed_keys_.insert(key_vals->second);
//          last_pressed_key_ = key_vals->second;
//          valid_key_ = true;
    }
    else {
//        if (last_pressed_key_ == key_vals->second) {
//            valid_key_ = false;
//        }
        currently_pressed_keys_.erase(key_vals->second);
    }
}

std::pair<uint8_t, uint8_t> Io::getKeyboardState() {
    uint8_t col = 0xff;
    uint8_t row = 0xff;
//    if (!valid_key_) {
//        return {col, row};
//    }
    for (auto key : currently_pressed_keys_) {
        std::pair<uint8_t, uint8_t> pair = key;
        col &= ~(1 << pair.first);
        row &= ~(1 << pair.second);
    }
//    col ^= 1 << last_pressed_key_.first;
//    row ^= 1 << last_pressed_key_.second;
    return {col, row};
}

uint8_t Io::getKeysByColumn(uint8_t key_column) {
    uint8_t row_code = 0xFF;

    for (auto &key_code : currently_pressed_keys_) {
        // if they share a 0
        if ((1 << key_code.first) & ~key_column) {
            row_code &= ~(1 << key_code.second);
        }
    }

    return row_code;
}

uint8_t Io::getKeysByRow(uint8_t key_row) {
    uint8_t column_code = 0xFF;

    for (auto &key_code : currently_pressed_keys_) {
        if ((1 << key_code.second) & ~key_row) {
            column_code &= ~(1 << key_code.first);
        }
    }

    return column_code;
}
