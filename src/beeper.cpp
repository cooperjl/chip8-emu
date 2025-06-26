#include <stdexcept>
#include <cmath>
#include <numbers>

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_error.h>

#include "beeper.h"

Beeper::Beeper() {
  SDL_AudioSpec spec;

  spec.format = SDL_AUDIO_U8;
  spec.channels = 1;
  spec.freq = sample_rate;

  stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);

  if (!stream) {
    throw std::runtime_error(std::string("ERROR: failed to open audio: ") + SDL_GetError());
  }

  SDL_ResumeAudioStreamDevice(stream);
}

void Beeper::beep() {
  const int minimum_audio = 100;
  if (SDL_GetAudioStreamQueued(stream) < minimum_audio) {
    for (int i = 0; i < samples.size(); i++) {
      // Flatten sine to a square wave
      float square_phase = std::sin(phase) > 0.0 ? 1.0 : -1.0;
      samples[i] = amplitude * square_phase + offset;
      phase += phase_inc;
    }
    if (phase >= 2.0 * std::numbers::pi) {
      phase -= 2.0 * std::numbers::pi;
    }

    SDL_PutAudioStreamData(stream, samples.data(), samples.size());
  }
}

Beeper::~Beeper() {
  SDL_DestroyAudioStream(stream);
}
