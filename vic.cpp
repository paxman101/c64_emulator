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
}

void Vic::updateBorderStates(uint16_t x) {
    // bit 3 of control registers refer to rsel and csel respectively
    bool rsel = (registers_[CTRL1] & 0x08) >> 3;
    bool csel = (registers_[CTRL2] & 0x08) >> 3;

    bool den = (registers_[CTRL1] & 0x10) >> 4;

    // border unit comparisons
    uint16_t left_compare = csel ? 0x18 : 0x1f;
    uint16_t right_compare = csel ? 0x158 : 0x14f;
    uint16_t top_compare = rsel ? 0x33 : 0x37;
    uint16_t bottom_compare = rsel ? 0xfb : 0xf7;
    // When border unit flip flops are switched:
    // 1. If the X coordinate reaches the right comparison value, the main border
    //    flip flop is set.
    if (x == right_compare) {
        main_border_ff_ = true;
    }
    if (current_cycle_ == 63) {
        // 2. If the Y coordinate reaches the bottom comparison value in cycle 63, the
        //    vertical border flip flop is set.
        if (current_raster_ == bottom_compare) {
            vertical_border_ff_ = true;
        }
        // 3. If the Y coordinate reaches the top comparison value in cycle 63 and the
        //    DEN bit in register $d011 is set, the vertical border flip flop is
        //    reset.
        else if (current_raster_ == top_compare && den) {
            vertical_border_ff_ = false;
        }
    }
    if (x == left_compare) {
        // 4. If the X coordinate reaches the left comparison value and the Y
        //    coordinate reaches the bottom one, the vertical border flip flop is set.
        if (current_raster_ == bottom_compare) {
            vertical_border_ff_ = true;
        }
        // 5. If the X coordinate reaches the left comparison value and the Y
        //    coordinate reaches the top one and the DEN bit in register $d011 is set,
        //    the vertical border flip flop is reset.
        else if (current_raster_ == top_compare && den) {
            vertical_border_ff_ = false;
        }
        // 6. If the X coordinate reaches the left comparison value and the vertical
        //   border flip flop is not set, the main flip flop is reset.
        if (!vertical_border_ff_) {
            main_border_ff_ = false;
        }
    }
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

    const uint8_t *bg_color_vals = palette[registers_[B0C] & 0x0F];
    uint16_t screen_y = current_raster_ > last_vblank_line ? (current_raster_ - last_vblank_line) : current_raster_ + (first_vblank_line - last_vblank_line);
    const uint8_t *colors;

    // TODO: Figure out what's causing us to need the -1
    uint16_t c_addr = vc_+ -1 + screen_addr;
    uint8_t c_val = getMem(c_addr);

    uint16_t g_addr = is_display_state_ ?  rc_ + (c_val << 3) + char_addr : 0x3fff;
    uint8_t g_val = getMem(g_addr);

    // If we are in idle, the foreground color will be black.
    uint8_t cram_color = is_display_state_ ? cram_->getValue(c_val) & 0x0F : 0;

    for (int i = 7; i >= 0; i--) {
        uint16_t our_x = current_x_ + (8 - i);

        // Don't render pixels outside the visible range
        if (our_x < first_visible_x_coord && our_x > last_visible_x_coord) {
            break;
        }

        // Check if we should draw border. Border preempts all other layers.
        updateBorderStates(our_x);
        if (main_border_ff_) {
            colors = palette[registers_[EC] & 0xF];
        }
        else {
            bool pix_val = (g_val & (1 << i)) >> i;
            colors = pix_val ? palette[cram_color] : bg_color_vals;
        }

        uint16_t screen_x =
                our_x >= first_visible_x_coord ? our_x - first_visible_x_coord : (max_x_coord - first_visible_x_coord) +
                                                                                 our_x;

        io_->drawPixel(screen_x, screen_y, colors[0], colors[1], colors[2], 255);
    }
    // vc_ and vmli_ are incremented after g-accesses in display state.
    if (is_display_state_ && !main_border_ff_) {
        vc_++;
        vmli_++;
    }

}

void Vic::run() {
    updateStates();

    // Vblank lines aren't drawn.
    if (current_raster_ < first_vblank_line || current_raster_ > last_vblank_line) {
        draw();
    }
    if (is_badline_ && current_cycle_ >= 14 && current_cycle_ <= 54) {
//        draw();
//        current_cycle_ += 40;
    }

    current_x_ += 8;
    if (current_x_ >= max_x_coord) {
        current_x_ = 0;
    }
    updateBorderStates(current_x_);

    // increment raster if done with line
    if (current_cycle_ == cycles_per_line) {
//        printf("VC:%4d VCBASE:%4d \n", vc_, vc_base_);
        incrementRaster();
    }
    if (current_raster_ == first_vblank_line && current_cycle_ == 1) {
        io_->update();
        io_->clearScreen();
    }
}

