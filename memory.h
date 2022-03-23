//
// Created by Paxton on 2022-03-21.
//

#ifndef C64_EMULATOR_MEMORY_H
#define C64_EMULATOR_MEMORY_H

#include <array>
#include <unordered_map>

#include "6510_Emulator/memory.h"

class Memory {
private:
    // The base addresses of each bank or zone area within memory.
    constexpr static const uint16_t zone_base_addrs_[] = {0x0000, 0x1000, 0x8000, 0xA000, 0xC000, 0xD000, 0xE000};
    // The size of each bank area or zone
    constexpr static const uint16_t zone_sizes_[] = {0x1000, 0x7000, 0x2000, 0x2000, 0x1000, 0x1000, 0x8000};

    struct Bank {
        uint8_t *data;
        uint_fast8_t zone;
        bool is_rom;

        Bank() = default;
        explicit Bank(uint_fast8_t zone, bool is_rom = false) : data{new uint8_t[zone_sizes_[zone]]}, zone{zone},
                                                                is_rom{is_rom} {};

        ~Bank() { delete[] data; }

        Bank(const Bank &) = delete;
    };

    // Depending on the PLA latch bit states, there are the following possible bank configurations
    // (https://www.c64-wiki.com/wiki/Bank_Switching):
    // Zone 0 (0x0000-0x0FFF): Always RAM
    // Zone 1 (0x1000-0x7FFF): RAM or unmapped
    // Zone 2 (0x8000-0x9FFF): RAM or cartridge ROM
    // Zone 3 (0xA000-0xBFFF): RAM, BASIC ROM, cartridge ROM, or unmapped
    // Zone 4 (0xC000-0xCFFF): RAM or unmapped
    // Zone 5 (0xD000-0xDFFF): RAM, character generator ROM, or I/O registers and color RAM
    // Zone 6 (0xE000-0xFFFF): RAM, KERNAL ROM, or cartridge rom

    // These underlying memory banks (appear) to always be written to except for I/O
    std::array<Bank, 7> ram_banks_ = {Bank{0}, Bank{1}, Bank{2}, Bank{3}, Bank{4}, Bank{5}, Bank{6}};

    // ROM banks:
    Bank basic_rom_{3, true};
    Bank char_rom_{5, true};
    Bank kernal_rom_{6, true};

    // A dummy Bank object used to represent the I/O mapped from 0xD000-0xDFFF
    Bank dummy_io_bank_{};

    // The current banks loaded initialized with the default mode (31)
    std::array<Bank *, 7> curr_banks_ = {&ram_banks_[0], &ram_banks_[1], &ram_banks_[2], &basic_rom_, &ram_banks_[4],
                                         &dummy_io_bank_, &kernal_rom_};

    // Get the zone number/index that where an address maps to
    static inline uint_fast8_t getZone(uint16_t addr);

public:

    // Get memory at address in the currently switched banks
    uint8_t getValue(uint16_t address);

    // Set memory at address in the currently switched banks
    void setValue(uint16_t address, uint8_t val);

    static void loadRomData();
};


#endif //C64_EMULATOR_MEMORY_H
