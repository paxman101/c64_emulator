//
// Created by Paxton on 2022-03-22.
//

#include "sid.h"

void Sid::setRegister(uint_fast16_t reg_offset, uint8_t val) {
    // Only the lower 5 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x20;

    // Don't set unused or read-only registers
    if (reg_index >= 0x19) {
        return;
    }

    registers[reg_index] = val;
}

uint8_t Sid::getRegister(uint_fast16_t reg_offset) {
    // Only the lower 5 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x20;

    // Return fake value for unused or write-only registers
    if (reg_index <= 0x18 || reg_index >= 0x1D) {
        return fake_reg;
    }

    return registers[reg_index];
}
