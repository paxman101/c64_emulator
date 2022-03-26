//
// Created by Paxton on 2022-03-20.
//

#include "vic.h"
#include "memory.h"

void Vic::setRegister(uint_fast16_t reg_offset, uint8_t val) {
    // Only the lower 6 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x40;

    // Don't change fake registers
    if (reg_index >= 47) {
        return;
    }

    // Writes to RST set the interrupt line
    if (reg_index == RST) {
        raster_int_ = val;
        return;
    }
    if (reg_index == CTRL1) {
        raster_int_ &= val >> 7;
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

void Vic::run() {
    static unsigned int cycle_count = 0;
    cycle_count++;

    // bit 3 of control registers refer to rsel and csel respectively
    bool rsel = (registers[CTRL1] & 0x08) >> 3;
    bool csel = (registers[CTRL2] & 0x08) >> 3;

    // raster line is defined by the raster register and bit 7 of the control 1 register
    unsigned int raster = (registers[CTRL1] >> 7) * 0x100 + registers[RST];

    // increment raster if done
    if (cycle_count == cycles_per_line) {
        cycle_count = 0;
        raster++;

        // wrap around at end of lines.
        if (raster > num_lines) {
            raster = 0;
        }

        registers[RST] = raster & 0xFF;
        registers[CTRL1] = ((raster & 0x100) >> 1) | (registers[CTRL1] & 0x7F);
    }
}
