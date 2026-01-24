#include "Window.h"

#include <chrono>
#include <memory>
#include <stdexcept>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "Beeper.h"
#include "Chip8Emulator.h"

Window::Window(std::string &filename) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    throw std::runtime_error(std::string("ERROR: failed to initialise SDL: ") +
                             SDL_GetError());
  }

  if (!SDL_CreateWindowAndRenderer("CHIP-8", 640, 320,
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE,
                                   &window, &renderer)) {
    throw std::runtime_error(
        std::string("ERROR: failed to create window and renderer: ") +
        SDL_GetError());
  }

  SDL_SetRenderLogicalPresentation(renderer, 64, 32,
                                   SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

  beeper = std::make_unique<Beeper>();

  chip8system = std::make_unique<Chip8Emulator>();
  init_callback();
  chip8system->loadRom(filename);
}

void Window::parse_keymap(const uint8_t key, const uint8_t status) const
{
  auto find_key = keymap.find(key);
  if (find_key != keymap.end()) {
    // For waiting
    if (chip8system->system.waiting && status == 0x0) {
      chip8system->system.key_released = find_key->second;
    }

    chip8system->system.keys.at(find_key->second) = status;
  }
}

void Window::poll_events()
{
  SDL_Event event;
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

void Window::clear() const
{
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);
}

void Window::draw() const
{
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  for (int x = 0; x < chip8system->system.current_width; x++) {
    for (int y = 0; y < chip8system->system.current_height; y++) {
      if (chip8system->system.display.at((y * chip8system->system.current_width) + x) == 1) {
        SDL_RenderPoint(renderer, x, y);
      }
    }
  }
}

void Window::present() const { SDL_RenderPresent(renderer); }

void Window::init_callback() const
{
  chip8system->system.set_callback([this](CallbackType callback_type) {
    switch (callback_type) {
      case CallbackType::CHIP8_CALLBACK_EXIT:
        // Call a quit event to ensure all quit handling is done in the same place.
        SDL_Event event;
        SDL_zero(event);
        event.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&event);
        break;
      case CallbackType::CHIP8_CALLBACK_HIRES:
        SDL_SetRenderLogicalPresentation(renderer, 128, 64,
                                         SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
        break;
      case CallbackType::CHIP8_CALLBACK_LORES:
        SDL_SetRenderLogicalPresentation(renderer, 64, 32,
                                         SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
        break;
    }
  });
}

void Window::main_loop() {
  using namespace std::chrono;

  auto current_time = system_clock::now();
  auto fps_time = current_time;
  auto cycle_time = current_time;

  while (running) {
    current_time = system_clock::now();

    // Target fps to reach the expected 60hz for chip8
    static constexpr double fps = 60;
    // Tickrate at standard default for now, to be configured game by game
    static constexpr double tickrate = 15.0;

    static constexpr auto fps_step = round<system_clock::duration>(duration<double>{1.0 / fps});
    static constexpr auto cycle_step = round<system_clock::duration>(duration<double>{1.0 / (fps * tickrate)});

    if (current_time > cycle_time + cycle_step) {
      chip8system->cycle();

      cycle_time = current_time;
    }

    if (current_time > fps_time + fps_step) {
      if (chip8system->updateTimers()) {
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
  chip8system.reset();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
}
