//
// Created by Paxton on 2022-03-23.
//

#ifndef C64_EMULATOR_COLORRAM_H
#define C64_EMULATOR_COLORRAM_H

#include <cstdint>

class ColorRam {
private:
    // https://www.c64-wiki.com/wiki/Color_RAM
    // 1000 nibbles of data. Apparently the upper nybbles when reading these bytes are random.
    uint8_t memory[1000];

public:
    uint8_t getValue(uint_fast16_t offset);
    void setValue(uint_fast16_t offset, uint8_t val);
};


#endif //C64_EMULATOR_COLORRAM_H
