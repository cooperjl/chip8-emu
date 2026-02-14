#ifndef CHIP8_INSTRUCTION_SET_H
#define CHIP8_INSTRUCTION_SET_H

#include <cstdint>

#include "system.h"

namespace Chip8::InstructionSet {
using InstructionFunctionPtr = void (System::*)(Instruction);

/**
 * @brief Struct which defines a mapping of opcodes to instruction functions to execute. CHIP-8
 * opcodes are a form of "composite" opcodes, so to speak, meaning instructions are differentiated
 * by a combination of specific nibbles. All instructions consider the high nibble, and some
 * rely on different combinations of nibbles in their low byte, which is mapped using the mask.
 */
struct OpcodeFunction {
    std::uint16_t mask;
    std::uint8_t high_nibble;
    std::uint8_t low_byte;
    InstructionFunctionPtr execute;

    [[nodiscard]] constexpr bool matches(Instruction const instruction) const {
        constexpr std::uint16_t SHIFT{12};
        std::uint16_t const masked_value{
            static_cast<std::uint16_t>((high_nibble << SHIFT) | low_byte)};

        return (instruction.raw_data() & mask) == masked_value;
    }
};

/**
 * @brief Factory function which generates OpcodeFunction structs for opcodes which rely on only
 * the high nibble, which have no additional nibbles or bytes composing their opcode.
 */
constexpr OpcodeFunction opcode_standard(std::uint8_t const high,
                                         InstructionFunctionPtr const& execute) {
    constexpr std::uint16_t HIGH_NIBBLE_MASK{0xF000};

    return {.mask = HIGH_NIBBLE_MASK,
            .high_nibble = high,
            .low_byte = 0x00, // low byte is masked out for this type of opcode, so will be 0x00
            .execute = execute};
}

/**
 * @brief Factory function which generates OpcodeFunction structs for opcodes which additionally
 * rely on the low nibble.
 */
constexpr OpcodeFunction opcode_low_nibble(std::uint8_t const high_nibble,
                                           std::uint8_t const low_nibble,
                                           InstructionFunctionPtr const& execute) {
    constexpr std::uint16_t HIGH_AND_LOW_NIBBLE_MASK{0xF00F};

    return {.mask = HIGH_AND_LOW_NIBBLE_MASK,
            .high_nibble = high_nibble,
            .low_byte = low_nibble,
            .execute = execute};
}

/**
 * @brief Factory function which generates OpcodeFunction structs for opcodes which additionally
 * rely on the low byte.
 */
constexpr OpcodeFunction opcode_low_byte(std::uint8_t const high_nibble,
                                         std::uint8_t const low_byte,
                                         InstructionFunctionPtr const& execute) {
    constexpr std::uint16_t HIGH_NIBBLE_AND_LOW_BYTE_MASK{0xF0FF};

    return {.mask = HIGH_NIBBLE_AND_LOW_BYTE_MASK,
            .high_nibble = high_nibble,
            .low_byte = low_byte,
            .execute = execute};
}

/**
 * @brief Factory function which generates OpcodeFunction structs for opcodes which additionally
 * rely on the third nibble, or the high nibble of the low byte.
 */
constexpr OpcodeFunction opcode_third_nibble(std::uint8_t const high_nibble,
                                         std::uint8_t const third_nibble,
                                         InstructionFunctionPtr const& execute) {
    constexpr std::uint16_t HIGH_NIBBLE_AND_THIRD_NIBBLE_MASK{0xF0F0};

    std::uint8_t const low_byte{static_cast<std::uint8_t>(third_nibble << 4)};

    return {.mask = HIGH_NIBBLE_AND_THIRD_NIBBLE_MASK,
            .high_nibble = high_nibble,
            .low_byte = low_byte,
            .execute = execute};
}

/**
 * @brief Instruction set decode table. Represents a mapping between instructions and functions
 * that should be executed by the chip8 system.
 */
static constexpr std::array DECODE_TABLE{
    opcode_third_nibble(0x0, 0xC, &System::sc_down),
    opcode_low_byte(0x0, 0xE0, &System::cls),
    opcode_low_byte(0x0, 0xEE, &System::ret),
    opcode_low_byte(0x0, 0xFB, &System::sc_right),
    opcode_low_byte(0x0, 0xFC, &System::sc_left),
    opcode_low_byte(0x0, 0xFD, &System::exit),
    opcode_low_byte(0x0, 0xFE, &System::lores),
    opcode_low_byte(0x0, 0xFF, &System::hires),
    opcode_standard(0x1, &System::jmp),
    opcode_standard(0x2, &System::call),
    opcode_standard(0x3, &System::seq_vx_nn),
    opcode_standard(0x4, &System::sne_vx_nn),
    opcode_standard(0x5, &System::seq_vx_vy),
    opcode_standard(0x6, &System::mov_vx_nn),
    opcode_standard(0x7, &System::add_vx_nn),
    opcode_low_nibble(0x8, 0x0, &System::mov_vx_vy),
    opcode_low_nibble(0x8, 0x1, &System::or_vx_vy),
    opcode_low_nibble(0x8, 0x2, &System::and_vx_vy),
    opcode_low_nibble(0x8, 0x3, &System::xor_vx_vy),
    opcode_low_nibble(0x8, 0x4, &System::add_vx_vy),
    opcode_low_nibble(0x8, 0x5, &System::sub_vx_vy),
    opcode_low_nibble(0x8, 0x6, &System::shr_vx_vy),
    opcode_low_nibble(0x8, 0x7, &System::rsb_vx_vy),
    opcode_low_nibble(0x8, 0xE, &System::shl_vx_vy),
    opcode_standard(0x9, &System::sne_vx_vy),
    opcode_standard(0xA, &System::mov_i_nnn),
    opcode_standard(0xB, &System::jmp_vx_nnn),
    opcode_standard(0xC, &System::rnd_vx_nn),
    opcode_standard(0xD, &System::drw),
    opcode_low_byte(0xE, 0x9E, &System::spr_vx),
    opcode_low_byte(0xE, 0xA1, &System::sup_vx),
    opcode_low_byte(0xF, 0x07, &System::mov_vx_dt),
    opcode_low_byte(0xF, 0x0A, &System::wait_mov_vx_key),
    opcode_low_byte(0xF, 0x15, &System::mov_dt_vx),
    opcode_low_byte(0xF, 0x18, &System::mov_st_vx),
    opcode_low_byte(0xF, 0x1E, &System::add_i_vx),
    opcode_low_byte(0xF, 0x29, &System::mov_i_font_vx),
    opcode_low_byte(0xF, 0x30, &System::mov_i_bfont_vx),
    opcode_low_byte(0xF, 0x33, &System::mov_i_bcd_vx),
    opcode_low_byte(0xF, 0x55, &System::mov_i_vx),
    opcode_low_byte(0xF, 0x65, &System::mov_vx_i),
};
} // namespace Chip8::InstructionSet
#endif // CHIP8_INSTRUCTION_SET_H