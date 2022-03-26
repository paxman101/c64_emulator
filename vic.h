//
// Created by Paxton on 2022-03-20.
//

#ifndef C64_EMULATOR_VIC_H
#define C64_EMULATOR_VIC_H

#include <cstdint>
#include <limits>
#include <vector>

#include "colorram.h"

class Memory;

class Vic {
private:
    // Register names with their value corresponding to the register index
    enum VicRegister {
        M0X = 0, M0Y,
        M1X, M1Y,
        M2X, M2Y,
        M3X, M3Y,
        M4X, M4Y,
        M5X, M5Y,
        M6X, M6Y,
        M7X, M7Y,
        X_MSB,
        CTRL1,
        RST,
        LPX, LPY,
        SPRT_ENABLE,
        CTRL2,
        SPRT_Y_EXP,
        MEM_PTRS,
        INT_REG, INT_ENABLE,
        SPRT_DATA_PRIO,
        SPRT_MULTICOLOR,
        SPRT_SPRT_COLLISION,
        SPRITE_DATA_COLLISION,
        EC,
        B0C, B1C, B2C, B3C,
        MM0, MM1,
        M0C, M1C, M2C, M3C, M4C, M5C, M6C, M7C
    };

    // The nitty-gritty details of the VIC II are covered here: http://www.zimmers.net/cbmpics/cbm/c64/vic-ii.txt
    // There are two NTSC revisions: the 6567R56A and the 6567R8.
    // I will focus on the behavior of the R56A for now.

    // Total lines to draw, including non-visible ones
    static const uint_fast16_t num_lines = 262;
    static const uint_fast8_t cycles_per_line = 64;

    Memory *memory_ = nullptr;
    ColorRam *cram_ = nullptr;

    // VIC II registers covered on https://www.c64-wiki.com/wiki/Page_208-211
    // There are 47 usable registers with the 27 other "registers" just returning 0xFF on read and ignore write
    uint8_t registers[47]{0};
    const uint8_t fake_reg = 0xFF;
    // Line to cause a raster interrupt on. Set by writing reg RST and bit 7 of CTRL1.
    uint_fast16_t raster_int_ = std::numeric_limits<uint_fast16_t>::max();

public:
    Vic() = default;

    void setRegister(uint_fast16_t reg_offset, uint8_t val);

    uint8_t getRegister(uint_fast16_t reg_offset);

    void setMemory(Memory *memory) {memory_ = memory;}
    void setCRAM(ColorRam *cram) {cram_ = cram;}

    void run();
};


#endif //C64_EMULATOR_VIC_H
