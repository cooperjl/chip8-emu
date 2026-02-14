#include "window.h"

#include <chrono>
#include <memory>
#include <stdexcept>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "beeper.h"
#include "chip8/emulator.h"

Window::Window(std::string_view const filename) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        throw std::runtime_error{std::string("Error: failed to initialise SDL: ") + SDL_GetError()};
    }

    constexpr std::uint16_t DEFAULT_WINDOW_WIDTH{640};
    constexpr std::uint16_t DEFAULT_WINDOW_HEIGHT{320};

    if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
                                     SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &window,
                                     &renderer)) {
        throw std::runtime_error{std::string("Error: failed to create window and renderer: ") +
                                 SDL_GetError()};
    }

    SDL_SetRenderLogicalPresentation(renderer, Chip8::System::LORES_WIDTH,
                                     Chip8::System::LORES_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    beeper = std::make_unique<Beeper>();

    chip8_emulator = std::make_unique<Chip8::Emulator>();
    init_callback();
    chip8_emulator->loadRom(filename);
}

void Window::parse_keymap(std::uint8_t const key, std::uint8_t const status) const {
    auto find_key{KEYMAP.find(key)};
    if (find_key != KEYMAP.end()) {
        // For waiting
        if (chip8_emulator->system.waiting && status == 0x0) {
            chip8_emulator->system.key_released = find_key->second;
        }

        chip8_emulator->system.keys.at(find_key->second) = status;
    }
}

void Window::poll_events() {
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            parse_keymap(event.key.scancode, 0x1);
            break;
        case SDL_EVENT_KEY_UP:
            parse_keymap(event.key.scancode, 0x0);
            break;
        default:
            break;
        }
    }
}

void Window::clear() const {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
}

void Window::draw() const {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    for (size_t x{0}; x < chip8_emulator->system.current_width; ++x) {
        for (size_t y{0}; y < chip8_emulator->system.current_height; ++y) {
            if (chip8_emulator->system.display.at((y * chip8_emulator->system.current_width) + x) == 1) {
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }
}

void Window::present() const { SDL_RenderPresent(renderer); }

void Window::init_callback() const {
    chip8_emulator->system.set_callback([this](Chip8::CallbackType const callback_type) {
        switch (callback_type) {
        case Chip8::CallbackType::CHIP8_CALLBACK_EXIT: {
            // Call a quit event to ensure all quit handling is done in the same place.
            SDL_Event event{};
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
            break;
        }
        case Chip8::CallbackType::CHIP8_CALLBACK_HIRES:
            SDL_SetRenderLogicalPresentation(renderer, Chip8::System::HIRES_WIDTH,
                                             Chip8::System::HIRES_HEIGHT,
                                             SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
            break;
        case Chip8::CallbackType::CHIP8_CALLBACK_LORES:
            SDL_SetRenderLogicalPresentation(renderer, Chip8::System::LORES_WIDTH,
                                             Chip8::System::LORES_HEIGHT,
                                             SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
            break;
        }
    });
}

void Window::main_loop() {
    using namespace std::chrono;

    auto current_time{system_clock::now()};
    auto fps_time{current_time};
    auto cycle_time{current_time};

    while (running) {
        current_time = system_clock::now();

        // Target fps to reach the expected 60hz for chip8
        static constexpr double FPS{60};
        // Tick rate at standard default for now, to be configured game by game
        static constexpr double TICK_RATE{15.0};

        static constexpr auto FPS_STEP{round<system_clock::duration>(duration<double>{1.0 / FPS})};
        static constexpr auto CYCLE_STEP{
            round<system_clock::duration>(duration<double>{1.0 / (FPS * TICK_RATE)})};

        if (current_time > cycle_time + CYCLE_STEP) {
            chip8_emulator->cycle();

            cycle_time = current_time;
        }

        if (current_time > fps_time + FPS_STEP) {
            if (chip8_emulator->updateTimers()) {
                beeper->beep();
            }

            clear();
            draw();
            present();
            poll_events();

            fps_time = current_time;
        }
    }
}

Window::~Window() {
    beeper.reset();
    chip8_emulator.reset();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}