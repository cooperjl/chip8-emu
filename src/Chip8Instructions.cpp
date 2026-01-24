#include "Chip8Instructions.h"

#include "Chip8Emulator.h"
#include "Chip8System.h"
#include "Config.h"
#include "Instruction.h"

#include <print>

void Chip8Instructions::sc_down(Chip8System& system, const Instruction instruction) noexcept
{
  const std::uint16_t scroll_n{ static_cast<uint16_t>(system.current_width * instruction.n()) };
  // Remove pixels scrolled down off the bottom of the screen
  system.display.erase(system.display.end() - scroll_n, system.display.end());
  // Insert blank pixels at start of vector, pushing the display down
  system.display.insert(system.display.begin(), scroll_n, 0x0);
}

void Chip8Instructions::cls(Chip8System& system) noexcept
{
  std::ranges::fill(system.display, 0x0);
}

void Chip8Instructions::ret(Chip8System& system) noexcept
{
  system.program_counter = system.stack.top();
  system.stack.pop();
}

void Chip8Instructions::sc_right(Chip8System& system) noexcept
{
  auto iter{ system.display.begin() };
  // Iterate row by row, performing scroll
  while (iter != system.display.end())
  {
    // Remove pixels scrolled right off the row
    system.display.erase(iter + system.current_width - 4, iter + system.current_width);
    // Insert blank pixels at the start of the row, pushing the display right
    system.display.insert(iter, 4, 0x0);

    if (std::distance(iter, system.display.end()) > system.current_width)
    {
      std::advance(iter, system.current_width);
    } else
    {
      iter = system.display.end();
    }
  }
}
void Chip8Instructions::sc_left(Chip8System& system) noexcept
{
  auto iter{ system.display.begin() };
  // Iterate row by row, performing scroll
  while (iter != system.display.end())
  {
    // Remove pixels scrolled left off the row
    system.display.erase(iter, iter + 4);
    // Insert blank pixels at the end of the row, pushing the display left
    system.display.insert(iter + system.current_width - 4, 4, 0x0);

    if (std::distance(iter, system.display.end()) > system.current_width)
    {
      std::advance(iter, system.current_width);
    } else
    {
      iter = system.display.end();
    }
  }
}
void Chip8Instructions::exit(const Chip8System& system) noexcept
{
  if (system.callback_function)
  {
    system.callback_function(CallbackType::CHIP8_CALLBACK_EXIT);
  } else
  {
    std::println(stderr, "WARN: callback not set. SUPER-CHIP exit instruction needs to modify window state");
  }
}
void Chip8Instructions::lores(Chip8System& system) noexcept
{
  if (system.callback_function)
  {
    system.callback_function(CallbackType::CHIP8_CALLBACK_LORES);
  } else
  {
    std::println(stderr, "WARN: callback not set. SUPER-CHIP lores instruction needs to modify window state");
  }

  system.current_width = Chip8System::LORES_WIDTH;
  system.current_height = Chip8System::LORES_HEIGHT;
  system.display.resize(static_cast<size_t>(system.current_width * system.current_height));

}

void Chip8Instructions::hires(Chip8System& system) noexcept
{
  if (system.callback_function)
  {
    system.callback_function(CallbackType::CHIP8_CALLBACK_HIRES);
  } else
  {
    std::println(stderr, "WARN: callback not set. SUPER-CHIP hires instruction needs to modify window state");
  }

  system.current_width = Chip8System::HIRES_WIDTH;
  system.current_height = Chip8System::HIRES_HEIGHT;
  system.display.resize(static_cast<size_t>(system.current_width * system.current_height));
}

void Chip8Instructions::jmp(Chip8System& system, const Instruction instruction) noexcept
{
  system.program_counter = instruction.nnn();
}

void Chip8Instructions::call(Chip8System& system, const Instruction instruction) noexcept
{
  system.stack.push(system.program_counter);
  system.program_counter = instruction.nnn();
}

void Chip8Instructions::seq_vx_nn(Chip8System& system, const Instruction instruction) noexcept
{
  if (system.registers.at(instruction.x()) == instruction.nn())
  {
    system.program_counter += 2;
  }
}

void Chip8Instructions::sne_vx_nn(Chip8System& system, const Instruction instruction) noexcept
{
  if (system.registers.at(instruction.x()) != instruction.nn())
  {
    system.program_counter += 2;
  }
}

void Chip8Instructions::seq_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  if (system.registers.at(instruction.x()) == system.registers.at(instruction.y()))
  {
    system.program_counter += 2;
  }
}

