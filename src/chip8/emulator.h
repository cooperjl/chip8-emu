#ifndef CHIP8_EMULATOR_H
#define CHIP8_EMULATOR_H

#include "instruction.h"
#include "system.h"

namespace Chip8 {
class Emulator {
public:
    Emulator();

    System system{};

    void decodeInstruction(Instruction instruction);

    void loadRom(std::string_view filename);

    bool updateTimers() noexcept;

    [[nodiscard]] Instruction getCurrentInstruction() const;

    void cycle();
};
} // namespace Chip8
#endif // CHIP8_EMULATOR_H
