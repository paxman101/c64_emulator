//
// Created by Paxton on 2022-03-20.
//

#include "vic.h"
#include "memory.h"
#include "cia.h"
#include "io.h"

uint8_t Vic::getMem(uint16_t address) {
    // Bits 0-1 in the first register of CIA 2 denotes the inverted 14-15 bits of the VIC's address lines.
    uint16_t base_addr = ~(cia2_->getRegister(0x0) & 0x3) << 14;
    // Now we may find the memory address wrt to the full 64 kB address space of the C64's RAM.
    uint16_t real_addr = base_addr + address;

    // Check if the address is within the mapped character generator ROM space.
    if (real_addr >= 0x1000 && real_addr < 0x2000) {
        return memory_->char_rom_.data[real_addr - 0x1000];
    }
    else if (real_addr >= 0x9000 && real_addr < 0xa000) {
        return memory_->char_rom_.data[real_addr - 0x9000];
    }

    // Otherwise, we are within the RAM banks.
    return memory_->ram_banks_[Memory::getZone(real_addr)].data[real_addr];
}

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
        registers_[reg_index] = val & 0x7F;
    }

    registers_[reg_index] = val;
}

uint8_t Vic::getRegister(uint_fast16_t reg_offset) {
    // Only the lower 6 address lines are read for registers
    uint_fast16_t reg_index = reg_offset % 0x40;

    // Fake registers return the same value (0xFF)
    if (reg_index >= 47) {
        return fake_reg_;
    }

    // Control register 2's bits 7-6 always read 1
    if (reg_index == 0x16) {
        return registers_[reg_index] | 0xC0;
    }

    // Interrupt related register's bits 4-6 always read 1
    if (reg_index == 0x19 || reg_index == 0x1A) {
        return registers_[reg_index] | 0x70;
    }

    // Color related register's upper nybble always read 1
    if (reg_index >= 0x20 && reg_index <= 0x2E) {
        return registers_[reg_index] | 0xF0;
    }

    return registers_[reg_index];
}

void Vic::updateStates() {
    current_cycle_++;
    current_x_ += 8;
    if (current_x_ + 16 >= max_x_coord) {
        current_x_ = 0;
    }

    // Check for bad line by the given condition from vic-ii.txt:
    //     A Bad Line Condition is given at any arbitrary clock cycle, if at the
    //     negative edge of ø0 at the beginning of the cycle RASTER >= $30 and RASTER
    //     <= $f7 and the lower three bits of RASTER are equal to YSCROLL and if the
    //     DEN bit was set during an arbitrary cycle of raster line $30.
    uint8_t yscroll = registers_[CTRL1] & 0x7;
    is_badline_ = current_raster_ >= 0x30 && current_raster_ <= 0xf7 && (current_raster_ & 0x7) == yscroll;

    if (current_cycle_ == 14) {
        vc_ = vc_base_;
        vmli_ = 0;
        if (is_badline_) {
            rc_ = 0;
        }
    }
    if (current_cycle_ == 58) {
       if (rc_ == 7) {
           is_display_state_ = false;
           vc_base_ = vc_;
       }
    }

    if (is_badline_) {
        is_display_state_ = true;
    }
    if (current_raster_ == 0) {
        vc_base_ = 0;
    }

    if (current_cycle_ == 58 && is_display_state_) {
        rc_++;
        if (rc_ > 7) {
            rc_ = 0;
        }
    }

    // bit 3 of control registers refer to rsel and csel respectively
    bool rsel = (registers_[CTRL1] & 0x08) >> 3;
    bool csel = (registers_[CTRL2] & 0x08) >> 3;

}

void Vic::incrementRaster() {
    current_raster_ = (registers_[CTRL1] >> 7) * 0x100 + registers_[RST];
    current_cycle_ = 0;
    current_x_ = first_x_coord;
    current_raster_++;

    // wrap around at end of lines.
    if (current_raster_ > num_lines) {
        current_raster_ = 0;
    }

    registers_[RST] = current_raster_ & 0xFF;
    registers_[CTRL1] = ((current_raster_ & 0x100) >> 1) | (registers_[CTRL1] & 0x7F);
}

void Vic::draw() {
    uint16_t char_addr = (registers_[MEM_PTRS] & 0xE) << 10;
    uint16_t screen_addr = (registers_[MEM_PTRS] & 0xF0) << 6;

    uint16_t g_addr = 0x3fff;
    if (is_display_state_) {
        uint16_t c_addr = vc_ + screen_addr;
        uint8_t c_val = getMem(c_addr);
        if (c_val != 0 && c_val != 0x20) {
//            printf("CVAL = %02X\n", c_val);
        }

        g_addr = rc_ + (c_val << 3) + char_addr;
        uint8_t g_val = getMem(g_addr);

        uint8_t cram_color = cram_->getValue(c_val) & 0x0F;
        uint8_t bg_color = registers_[B0C] & 0x0F;

        for (int i = 0; i < 8; i++) {
            bool pix_val = (g_val & (1 << i)) >> i;
            auto our_x = current_x_ + (8 - i);

            const uint8_t *colors = pix_val ? palette[cram_color] : palette[bg_color];
            auto screen_x = our_x < first_visible_x_coord ? (max_x_coord - first_visible_x_coord) + our_x : first_visible_x_coord - our_x;
            auto screen_y = current_raster_ > last_vblank_line ? (current_raster_ - last_vblank_line) : current_raster_ + last_vblank_line;
//            auto screen_y = current_raster_;
            io_->drawPixel(screen_x, screen_y, colors[0], colors[1], colors[2], 255);
        }

        vc_++;
        vmli_++;
    }

}

void Vic::run() {
    updateStates();

    // Vblank lines aren't drawn.
    if (current_raster_ < first_vblank_line || current_raster_ > last_vblank_line) {
        if (current_x_ >= 0x18 || current_x_ <= 0x14e) {
            draw();
        }
    }
    if (is_badline_ && current_cycle_ >= 14 && current_cycle_ <= 54) {
//        draw();
//        current_cycle_ += 40;
    }

    // increment raster if done with line
    if (current_cycle_ == cycles_per_line) {
        incrementRaster();
    }
    if (current_raster_ == first_vblank_line && current_cycle_ == 1) {
        io_->update();
        io_->clearScreen();
    }
}

