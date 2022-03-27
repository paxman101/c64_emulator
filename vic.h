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
class Cia2;
class Io;

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
        SPRT_X_EXPANSION,
        SPRT_SPRT_COLLISION,
        SPRITE_DATA_COLLISION,
        EC,
        B0C, B1C, B2C, B3C,
        MM0, MM1,
        M0C, M1C, M2C, M3C, M4C, M5C, M6C, M7C
    };

    constexpr static const uint8_t palette[][3] = {{  0,   0,   0}, {255, 255, 255}, {136,   0,   0}, {170, 255, 238},
                                                   {204,  68, 204}, {  0, 204, 85}, {0, 0, 170}, {238, 238, 119},
                                                   {221, 136, 85}, {102, 68, 0}, {255, 119, 119}, {51, 51, 51},
                                                   {119, 119, 119}, {170, 255, 102}, {  0, 136, 255}, {187, 187, 187}};

    // The nitty-gritty details of the VIC II are covered here: http://www.zimmers.net/cbmpics/cbm/c64/vic-ii.txt
    // There are two NTSC revisions: the 6567R56A and the 6567R8.
    // I will focus on the behavior of the R56A for now.

    // Total lines to draw, including non-visible ones
    static const uint16_t num_lines = 262;
    static const uint8_t visible_lines = 234;
    static const uint8_t cycles_per_line = 64;
    static const uint8_t first_vblank_line = 13;
    static const uint8_t last_vblank_line = 40;
    // First x coord of a line
    static const uint16_t first_x_coord = 0x19c;
    static const uint16_t first_visible_x_coord = 0x1e8;
    static const uint16_t last_visible_x_coord = 0x184;
    static const uint16_t max_x_coord = 0x1ff;

    Memory *memory_ = nullptr;
    ColorRam *cram_ = nullptr;
    Cia2 *cia2_ = nullptr;
    Io *io_ = nullptr;

    uint16_t current_raster_ = 0;
    uint8_t current_cycle_ = 0;
    bool is_badline_ = false;

    // VIC II registers covered on https://www.c64-wiki.com/wiki/Page_208-211
    // There are 47 usable registers with the 27 other "registers" just returning 0xFF on read and ignore write
    uint8_t registers_[47]{0};

    // Special internal registers:
    const uint8_t fake_reg_ = 0xFF;
    // Line to cause a raster interrupt on. Set by writing reg RST and bit 7 of CTRL1.
    uint_fast16_t raster_int_ = std::numeric_limits<uint_fast16_t>::max();
    uint16_t current_x_ = 0;

    // Buffers
    uint16_t v_matrix_buf_[40] {0};
    uint32_t sprite_buf_[8] {0};

    // VC and RC
    uint_fast16_t vc_ = 0;
    uint_fast16_t vc_base_ = 0;
    uint_fast8_t  rc_ = 0;
    uint_fast8_t  vmli_ = 0;

    // display state when true; idle state when false
    bool is_display_state_ = false;

    // Get memory with respect to the VICs view of memory.
    // The VIC can only address 16 kB of memory by itself with the two other bits requried for adressing the full
    // 64 kB of memory being provided by the CIA 2.
    // The VIC also has the character generator ROM mapped in at 0x1000 in banks 0 and 2.
    uint8_t getMem(uint16_t address);

    void updateStates();
    void incrementRaster();
    void draw();
public:
    Vic() = default;

    void setRegister(uint_fast16_t reg_offset, uint8_t val);

    uint8_t getRegister(uint_fast16_t reg_offset);

    void setMemory(Memory *memory) { memory_ = memory; }
    void setCRAM(ColorRam *cram) { cram_ = cram; }
    void setCIA2(Cia2 *cia2) { cia2_ = cia2; }
    void setIo(Io *io) { io_ = io; }

    void run();
};


#endif //C64_EMULATOR_VIC_H
