#include "Config.h"

#include <cstdint>

#include "Chip8System.h"

bool Config::shift_quirk{false};
bool Config::jump_quirk{false};
bool Config::memory_quirk{false};
bool Config::vblank_quirk{false};

std::uint8_t Config::max_width{Chip8System::HIRES_WIDTH};
std::uint8_t Config::max_height{Chip8System::HIRES_HEIGHT};

void Config::load_chip8() {
    shift_quirk = false;
    jump_quirk = false;
    memory_quirk = false;
    vblank_quirk = false;

    max_width = Chip8System::LORES_WIDTH;
    max_height = Chip8System::LORES_HEIGHT;
}

void Config::load_super_chip() {
    shift_quirk = true;
    jump_quirk = true;
    memory_quirk = true;
    vblank_quirk = true;

    max_width = Chip8System::HIRES_WIDTH;
    max_height = Chip8System::HIRES_HEIGHT;
}

void Config::load_xo_chip() {}
