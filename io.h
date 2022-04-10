//
// Created by Paxton on 2022-03-21.
//

#ifndef C64_EMULATOR_IO_H
#define C64_EMULATOR_IO_H

#include <vector>
#include <unordered_map>
#include <set>

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

    // Maps keycodes to the row and column pairs of the C64's keyboard.
    // from: http://c64os.com/post/howthekeyboardworks
    const std::unordered_map<SDL_Scancode, std::pair<uint8_t, uint8_t>> key_map = {
            {SDL_SCANCODE_STOP, {7,7}},     {SDL_SCANCODE_Q, {7,6}},           {SDL_SCANCODE_APPLICATION, {7,5}}, {SDL_SCANCODE_SPACE, {7,4}},  {SDL_SCANCODE_2, {7,3}},     {SDL_SCANCODE_LCTRL, {7,2}},     {SDL_SCANCODE_LEFT, {7,1}},         {SDL_SCANCODE_1, {7,0}},
            {SDL_SCANCODE_SLASH, {6,7}},    {SDL_SCANCODE_UP, {6,6}},          {SDL_SCANCODE_APOSTROPHE, {6,5}},       {SDL_SCANCODE_RSHIFT, {6,4}}, {SDL_SCANCODE_HOME, {6,3}},  {SDL_SCANCODE_SEMICOLON, {6,2}}, {SDL_SCANCODE_RIGHTBRACKET, {6,1}}, {SDL_SCANCODE_EQUALS, {6,0}},
            {SDL_SCANCODE_COMMA, {5,7}},    {SDL_SCANCODE_LEFTBRACKET, {5,6}}, {SDL_SCANCODE_SEMICOLON, {5,5}},       {SDL_SCANCODE_PERIOD, {5,4}}, {SDL_SCANCODE_MINUS, {5,3}}, {SDL_SCANCODE_L, {5,2}},         {SDL_SCANCODE_P, {5,1}},            {SDL_SCANCODE_SLASH, {5,0}},
            {SDL_SCANCODE_N, {4,7}},        {SDL_SCANCODE_O, {4,6}},           {SDL_SCANCODE_K, {4,5}},           {SDL_SCANCODE_M, {4,4}},      {SDL_SCANCODE_0, {4,3}},     {SDL_SCANCODE_J, {4,2}},         {SDL_SCANCODE_I, {4,1}},            {SDL_SCANCODE_9, {4,0}},
            {SDL_SCANCODE_V, {3,7}},        {SDL_SCANCODE_U, {3,6}},           {SDL_SCANCODE_H, {3,5}},           {SDL_SCANCODE_B, {3,4}},      {SDL_SCANCODE_8, {3,3}},     {SDL_SCANCODE_G, {3,2}},         {SDL_SCANCODE_Y, {3,1}},            {SDL_SCANCODE_7, {3,0}},
            {SDL_SCANCODE_X, {2,7}},        {SDL_SCANCODE_T, {2,6}},           {SDL_SCANCODE_F, {2,5}},           {SDL_SCANCODE_C, {2,4}},      {SDL_SCANCODE_6, {2,3}},     {SDL_SCANCODE_D, {2,2}},         {SDL_SCANCODE_R, {2,1}},            {SDL_SCANCODE_5, {2,0}},
            {SDL_SCANCODE_LSHIFT, {1,7}},   {SDL_SCANCODE_E, {1,6}},           {SDL_SCANCODE_S, {1,5}},           {SDL_SCANCODE_Z, {1,4}},      {SDL_SCANCODE_4, {1,3}},     {SDL_SCANCODE_A, {1,2}},         {SDL_SCANCODE_W, {1,1}},            {SDL_SCANCODE_3, {1,0}},
            {SDL_SCANCODE_PAGEDOWN, {0,7}}, {SDL_SCANCODE_F5, {0,6}},          {SDL_SCANCODE_F3, {0,5}},          {SDL_SCANCODE_F1, {0,4}},     {SDL_SCANCODE_F7, {0,3}},    {SDL_SCANCODE_PAGEUP, {0,2}},    {SDL_SCANCODE_RETURN, {0,1}},       {SDL_SCANCODE_BACKSPACE, {0,0}},
    };

    std::set<std::pair<uint8_t, uint8_t>> currently_pressed_keys_;

    std::pair<uint8_t, uint8_t> last_pressed_key_;
    bool valid_key_;

    void updateKey(SDL_Scancode scancode, bool set);
public:
    Io(int horizontal_pixels, int vertical_pixels) : text_width{horizontal_pixels}, text_height{vertical_pixels} {}

    void initScreen();
    void clearScreen() { SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255); SDL_RenderClear(renderer_); }

    void drawPixel(int x, int y, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);
    void drawPixelStrip(int x, int y, int len, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);

    void updateScreen();

    void checkKeyboard();
    std::pair<uint8_t, uint8_t> getKeyboardState();

    // Returns currently pressed keys in given column code
    uint8_t getKeysByColumn(uint8_t key_column);
    // Returns currently pressed keys in given row code
    uint8_t getKeysByRow(uint8_t key_row);
};


#endif //C64_EMULATOR_IO_H
