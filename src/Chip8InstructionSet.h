#ifndef CHIP8_INSTRUCTION_SET_H
#define CHIP8_INSTRUCTION_SET_H

#include <cstdint>

namespace InstructionSet
{

using Chip8InstructionFunctionPtr = void(Chip8System::*)(Instruction);

struct OpcodeFunction
{
  std::uint16_t mask;
  std::uint16_t value;
  Chip8InstructionFunctionPtr execute;

  [[nodiscard]] constexpr bool matches(Instruction const instruction) const {
    return (instruction.raw_data() & mask) == value;
  }
};

constexpr std::uint16_t high_nibble_operation(std::uint8_t const high)
{
  constexpr std::uint16_t SHIFT{ 12 };
  return static_cast<std::uint16_t>(high) << SHIFT;
}

constexpr std::uint16_t high_low_nibble_operation(std::uint8_t const high, std::uint8_t const low)
{
  return high_nibble_operation(high) | static_cast<std::uint16_t>(low);
}

constexpr std::uint16_t high_nibble_low_byte_operation(std::uint8_t const high, std::uint8_t const byte)
{
  return high_nibble_operation(high) | static_cast<std::uint16_t>(byte);
}

constexpr OpcodeFunction opcode_high_nibble(std::uint8_t const high, Chip8InstructionFunctionPtr const& execute)
{
  constexpr std::uint16_t HIGH_NIBBLE_MASK{ 0xF000 };
  return { .mask = HIGH_NIBBLE_MASK, .value = high_nibble_operation(high), .execute = execute };
}

constexpr OpcodeFunction opcode_high_low_nibble(std::uint8_t const high, std::uint8_t const low, Chip8InstructionFunctionPtr const& execute)
{
  constexpr std::uint16_t HIGH_LOW_NIBBLE_MASK{ 0xF00F };
  return { .mask = HIGH_LOW_NIBBLE_MASK, .value = high_low_nibble_operation(high, low), .execute = execute };
}

constexpr OpcodeFunction opcode_high_nibble_low_byte(std::uint8_t const high, std::uint8_t const byte, Chip8InstructionFunctionPtr const& execute)
{
  constexpr std::uint16_t HIGH_NIBBLE_LOW_BYTE_MASK{ 0xF0FF };
  return { .mask = HIGH_NIBBLE_LOW_BYTE_MASK, .value = high_low_nibble_operation(high, byte), .execute = execute };
}
}

#endif // CHIP8_INSTRUCTION_SET_H