void Chip8Instructions::mov_vx_nn(Chip8System& system, const Instruction instruction) noexcept
{
  system.registers.at(instruction.x()) = instruction.nn();
}

void Chip8Instructions::add_vx_nn(Chip8System& system, const Instruction instruction) noexcept
{
  system.registers.at(instruction.x()) += instruction.nn();
}

void Chip8Instructions::mov_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  system.registers.at(instruction.x()) = system.registers.at(instruction.y());
}

void Chip8Instructions::or_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  system.registers.at(instruction.x()) |= system.registers.at(instruction.y());

  // quirk
  system.registers.at(Chip8System::FLAG_REGISTER_IDX) = 0;
}

void Chip8Instructions::and_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  system.registers.at(instruction.x()) &= system.registers.at(instruction.y());

  // quirk
  system.registers.at(Chip8System::FLAG_REGISTER_IDX) = 0;
}

void Chip8Instructions::xor_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  system.registers.at(instruction.x()) ^= system.registers.at(instruction.y());

  // quirk
  system.registers.at(Chip8System::FLAG_REGISTER_IDX) = 0;
}

void Chip8Instructions::add_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  const std::uint8_t register_x{ system.registers.at(instruction.x()) };
  const std::uint8_t register_y{ system.registers.at(instruction.y()) };

  system.registers.at(instruction.x()) = register_x + register_y;

  // Set flag register to the carry value
  system.registers.at(Chip8System::FLAG_REGISTER_IDX) = (register_x > 255 - register_y) ? 1 : 0;
}

void Chip8Instructions::sub_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  const std::uint8_t register_x{ system.registers.at(instruction.x()) };
  const std::uint8_t register_y{ system.registers.at(instruction.y()) };

  system.registers.at(instruction.x()) = register_x - register_y;

  // Set flag register to the borrow value
  system.registers.at(Chip8System::FLAG_REGISTER_IDX) = (register_x >= register_y) ? 1 : 0;
}
void Chip8Instructions::shr_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  if (!Config::shift_quirk)
  {
    system.registers.at(instruction.x()) = system.registers.at(instruction.y());
  }

  const std::uint8_t register_x{ system.registers.at(instruction.x()) };

  system.registers.at(instruction.x()) >>= 1;  // Perform shift
  system.registers.at(Chip8System::FLAG_REGISTER_IDX) = (register_x & 0b00000001);
}

void Chip8Instructions::rsb_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  const std::uint8_t register_x{ system.registers.at(instruction.x()) };
  const std::uint8_t register_y{ system.registers.at(instruction.y()) };

  system.registers.at(instruction.x()) = register_y - register_x;

  // Set flag register to the borrow value
  system.registers.at(Chip8System::FLAG_REGISTER_IDX) = (register_y >= register_x) ? 1 : 0;
}

void Chip8Instructions::shl_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  if (!Config::shift_quirk)
  {
    system.registers.at(instruction.x()) = system.registers.at(instruction.y());
  }

  const std::uint8_t register_x{ system.registers.at(instruction.x()) };

  system.registers.at(instruction.x()) <<= 1; // Perform shift
  system.registers.at(Chip8System::FLAG_REGISTER_IDX) = register_x >> 7; // Set flag register to the bit shifted out
}

void Chip8Instructions::sne_vx_vy(Chip8System& system, const Instruction instruction) noexcept
{
  if (system.registers.at(instruction.x()) != system.registers.at(instruction.y()))
  {
    system.program_counter += 2;
  }
}

void Chip8Instructions::mov_i_nnn(Chip8System& system, const Instruction instruction) noexcept
{
  system.index_register= instruction.nnn();
}
void Chip8Instructions::jmp_vx_nnn(Chip8System& system, const Instruction instruction) noexcept
{
  if (Config::jump_quirk)
  {
    system.program_counter = instruction.nnn() + system.registers.at(instruction.x());
  } else
  {
    system.program_counter = instruction.nnn() + system.registers.at(0x0);
  }
}

void Chip8Instructions::rnd_vx_nn(Chip8System& system, const Instruction instruction) noexcept
{
  // system.registers.at(instruction.x()) = dist(rng) & instruction.nn();
}

