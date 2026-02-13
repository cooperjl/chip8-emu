#include "Chip8Emulator.h"

#include "Chip8InstructionSet.h"

#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <iterator>
#include <print>
#include <sstream>
#include <stdexcept>

#include "Chip8System.h"
#include "Config.h"
#include "Instruction.h"

Chip8Emulator::Chip8Emulator()
{
  Config::load_schip();

  system.display.reserve(static_cast<size_t>(Config::max_width * Config::max_height));
  // Size to base size
  system.display.resize(system.LORES_WIDTH * system.LORES_HEIGHT);
  // read fonts in
  std::ranges::copy(FONT, std::begin(system.memory));
  std::ranges::copy(BIG_FONT, std::begin(system.memory) + FONT.size());
}

/** @brief Primary instruction decoding switch case. */
void Chip8Emulator::decodeInstruction(Instruction const instruction)
{
  for (auto const& row : DECODE_TABLE)
  {
    if (row.matches(instruction))
    {
      (system.*row.execute)(instruction);
      return;
    }
  }

  std::string const errorString
  {
    std::format(
      "Error: Invalid (or unimplemented) chip8 instruction: 0x{:04X}",
      instruction.raw_data())
  };

  throw std::runtime_error{ errorString };
}

void Chip8Emulator::loadRom(std::string_view filename)
{
  constexpr std::uint16_t START_IDX{ 0x200 };

  if (std::ifstream file{ filename.data(), std::ios::binary })
  {
    std::println("Loading rom from path: {}", filename);
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string rom = buffer.str();

    std::ranges::copy(rom, std::begin(system.memory) + START_IDX);
  }
  else
  {
    throw std::runtime_error(std::string("Error: file read went wrong"));
  }

  system.program_counter = START_IDX;
}

bool Chip8Emulator::updateTimers() noexcept
{
  if (system.delay_timer > 0)
  {
    system.delay_timer--;
  }
  if (system.sound_timer > 0)
  {
    system.sound_timer--;
  }
  return system.sound_timer > 0;
}

Instruction Chip8Emulator::getCurrentInstruction() const
{
  return Instruction
  {
    system.memory.at(system.program_counter),
    system.memory.at(system.program_counter + 1)
  };
}

auto Chip8Emulator::cycle() -> int
{
  const Instruction instruction{ getCurrentInstruction() };

  system.program_counter += 2;

  decodeInstruction(instruction);

  return 0;
}
