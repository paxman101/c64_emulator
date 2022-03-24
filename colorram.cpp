//
// Created by Paxton on 2022-03-23.
//

#include "colorram.h"

uint8_t ColorRam::getValue(uint_fast16_t offset) {
    if (offset >= 1000) {
        // let's assume that the unused addresses return 0xFF
        return 0xFF;
    }

    return memory[offset];
}

void ColorRam::setValue(uint_fast16_t offset, uint8_t val) {
    if (offset >= 1000) {
        return;
    }

    memory[offset] = val;
}
