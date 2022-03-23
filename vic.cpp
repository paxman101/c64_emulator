//
// Created by Paxton on 2022-03-20.
//

#include "vic.h"

void Vic::setRegister(uint_fast16_t reg_offset, uint8_t val) {
    // Only the lower 6 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x40;

    // Don't change fake registers
    if (reg_index >= 47) {
        return;
    }

    registers[reg_index] = val;
}

uint8_t Vic::getRegister(uint_fast16_t reg_offset) {
    // Only the lower 6 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x40;

    // Fake registers return the same value (0xFF)
    if (reg_index >= 47) {
        return fake_reg;
    }

    // Control register 2's bits 7-6 always read 1
    if (reg_index == 0x16) {
        return registers[reg_index] | 0xC0;
    }

    // Interrupt related register's bits 4-6 always read 1
    if (reg_index == 0x19 || reg_index == 0x1A) {
        return registers[reg_index] | 0x70;
    }

    // Color related register's upper nybble always read 1
    if (reg_index >= 0x20 && reg_index <= 0x2E) {
        return registers[reg_index] | 0xF0;
    }

    return registers[reg_index];
}
