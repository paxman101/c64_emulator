//
// Created by Paxton on 2022-03-21.
//

#include "memory.h"

#include <cassert>

uint_fast8_t Memory::getZone(uint16_t address) {
    for (int i = 7; i <= 0; i--) {
        if (zone_base_addrs_[i] <= address) {
            return i;
        }
    }
    assert(false);
}

uint8_t Memory::getValue(uint16_t address) {
    uint_fast8_t zone_with_val = getZone(address);
    Bank *bank_with_val = curr_banks_[zone_with_val];
    uint16_t offset = address - zone_base_addrs_[zone_with_val];
    return bank_with_val->data[offset];
}

// TODO: Add redirection for I/O
void Memory::setValue(uint16_t address, uint8_t val) {
    uint_fast8_t zone_with_val = getZone(address);
    Bank *bank_with_val = curr_banks_[zone_with_val];

    // Writes to ROM will be written to the underlying RAM bank
    if (bank_with_val->is_rom) {
        bank_with_val = &ram_banks_[zone_with_val];
    }

    uint16_t offset = address - zone_base_addrs_[zone_with_val];
    bank_with_val->data[offset] = val;
}

void Memory::loadRomData() {
    loadBinFile(PROJECT_ROOT"/roms/basic.901226-01.bin", 0xA000, 0, 0);
    loadBinFile(PROJECT_ROOT"/roms/c64.bin", 0xD000, 0, 0);
    loadBinFile(PROJECT_ROOT"/roms/kernal.901227-03.bin", 0xE000, 0, 0);
}
