#ifndef CHIP8_EMU_SDL_WRAPPER_H
#define CHIP8_EMU_SDL_WRAPPER_H

#include <memory>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>

struct SDLContext {
    explicit SDLContext(SDL_InitFlags const flags) {
        if (!SDL_Init(flags)) {
            throw std::runtime_error{std::string("Error: failed to initialise SDL: ") +
                                     SDL_GetError()};
        }
    }

    ~SDLContext() { SDL_Quit(); }
};

template <auto DeleteFunction> struct SDLDeleter {
    template <typename T> void operator()(T* ptr) const {
        if (ptr) {
            DeleteFunction(ptr);
        }
    }
};

template <class T, auto DeleteFunction>
using SDLWrappedPtr = std::unique_ptr<T, SDLDeleter<DeleteFunction>>;

#endif // CHIP8_EMU_SDL_WRAPPER_H
