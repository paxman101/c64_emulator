//
// Created by Paxton on 2022-03-20.
//

#ifndef C64_EMULATOR_VIC_H
#define C64_EMULATOR_VIC_H

#include <cstdint>

class Vic {
private:
    // VIC II registers covered on https://www.c64-wiki.com/wiki/Page_208-211
    // There are 47 usable registers with the 27 other "registers" just returning 0xFF on read and ignore write
    uint8_t registers[47]{0};
    const uint8_t fake_reg = 0xFF;

public:
    void setRegister(uint_fast16_t reg_offset, uint8_t val);

    uint8_t getRegister(uint_fast16_t reg_offset);
};


#endif //C64_EMULATOR_VIC_H
