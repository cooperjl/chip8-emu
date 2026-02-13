#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>

class Instruction {
    std::uint16_t data;

public:
    Instruction(std::uint16_t const instruction) : data{instruction} {}

    Instruction(std::uint8_t const inst_upper, std::uint8_t const inst_lower)
        : data{static_cast<std::uint16_t>(inst_upper << 8 | inst_lower)} {}

    [[nodiscard]] std::uint16_t raw_data() const { return data; }

    [[nodiscard]] std::uint8_t opcode() const {
        constexpr std::uint16_t MASK{0xF000};
        constexpr std::uint8_t SHIFT{12};

        return ((data & MASK) >> SHIFT);
    }

    [[nodiscard]] std::uint8_t x() const {
        constexpr std::uint16_t MASK{0x0F00};
        constexpr std::uint8_t SHIFT{8};

        return ((data & MASK) >> SHIFT);
    }

    [[nodiscard]] std::uint8_t y() const {
        constexpr std::uint16_t MASK{0x00F0};
        constexpr std::uint16_t SHIFT{4};

        return ((data & MASK) >> SHIFT);
    }

    [[nodiscard]] std::uint8_t n() const {
        constexpr std::uint16_t MASK{0x000F};

        return (data & MASK);
    }

    [[nodiscard]] std::uint8_t nn() const {
        constexpr std::uint16_t MASK{0x00FF};

        return (data & MASK);
    }

    [[nodiscard]] std::uint16_t nnn() const {
        constexpr std::uint16_t MASK{0x0FFF};

        return (data & MASK);
    }
};

#endif // INSTRUCTION_H