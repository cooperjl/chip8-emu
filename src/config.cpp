#include "config.h"

#include <cstdint>

#include "chip8.h"

bool Config::shift_quirk = false;
bool Config::jump_quirk = false;
bool Config::memory_quirk = false;
bool Config::vblank_quirk = false;

std::uint8_t Config::max_width = Chip8System::hires_width;
std::uint8_t Config::max_height = Chip8System::hires_height;

void Config::load_chip8() {
  shift_quirk = false;
  jump_quirk = false;
  memory_quirk = false;
  vblank_quirk = false;

  max_width = Chip8System::lores_width;
  max_height = Chip8System::lores_height;
}

void Config::load_schip() {
  shift_quirk = true;
  jump_quirk = true;
  memory_quirk = true;
  vblank_quirk = true;

  max_width = Chip8System::hires_width;
  max_height = Chip8System::hires_height;
}

void Config::load_xochip() {
  
}
