#ifndef CHIP8_INSTRUCTION_SET_H
#define CHIP8_INSTRUCTION_SET_H

#include <cstdint>

#include "Chip8System.h"

namespace InstructionSet {

using Chip8InstructionFunctionPtr = void (Chip8System::*)(Instruction);

struct OpcodeFunction {
    std::uint16_t mask;
    std::uint8_t high_nibble;
    std::uint8_t low_byte{0x00};
    Chip8InstructionFunctionPtr execute;

    [[nodiscard]] constexpr bool matches(Instruction const instruction) const {
        constexpr std::uint16_t SHIFT{12};
        std::uint16_t const masked_value{static_cast<std::uint16_t>((high_nibble << SHIFT) | low_byte)};

        return (instruction.raw_data() & mask) == masked_value;
    }
};

constexpr OpcodeFunction opcode_standard(std::uint8_t const high,
                                            Chip8InstructionFunctionPtr const& execute) {
    constexpr std::uint16_t HIGH_NIBBLE_MASK{0xF000};
    return {.mask = HIGH_NIBBLE_MASK, .high_nibble = high, .execute = execute};
}

constexpr OpcodeFunction opcode_low_nibble(std::uint8_t const high, std::uint8_t const low,
                                                Chip8InstructionFunctionPtr const& execute) {
    constexpr std::uint16_t HIGH_AND_LOW_NIBBLE_MASK{0xF00F};
    return {.mask = HIGH_AND_LOW_NIBBLE_MASK,
            .high_nibble = high,
            .low_byte = low,
            .execute = execute};
}

constexpr OpcodeFunction opcode_low_byte(std::uint8_t const high,
                                                     std::uint8_t const byte,
                                                     Chip8InstructionFunctionPtr const& execute) {
    constexpr std::uint16_t HIGH_NIBBLE_AND_LOW_BYTE_MASK{0xF0FF};
    return {.mask = HIGH_NIBBLE_AND_LOW_BYTE_MASK,
            .high_nibble = high,
            .low_byte = byte,
            .execute = execute};
}
} // namespace InstructionSet

#endif // CHIP8_INSTRUCTION_SET_H
