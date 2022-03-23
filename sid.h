//
// Created by Paxton on 2022-03-22.
//

#ifndef C64_EMULATOR_SID_H
#define C64_EMULATOR_SID_H

#include <cstdint>

class Sid {
private:
    // SID registers covered on https://www.c64-wiki.com/wiki/SID
    // There are 29 usable registers, the majority of them being write-only.
    // Like the VIC II, there is "mirroring" of the registers, this time every 0x20 bytes (according to https://sta.c64.org/cbm64mem.html).
    // I'm assuming that the 3 unusable bytes at offsets 0x1D-0x1F act the same as "fake" registers in the VIC II.
    uint8_t registers[29]{0};
    const uint8_t fake_reg = 0xFF;

public:
    void setRegister(uint_fast16_t reg_offset, uint8_t val);

    uint8_t getRegister(uint_fast16_t reg_offset);
};


#endif //C64_EMULATOR_SID_H
