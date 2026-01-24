#ifndef BEEPER_H
#define BEEPER_H

#include <SDL3/SDL_audio.h>

#include <array>
#include <cstdint>
#include <numbers>

class Beeper {
  SDL_AudioStream *stream = nullptr;

  static constexpr std::uint16_t sample_rate = 48000;
  static constexpr std::uint16_t frequency = 440;
  static constexpr std::uint16_t amplitude = 32;
  static constexpr std::uint16_t offset = 128;

  static constexpr float phase_inc =
      2.0 * std::numbers::pi * frequency / sample_rate;

  std::array<std::uint8_t, 1024> samples;
  double phase = 0;  // phase for sine wave

 public:
  Beeper();

  void beep();

  ~Beeper();
};

#endif
