#include "Chip8Emulator.h"

#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <iterator>
#include <print>
#include <sstream>
#include <unordered_map>

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

void Chip8Emulator::decodeInstruction(Instruction instruction)
{
  for (auto [mask, value, execute] : DECODE_TABLE)
  {
    if ((instruction.raw_data() & mask) == value)
    {
      std::visit([this, instruction](auto&& instructionFunction)
      {
        using T = std::decay_t<decltype(instructionFunction)>;
        if constexpr (std::is_same_v<T, Chip8Function>)
        {
            instructionFunction(system);
        } else if constexpr (std::is_same_v<T, Chip8InstructionFunction>)
        {
            instructionFunction(system, instruction);
        }
        else
        {
          throw std::runtime_error("Error: unable to properly call instruction");
        }
      }, execute);

      return;
    }
  }

  const std::string errorString
  {
    std::format(
      "Error: Invalid (or unimplemented) chip8 instruction: {:04X}",
      instruction.raw_data())
  };

  throw std::runtime_error(errorString);
}

void Chip8Emulator::loadRom(std::string &filename)
{
  constexpr std::uint16_t START_IDX{ 0x200 };

  if (std::ifstream file{filename, std::ios::binary})
  {
    std::println("Loading rom from path: {}", filename);
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string rom = buffer.str();

    std::ranges::copy(rom, std::begin(system.memory) + START_IDX);
  }
  else
  {
    throw std::runtime_error(std::string("ERROR: file read went wrong"));
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