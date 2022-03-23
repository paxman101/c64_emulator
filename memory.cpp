//
// Created by Paxton on 2022-03-21.
//

#include "memory.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <iterator>

uint_fast8_t Memory::getZone(uint16_t address) {
    for (int i = 6; i >= 0; i--) {
        if (zone_base_addrs_[i] <= address) {
            return i;
        }
    }
    assert(false);
}

void Memory::loadRomData(Bank *rom, const char *path) {
    std::ifstream in_stream {path, std::ios::binary};

    if (!in_stream.is_open()) {
        std::perror("Memory::loadRomData");
    }

    uint_fast16_t offset = 0x0000;
    for (auto iter = std::istreambuf_iterator<char>{in_stream}; iter != std::istreambuf_iterator<char>{}; ++iter) {
        rom->data[offset++] = *iter;
    }
}

Memory::Memory(Vic &vic) : vic_{vic} {
    loadRomData(&basic_rom_, PROJECT_ROOT"/roms/basic.901226-01.bin");
    loadRomData(&char_rom_, PROJECT_ROOT"/roms/c64.bin");
    loadRomData(&kernal_rom_, PROJECT_ROOT"/roms/kernal.901227-03.bin");
}

uint8_t Memory::getValue(uint16_t address) {
    uint_fast8_t zone_with_val = getZone(address);
    Bank *bank_with_val = curr_banks_[zone_with_val];
    uint16_t offset = address - zone_base_addrs_[zone_with_val];

    // Forward I/O request to the correct device
    if (bank_with_val == &dummy_io_bank_) {
        // VIC II register location
        if (offset < 0x400) {
            return vic_.getRegister(offset);
        }
    }

    return bank_with_val->data[offset];
}

// TODO: Finish redirection for I/O
void Memory::setValue(uint16_t address, uint8_t val) {
    uint_fast8_t zone_with_val = getZone(address);
    Bank *bank_with_val = curr_banks_[zone_with_val];
    uint16_t offset = address - zone_base_addrs_[zone_with_val];

    // Writes to ROM will be written to the underlying RAM bank
    if (bank_with_val->is_rom) {
        bank_with_val = &ram_banks_[zone_with_val];
    }

    // Forward I/O request to the correct device
    if (bank_with_val == &dummy_io_bank_) {
        // VIC II register location
        if (offset < 0x400) {
            vic_.setRegister(offset, val);
        }
        return;
    }

    bank_with_val->data[offset] = val;
}
