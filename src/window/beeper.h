#ifndef CHIP8_BEEPER_H
#define CHIP8_BEEPER_H

#include <array>
#include <cstdint>
#include <numbers>

#include <SDL3/SDL_audio.h>

#include "sdl_wrapper.h"

class Beeper {
private:
    static constexpr std::uint16_t SAMPLE_RATE{48000};
    static constexpr std::uint16_t FREQUENCY{440};
    static constexpr std::uint16_t AMPLITUDE{32};
    static constexpr std::uint16_t OFFSET{128};

    static constexpr float PHASE_INC{2.0 * std::numbers::pi * FREQUENCY / SAMPLE_RATE};

    SDLWrappedPtr<SDL_AudioStream, SDL_DestroyAudioStream> stream;

    std::array<std::uint8_t, 1024> samples{};
    double phase{0}; // phase for sine wave

public:
    Beeper();

    void beep();
};
#endif // CHIP8_BEEPER_H