void Chip8Instructions::drw(Chip8System& system, const Instruction instruction) noexcept
{
  const std::uint8_t register_x{ static_cast<uint8_t>(system.registers.at(instruction.x()) % system.current_width) };
  const std::uint8_t register_y{ static_cast<uint8_t>(system.registers.at(instruction.y()) % system.current_height) };

  system.registers.at(Chip8System::FLAG_REGISTER_IDX) = 0;

  const bool schip{ instruction.n() == 0 };

  const std::uint8_t n_col{ static_cast<uint8_t>(schip ? 16 : 8) };
  const std::uint8_t n_row{ static_cast<uint8_t>(schip ? 16 : instruction.n()) };

  for (std::uint8_t row = 0; row < n_row && register_x + row < system.current_height; row++)
  {
    for (std::uint8_t col = 0; col < n_col && register_y + col < system.current_width; col++)
    {
      std::uint8_t byte{ 0 };
      if (schip)
      {
        byte = system.memory.at(system.index_register + (2 * row) + (col >= 8 ? 1 : 0));
      } else
      {
        byte = system.memory.at(system.index_register + row);
      }

      // Extract current pixel from byte using bit shift and binary and
      const std::uint8_t pixel{ static_cast<std::uint8_t>(byte & 0b10000000 >> (col >= 8 ? col & 0x7 : col)) };
      if (pixel != 0)
      {
        const std::uint16_t idx{ static_cast<std::uint16_t>((register_x + row) * system.current_width + (register_y + col)) };
        if (system.display.at(idx) == 0x01)
        {
          system.registers.at(Chip8System::FLAG_REGISTER_IDX) = 1;
        }
        system.display.at(idx) ^= 1;
      }
    }
  }
}

void Chip8Instructions::spr_vx(Chip8System& system, const Instruction instruction) noexcept
{
  const std::uint8_t register_x{ system.registers.at(instruction.x()) };

  if (register_x < system.keys.size() && system.keys.at(register_x) == 0x1)
  {
    system.program_counter += 2;
  }
}

void Chip8Instructions::sup_vx(Chip8System& system, const Instruction instruction) noexcept
{
  const std::uint8_t register_x{ system.registers.at(instruction.x()) };

  if (register_x < system.keys.size() && system.keys.at(register_x) == 0x0)
  {
    system.program_counter += 2;
  }
}

void Chip8Instructions::mov_vx_dt(Chip8System& system, const Instruction instruction) noexcept
{
  system.registers.at(instruction.x()) = system.delay_timer;
}

void Chip8Instructions::wait_mov_vx_key(Chip8System& system, const Instruction instruction) noexcept
{
  if (system.key_released == 0xFF)
  {
    system.waiting = true;
    system.program_counter -= 2;
  } else
  {
    system.registers.at(instruction.x()) = system.key_released;
    system.waiting = false;
    system.key_released = 0xFF;
  }
}

void Chip8Instructions::mov_dt_vx(Chip8System& system, const Instruction instruction) noexcept
{
  system.delay_timer = system.registers.at(instruction.x());
}

void Chip8Instructions::mov_st_vx(Chip8System& system, const Instruction instruction) noexcept
{
  system.sound_timer = system.registers.at(instruction.x());
}

void Chip8Instructions::add_i_vx(Chip8System& system, const Instruction instruction) noexcept
{
  system.index_register += system.registers.at(instruction.x());
}

void Chip8Instructions::mov_i_font_vx(Chip8System& system, const Instruction instruction) noexcept
{
  // 5 bytes per character
  system.index_register = (system.registers.at(instruction.x()) * 0x5);
}

void Chip8Instructions::mov_i_bfont_vx(Chip8System& system, const Instruction instruction) noexcept
{
  // A bytes per character, plus offset from FONT size
  system.index_register = (system.registers.at(instruction.x()) * 0xA) + FONT.size();
}

void Chip8Instructions::mov_i_bcd_vx(Chip8System& system, const Instruction instruction) noexcept
{
  system.memory.at(system.index_register) = system.registers.at(instruction.x()) / 100;
  system.memory.at(system.index_register + 1) = (system.registers.at(instruction.x()) / 10) % 10;
  system.memory.at(system.index_register + 2) = system.registers.at(instruction.x()) % 10;
}

void Chip8Instructions::mov_i_vx(Chip8System& system, const Instruction instruction) noexcept
{
  for (size_t idx{ 0 }; idx <= instruction.x(); idx++)
  {
    system.memory.at(system.index_register + idx) = system.registers.at(idx);
  }

  if (!Config::memory_quirk)
  {
    system.index_register= instruction.x() + 1;
  }
}

void Chip8Instructions::mov_vx_i(Chip8System& system, const Instruction instruction) noexcept
{
  for (size_t idx{ 0 }; idx <= instruction.x(); idx++)
  {
    system.registers.at(idx) = system.memory.at(system.index_register + idx);
  }

  if (!Config::memory_quirk)
  {
    system.index_register = instruction.x() + 1;
  }
}