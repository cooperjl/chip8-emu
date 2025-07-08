#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cstdint>

class Instruction {
  std::uint16_t data_;

 public:
  Instruction(uint16_t instruction) : data_{instruction} {}
  Instruction(uint8_t inst_upper, uint8_t inst_lower)
      : data_{static_cast<uint16_t>(inst_upper << 8 | inst_lower)} {}

  [[nodiscard]] auto data() const -> std::uint16_t { return data_; }

  [[nodiscard]] auto opcode() const -> std::uint8_t {
    const auto mask = 0xF000;
    const auto shift = 12;

    return (data_ & mask) >> shift;
  }

  [[nodiscard]] auto x() const -> std::uint8_t {
    const auto mask = 0x0F00;
    const auto shift = 8;

    return (data_ & mask) >> shift;
  }

  [[nodiscard]] auto y() const -> std::uint8_t {
    const auto mask = 0x00F0;
    const auto shift = 4;

    return (data_ & mask) >> shift;
  }

  [[nodiscard]] auto n() const -> std::uint8_t {
    const auto mask = 0x000F;

    return (data_ & mask);
  }
  [[nodiscard]] auto nn() const -> std::uint8_t {
    const auto mask = 0x00FF;

    return (data_ & mask);
  }

  [[nodiscard]] auto nnn() const -> std::uint16_t {
    const auto mask = 0x0FFF;

    return (data_ & mask);
  }
};

#endif
