#include "emulator.h"

#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <iterator>
#include <print>
#include <sstream>
#include <stdexcept>

#include "config.h"
#include "fonts.h"
#include "instruction.h"
#include "instruction_set.h"
#include "system.h"

namespace Chip8 {
Emulator::Emulator() {
    Config::load_super_chip();

    system.display.reserve(static_cast<size_t>(Config::max_width * Config::max_height));
    // Size to base size
    system.display.resize(System::LORES_WIDTH * System::LORES_HEIGHT);
    // read fonts in
    std::ranges::copy(FONT, std::begin(system.memory));
    std::ranges::copy(BIG_FONT, std::begin(system.memory) + FONT.size());
}

/** @brief Primary instruction decoding and execution function. */
void Emulator::decodeInstruction(Instruction const instruction) {
    for (auto const& row : InstructionSet::DECODE_TABLE) {
        if (row.matches(instruction)) {
            (system.*row.execute)(instruction);
            return;
        }
    }

    std::println(stderr, "Error: Invalid (or unimplemented) CHIP-8 instruction: 0x{:04X}",
                 instruction.raw_data());
}

void Emulator::loadRom(std::string_view filename) {
    constexpr std::uint16_t START_IDX{0x200};

    if (std::ifstream file{filename.data(), std::ios::binary}) {
        std::println("Loading rom from path: {}", filename);
        std::ostringstream buffer{};
        buffer << file.rdbuf();
        std::string rom{buffer.str()};

        std::ranges::copy(rom, std::begin(system.memory) + START_IDX);
    } else {
        throw std::runtime_error(std::string("Error: file read went wrong"));
    }

    system.program_counter = START_IDX;
}

bool Emulator::updateTimers() noexcept {
    if (system.delay_timer > 0) {
        system.delay_timer--;
    }
    if (system.sound_timer > 0) {
        system.sound_timer--;
    }
    return system.sound_timer > 0;
}

Instruction Emulator::getCurrentInstruction() const {
    return Instruction{system.memory.at(system.program_counter),
                       system.memory.at(system.program_counter + 1)};
}

void Emulator::cycle() {
    Instruction const instruction{getCurrentInstruction()};

    system.program_counter += 2;

    decodeInstruction(instruction);
}
} // namespace Chip8