#include "config.h"

bool Config::shift_quirk = false;
bool Config::jump_quirk = false;
bool Config::memory_quirk = false;

void Config::load_chip8() {
  shift_quirk = false;
  jump_quirk = false;
  memory_quirk = false;
}

void Config::load_schip() {
  shift_quirk = true;
  jump_quirk = true;
  memory_quirk = true;
}

void Config::load_xochip() {
  
}
