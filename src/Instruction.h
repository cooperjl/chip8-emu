#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>

class Instruction
{
  std::uint16_t data;

 public:
  Instruction(std::uint16_t const instruction) : data{instruction}{}
  Instruction(std::uint8_t const inst_upper, std::uint8_t const inst_lower): data
  {
    static_cast<std::uint16_t>(inst_upper << 8 | inst_lower)
  }{}

  [[nodiscard]] auto raw_data() const -> std::uint16_t
  {
     return data;
  }

  [[nodiscard]] auto opcode() const -> std::uint8_t
  {
    constexpr std::uint16_t MASK { 0xF000 };
    constexpr std::uint8_t SHIFT{ 12 };

    return ((data & MASK) >> SHIFT);
  }

  [[nodiscard]] auto x() const -> std::uint8_t
  {
    constexpr std::uint16_t MASK{ 0x0F00 };
    constexpr std::uint8_t SHIFT{ 8 };

    return ((data & MASK) >> SHIFT);
  }

  [[nodiscard]] auto y() const -> std::uint8_t
  {
    constexpr std::uint16_t MASK{ 0x00F0 };
    constexpr std::uint16_t SHIFT{ 4 };

    return ((data & MASK) >> SHIFT);
  }

  [[nodiscard]] auto n() const -> std::uint8_t
  {
    constexpr std::uint16_t MASK{ 0x000F };

    return (data & MASK);
  }

  [[nodiscard]] auto nn() const -> std::uint8_t
  {
    constexpr std::uint16_t MASK{ 0x00FF };

    return (data & MASK);
  }

  [[nodiscard]] auto nnn() const -> std::uint16_t
  {
    constexpr std::uint16_t MASK{ 0x0FFF };

    return (data & MASK);
  }
};

#endif
