#include "system.h"

#include <print>

#include "config.h"
#include "fonts.h"
#include "instruction.h"

namespace Chip8 {
void System::sc_down(Instruction const instruction) noexcept {
    std::uint16_t const scroll_n{static_cast<std::uint16_t>(current_width * instruction.n())};
    // Remove pixels scrolled down off the bottom of the screen
    display.erase(display.end() - scroll_n, display.end());
    // Insert blank pixels at start of vector, pushing the display down
    display.insert(display.begin(), scroll_n, 0x0);
}

void System::cls(Instruction const instruction) noexcept { std::ranges::fill(display, 0x0); }

void System::ret(Instruction const instruction) noexcept {
    program_counter = stack.top();
    stack.pop();
}

void System::sc_right(Instruction const instruction) noexcept {
    auto iter{display.begin()};
    // Iterate row by row, performing scroll
    while (iter != display.end()) {
        // Remove pixels scrolled right off the row
        display.erase(iter + current_width - 4, iter + current_width);
        // Insert blank pixels at the start of the row, pushing the display right
        display.insert(iter, 4, 0x0);

        if (std::distance(iter, display.end()) > current_width) {
            std::advance(iter, current_width);
        } else {
            iter = display.end();
        }
    }
}
void System::sc_left(Instruction const instruction) noexcept {
    auto iter{display.begin()};
    // Iterate row by row, performing scroll
    while (iter != display.end()) {
        // Remove pixels scrolled left off the row
        display.erase(iter, iter + 4);
        // Insert blank pixels at the end of the row, pushing the display left
        display.insert(iter + current_width - 4, 4, 0x0);

        if (std::distance(iter, display.end()) > current_width) {
            std::advance(iter, current_width);
        } else {
            iter = display.end();
        }
    }
}
void System::exit(Instruction const instruction) noexcept {
    if (callback_function) {
        callback_function(CallbackType::CHIP8_CALLBACK_EXIT);
    } else {
        std::println(
            stderr,
            "Warning: callback not set: SUPER-CHIP exit instruction needs to modify window state");
    }
}
void System::lores(Instruction const instruction) noexcept {
    if (callback_function) {
        callback_function(CallbackType::CHIP8_CALLBACK_LORES);
    } else {
        std::println(
            stderr,
            "Warning: callback not set: SUPER-CHIP lores instruction needs to modify window state");
    }

    current_width = LORES_WIDTH;
    current_height = LORES_HEIGHT;
    display.resize(static_cast<size_t>(current_width * current_height));
}

void System::hires(Instruction const instruction) noexcept {
    if (callback_function) {
        callback_function(CallbackType::CHIP8_CALLBACK_HIRES);
    } else {
        std::println(
            stderr,
            "Warning: callback not set: SUPER-CHIP hires instruction needs to modify window state");
    }

    current_width = HIRES_WIDTH;
    current_height = HIRES_HEIGHT;
    display.resize(static_cast<size_t>(current_width * current_height));
}

void System::jmp(Instruction const instruction) noexcept { program_counter = instruction.nnn(); }

void System::call(Instruction const instruction) noexcept {
    stack.push(program_counter);
    program_counter = instruction.nnn();
}

void System::seq_vx_nn(Instruction const instruction) noexcept {
    if (registers.at(instruction.x()) == instruction.nn()) {
        program_counter += 2;
    }
}

void System::sne_vx_nn(Instruction const instruction) noexcept {
    if (registers.at(instruction.x()) != instruction.nn()) {
        program_counter += 2;
    }
}

void System::seq_vx_vy(Instruction const instruction) noexcept {
    if (registers.at(instruction.x()) == registers.at(instruction.y())) {
        program_counter += 2;
    }
}

void System::mov_vx_nn(Instruction const instruction) noexcept {
    registers.at(instruction.x()) = instruction.nn();
}

void System::add_vx_nn(Instruction const instruction) noexcept {
    registers.at(instruction.x()) += instruction.nn();
}

void System::mov_vx_vy(Instruction const instruction) noexcept {
    registers.at(instruction.x()) = registers.at(instruction.y());
}

void System::or_vx_vy(Instruction const instruction) noexcept {
    registers.at(instruction.x()) |= registers.at(instruction.y());

    // quirk
    registers.at(FLAG_REGISTER_IDX) = 0;
}

void System::and_vx_vy(Instruction const instruction) noexcept {
    registers.at(instruction.x()) &= registers.at(instruction.y());

    // quirk
    registers.at(FLAG_REGISTER_IDX) = 0;
}

void System::xor_vx_vy(Instruction const instruction) noexcept {
    registers.at(instruction.x()) ^= registers.at(instruction.y());

    // quirk
    registers.at(FLAG_REGISTER_IDX) = 0;
}

void System::add_vx_vy(Instruction const instruction) noexcept {
    std::uint8_t const register_x{registers.at(instruction.x())};
    std::uint8_t const register_y{registers.at(instruction.y())};

    registers.at(instruction.x()) = register_x + register_y;

    // Set flag register to the carry value
    registers.at(FLAG_REGISTER_IDX) = (register_x > 255 - register_y) ? 1 : 0;
}

void System::sub_vx_vy(Instruction const instruction) noexcept {
    std::uint8_t const register_x{registers.at(instruction.x())};
    std::uint8_t const register_y{registers.at(instruction.y())};

    registers.at(instruction.x()) = register_x - register_y;

    // Set flag register to the borrow value
    registers.at(FLAG_REGISTER_IDX) = (register_x >= register_y) ? 1 : 0;
}
void System::shr_vx_vy(Instruction const instruction) noexcept {
    if (!Config::shift_quirk) {
        registers.at(instruction.x()) = registers.at(instruction.y());
    }

    std::uint8_t const register_x{registers.at(instruction.x())};

    registers.at(instruction.x()) >>= 1; // Perform shift
    registers.at(FLAG_REGISTER_IDX) = (register_x & 0b00000001);
}

void System::rsb_vx_vy(Instruction const instruction) noexcept {
    std::uint8_t const register_x{registers.at(instruction.x())};
    std::uint8_t const register_y{registers.at(instruction.y())};

    registers.at(instruction.x()) = register_y - register_x;

    // Set flag register to the borrow value
    registers.at(FLAG_REGISTER_IDX) = (register_y >= register_x) ? 1 : 0;
}

void System::shl_vx_vy(Instruction const instruction) noexcept {
    if (!Config::shift_quirk) {
        registers.at(instruction.x()) = registers.at(instruction.y());
    }

    std::uint8_t const register_x{registers.at(instruction.x())};

    registers.at(instruction.x()) <<= 1;               // Perform shift
    registers.at(FLAG_REGISTER_IDX) = register_x >> 7; // Set flag register to the bit shifted out
}

void System::sne_vx_vy(Instruction const instruction) noexcept {
    if (registers.at(instruction.x()) != registers.at(instruction.y())) {
        program_counter += 2;
    }
}

void System::mov_i_nnn(Instruction const instruction) noexcept {
    index_register = instruction.nnn();
}
void System::jmp_vx_nnn(Instruction const instruction) noexcept {
    if (Config::jump_quirk) {
        program_counter = instruction.nnn() + registers.at(instruction.x());
    } else {
        program_counter = instruction.nnn() + registers.at(0x0);
    }
}

void System::rnd_vx_nn(Instruction const instruction) noexcept {
    // registers.at(instruction.x()) = dist(rng) & instruction.nn();
}

void System::drw(Instruction const instruction) noexcept {
    std::uint8_t const register_x{
        static_cast<std::uint8_t>(registers.at(instruction.x()) % current_width)};
    std::uint8_t const register_y{
        static_cast<std::uint8_t>(registers.at(instruction.y()) % current_height)};

    registers.at(FLAG_REGISTER_IDX) = 0;

    bool const is_super_chip{instruction.n() == 0};

    std::uint8_t const n_col{static_cast<uint8_t>(is_super_chip ? 16 : 8)};
    std::uint8_t const n_row{static_cast<uint8_t>(is_super_chip ? 16 : instruction.n())};

    for (std::uint8_t row = 0; row < n_row && register_y + row < current_height; row++) {
        for (std::uint8_t col = 0; col < n_col && register_x + col < current_width; col++) {
            std::uint8_t byte{0};
            if (is_super_chip) {
                byte = memory.at(index_register + (2 * row) + (col >= 8 ? 1 : 0));
            } else {
                byte = memory.at(index_register + row);
            }

            // Extract current pixel from byte using bit shift and binary and
            std::uint8_t const pixel{
                static_cast<std::uint8_t>(byte & (0b10000000 >> (col >= 8 ? (col & 0x7) : col)))};
            if (pixel != 0) {
                std::uint16_t const idx{static_cast<std::uint16_t>(
                    (register_y + row) * current_width + (register_x + col))};
                if (display.at(idx) == 0x01) {
                    registers.at(FLAG_REGISTER_IDX) = 1;
                }
                display.at(idx) ^= 1;
            }
        }
    }
}

void System::spr_vx(Instruction const instruction) noexcept {
    std::uint8_t const register_x{registers.at(instruction.x())};

    if (register_x < keys.size() && keys.at(register_x) == 0x1) {
        program_counter += 2;
    }
}

void System::sup_vx(Instruction const instruction) noexcept {
    std::uint8_t const register_x{registers.at(instruction.x())};

    if (register_x < keys.size() && keys.at(register_x) == 0x0) {
        program_counter += 2;
    }
}

void System::mov_vx_dt(Instruction const instruction) noexcept {
    registers.at(instruction.x()) = delay_timer;
}

void System::wait_mov_vx_key(Instruction const instruction) noexcept {
    if (key_released == 0xFF) {
        waiting = true;
        program_counter -= 2;
    } else {
        registers.at(instruction.x()) = key_released;
        waiting = false;
        key_released = 0xFF;
    }
}

void System::mov_dt_vx(Instruction const instruction) noexcept {
    delay_timer = registers.at(instruction.x());
}

void System::mov_st_vx(Instruction const instruction) noexcept {
    sound_timer = registers.at(instruction.x());
}

void System::add_i_vx(Instruction const instruction) noexcept {
    index_register += registers.at(instruction.x());
}

void System::mov_i_font_vx(Instruction const instruction) noexcept {
    // 5 bytes per character
    index_register = (registers.at(instruction.x()) * 0x5);
}

void System::mov_i_bfont_vx(Instruction const instruction) noexcept {
    // A bytes per character, plus offset from FONT size
    index_register = (registers.at(instruction.x()) * 0xA) + FONT.size();
}

void System::mov_i_bcd_vx(Instruction const instruction) noexcept {
    memory.at(index_register) = registers.at(instruction.x()) / 100;
    memory.at(index_register + 1) = (registers.at(instruction.x()) / 10) % 10;
    memory.at(index_register + 2) = registers.at(instruction.x()) % 10;
}

void System::mov_i_vx(Instruction const instruction) noexcept {
    for (size_t idx{0}; idx <= instruction.x(); idx++) {
        memory.at(index_register + idx) = registers.at(idx);
    }

    if (!Config::memory_quirk) {
        index_register = instruction.x() + 1;
    }
}

void System::mov_vx_i(Instruction const instruction) noexcept {
    for (size_t idx{0}; idx <= instruction.x(); idx++) {
        registers.at(idx) = memory.at(index_register + idx);
    }

    if (!Config::memory_quirk) {
        index_register = instruction.x() + 1;
    }
}
} // namespace Chip8