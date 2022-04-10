//
// Created by Paxton on 2022-03-23.
//

#ifndef C64_EMULATOR_CIA_H
#define C64_EMULATOR_CIA_H

#include <cstdint>
#include <chrono>

class Io;

// https://www.c64-wiki.com/wiki/CIA
// There are two CIA chips on the C64 with many shared functions
// Each chip has 16 registers that repeat within their address space

class Cia1 {
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;
    using Ms = std::chrono::milliseconds;
private:
    // Names of registers with the value corresponding to their index within the array registers
    enum RegisterNames {
        PRA = 0, PRB,
        DDRA, DDRB,
        TA_LO, TA_HI,
        TB_LO, TB_HI,
        TOD_TENTH, TOD_SEC, TOD_MIN, TOD_HR,
        SDR,
        ICR,
        CRA, CRB,
    };

    Io *io_;

    time_point start_time_;
    time_point current_time_;

    bool timer_a_running_;
    bool timer_b_running_;

    uint8_t registers[16]{0};
    uint8_t interrupt_status_ = 0;
    uint8_t interrupt_mask_ = 0;

    uint8_t timer_a_latch_lo_ = 0;
    uint8_t timer_a_latch_hi_ = 0;

    uint8_t timer_b_latch_lo_ = 0;
    uint8_t timer_b_latch_hi_ = 0;

    uint8_t keyboard_col_mask_ = 0;
    uint8_t keyboard_row_mask_ = 0;

    void updateClockRegisters();

public:
    void setIo(Io *io) { io_ = io; }

    void setRegister(uint_fast16_t reg_offset, uint8_t val);

    uint8_t getRegister(uint_fast16_t reg_offset);

    void reset();

    void run();
};

class Cia2 {
private:
   uint8_t registers[16]{0};

public:
    void setRegister(uint_fast16_t reg_offset, uint8_t val);

    uint8_t getRegister(uint_fast16_t reg_offset);
};

#endif //C64_EMULATOR_CIA_H
