#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
struct Config {
  static bool shift_quirk;
  static bool jump_quirk;
  static bool memory_quirk; // load store FX55 FX65 quirk
  static bool vblank_quirk;

  static std::uint8_t max_width;
  static std::uint8_t max_height;

  static void load_chip8();
  static void load_schip();
  static void load_xochip();
};

#endif // CONFIG_H
