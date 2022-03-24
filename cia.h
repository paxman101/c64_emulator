//
// Created by Paxton on 2022-03-23.
//

#ifndef C64_EMULATOR_CIA_H
#define C64_EMULATOR_CIA_H

#include <cstdint>

// https://www.c64-wiki.com/wiki/CIA
// There are two CIA chips on the C64 with many shared functions
// Each chip has 16 registers that repeat within their adress space

class Cia1 {
private:
    uint8_t registers[16];

public:
    void setRegister(uint_fast16_t reg_offset, uint8_t val);

    uint8_t getRegister(uint_fast16_t reg_offset);
};

class Cia2 {
private:
   uint8_t registers[16];

public:
    void setRegister(uint_fast16_t reg_offset, uint8_t val);

    uint8_t getRegister(uint_fast16_t reg_offset);
};

#endif //C64_EMULATOR_CIA_H
