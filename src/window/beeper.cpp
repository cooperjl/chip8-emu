#include "beeper.h"

#include <cmath>
#include <numbers>
#include <stdexcept>

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_error.h>

Beeper::Beeper() {
    constexpr SDL_AudioSpec AUDIO_SPEC{.format = SDL_AUDIO_U8, .channels = 1, .freq = SAMPLE_RATE};

    stream = SDLWrappedPtr<SDL_AudioStream, SDL_DestroyAudioStream>{SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &AUDIO_SPEC, nullptr, nullptr)};

    if (!stream) {
        throw std::runtime_error{std::string("Error: failed to open audio: ") + SDL_GetError()};
    }

    SDL_ResumeAudioStreamDevice(stream.get());
}

void Beeper::beep() {
    constexpr int MINIMUM_AUDIO{100};
    if (SDL_GetAudioStreamQueued(stream.get()) < MINIMUM_AUDIO) {
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

        SDL_PutAudioStreamData(stream.get(), samples.data(), static_cast<int>(samples.size()));
    }
}