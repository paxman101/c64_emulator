//
// Created by Paxton on 2022-03-23.
//

#include "cia.h"

void Cia1::setRegister(uint_fast16_t reg_offset, uint8_t val) {
    // Only the lower 4 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x10;

    registers[reg_index] = val;
}

uint8_t Cia1::getRegister(uint_fast16_t reg_offset) {
    // Only the lower 4 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x10;

    return registers[reg_index];
}

void Cia2::setRegister(uint_fast16_t reg_offset, uint8_t val) {
    // Only the lower 4 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x10;

    registers[reg_index] = val;
}

uint8_t Cia2::getRegister(uint_fast16_t reg_offset) {
    // Only the lower 4 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x10;

    return registers[reg_index];
}
