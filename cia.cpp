//
// Created by Paxton on 2022-03-23.
//

#include "cia.h"

#include "io.h"

extern "C" {
#include "cpu.h"
}

void Cia1::updateClockRegisters() {
    uint32_t ms_since_start = std::chrono::duration_cast<Ms>(current_time_ - start_time_).count();

    // Times are stored as BCD numbers
    // Tenth seconds are stored in bits 0-3; the other bits are 0
    registers[TOD_TENTH] = (ms_since_start / 100) % 10;


    // Seconds in bits 0-3, ten seconds in 4-6, bit 7 is 0
    registers[TOD_SEC] = ((ms_since_start / 10000 % 10) << 4) + (ms_since_start / 1000) % 10;

    uint32_t minutes_since_start = ms_since_start / 1000 / 60;
    // Minutes in bits 0-3, ten minutes in 4-6, bit 7 is 0
    registers[TOD_MIN] = ((minutes_since_start / 10 % 10) << 4) + (minutes_since_start % 10);

}

void Cia1::setRegister(uint_fast16_t reg_offset, uint8_t val) {
    // Only the lower 4 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x10;

    switch(reg_offset) {
        case PRA:
            keyboard_col_mask_ = val;
            break;
        case PRB:
            keyboard_row_mask_ = val;
            break;
        case TA_LO:
            timer_a_latch_lo_ = val;
            break;
        case TA_HI:
            timer_a_latch_hi_ = val;
            break;
        case TB_LO:
            timer_b_latch_lo_ = val;
            break;
        case TB_HI:
            timer_b_latch_hi_ = val;
            break;
        case ICR:
            // check if we are clearing or setting mask bytes. 1 = set bit, 0 = clear bit.
            if (val >> 7) {
                interrupt_mask_ |= val & 0x1F;
            }
            else {
                interrupt_mask_ &= ~(val & 0x1F);
            }
            break;
        case CRA:
            timer_a_running_ = val & 0x1;
            // Bit 4 of CRA loads latch into timer
            if ((val & 0x10) >> 4) {
                registers[TA_LO] = timer_a_latch_lo_;
                registers[TA_HI] = timer_a_latch_hi_;
            }
            break;
        case CRB:
            timer_b_running_ = val & 0x1;
            if ((val & 0x10) >> 4) {
                registers[TB_LO] = timer_b_latch_lo_;
                registers[TB_HI] = timer_b_latch_hi_;
            }
            break;
        default:
            registers[reg_index] = val;
    }
}

uint8_t Cia1::getRegister(uint_fast16_t reg_offset) {
    // Only the lower 4 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x10;

    switch (reg_offset) {
        // Reading keyboard: https://paulnotebook.net/tag/keyboard-matrix/
        case PRA: {
            auto pair = io_->getKeyboardState();
            printf("vals: %02X %02X\n", pair.first, pair.second);
            return pair.second ^ keyboard_row_mask_;

        }
        case PRB: {
//            auto pair = io_->getKeyboardState();
//            // If they share any 0 bits
//            uint8_t inv_pair = ~pair.first;
//            uint8_t inv_col_mask = ~keyboard_col_mask_;
//            if (inv_pair & inv_col_mask) {
//
//                return pair.second;
//            }
//            else {
//                return 0xFF;
//            }
            return io_->getKeysByColumn(keyboard_col_mask_);
        }
        // Reading ICR returns the interrupt status bits and also clears them
        case ICR: {
            uint8_t prev_interrupt_status = interrupt_status_;
            interrupt_status_ = 0;
            return prev_interrupt_status;
        }
        default:
            return registers[reg_index];
    }
}

void Cia1::reset() {
    start_time_ = clock::now();
}

void Cia1::run() {
    // If start_time_ is uninitialized, run reset().
    current_time_ = clock::now();
    updateClockRegisters();

    // Run timer
    if (timer_a_running_) {
        uint16_t timer_val = (registers[TA_HI] << 8) + registers[TA_LO];
        uint16_t temp = timer_val;
        timer_val--;
        // check for "underflow" (it's really overflow)
        if (timer_val > temp) {
            interrupt_status_ |= 0x1;
            // CRA if bit 3 = 1, then stop timer. Else reload from latch.
            if ((registers[CRA] & 0x4) >> 3) {
                timer_a_running_ = false;
            }
            else {
                timer_val = (timer_a_latch_hi_ << 8) + timer_a_latch_lo_;
            }
        }
        registers[TA_LO] = timer_val & 0xFF;
        registers[TA_HI] = timer_val >> 8;
    }
    if (timer_b_running_) {
        uint16_t timer_val = (registers[TB_HI] << 8) + registers[TB_LO];
        uint16_t temp = timer_val;
        timer_val--;
        // check for "underflow" (it's really overflow)
        if (timer_val > temp) {
            interrupt_status_ |= 0x2;
            // CRB if bit 3 = 1, then stop timer. Else reload from latch.
            if ((registers[CRB] & 0x4) >> 3) {
                timer_a_running_ = false;
            }
            else {
                timer_val = (timer_b_latch_hi_ << 8) + timer_b_latch_lo_;
            }
        }
        registers[TB_LO] = timer_val & 0xFF;
        registers[TB_HI] = timer_val >> 8;
    }
    // cause interrupt if needed
    if (interrupt_mask_ & interrupt_status_) {
        triggerInterrupt(INT_IRQ);
    }
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
