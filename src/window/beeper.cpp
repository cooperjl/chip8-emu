#include "beeper.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_error.h>

#include <cmath>
#include <numbers>
#include <stdexcept>

Beeper::Beeper() {
    constexpr SDL_AudioSpec AUDIO_SPEC{.format = SDL_AUDIO_U8, .channels = 1, .freq = SAMPLE_RATE};

    stream =
        SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &AUDIO_SPEC, nullptr, nullptr);

    if (stream == nullptr) {
        throw std::runtime_error(std::string("Error: failed to open audio: ") + SDL_GetError());
    }

    SDL_ResumeAudioStreamDevice(stream);
}

void Beeper::beep() {
    constexpr int MINIMUM_AUDIO{100};
    if (SDL_GetAudioStreamQueued(stream) < MINIMUM_AUDIO) {
        for (std::uint8_t& sample : samples) {
            // Flatten sine to a square wave
            std::int8_t const square_phase{
                static_cast<std::int8_t>(std::sin(phase) > 0.0 ? 1 : -1)};

            sample = AMPLITUDE * square_phase + OFFSET;
            phase += PHASE_INC;
        }
        if (phase >= 2.0 * std::numbers::pi) {
            phase -= 2.0 * std::numbers::pi;
        }

        SDL_PutAudioStreamData(stream, samples.data(), static_cast<int>(samples.size()));
    }
}

Beeper::~Beeper() { SDL_DestroyAudioStream(stream); }