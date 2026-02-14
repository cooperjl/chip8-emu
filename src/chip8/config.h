#ifndef CHIP8_CONFIG_H
#define CHIP8_CONFIG_H

#include <cstdint>

namespace Chip8 {
struct Config {
    static bool shift_quirk;
    static bool jump_quirk;
    static bool memory_quirk; // load store FX55 FX65 quirk
    static bool vblank_quirk;

    static std::uint8_t max_width;
    static std::uint8_t max_height;

    static void load_chip8();
    static void load_super_chip();
    static void load_xo_chip();
};
} // namespace Chip8
#endif // CHIP8_CONFIG_H
