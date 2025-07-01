#include <memory>
#include <stdexcept>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#include "window.h"
#include "beeper.h"
#include "chip8.h"

Window::Window(std::string &filename) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    throw std::runtime_error(std::string("ERROR: failed to initialise SDL: ") + SDL_GetError());
  }

  if (!SDL_CreateWindowAndRenderer("CHIP-8", 640, 320, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    throw std::runtime_error(std::string("ERROR: failed to create window and renderer: ") + SDL_GetError());
  }

  SDL_SetRenderLogicalPresentation(renderer, 64, 32, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

  beeper = std::make_unique<Beeper>();

  chip8system = std::make_unique<Chip8System>();
  chip8system->load_rom(filename);
}

void Window::parse_keymap(uint8_t key, uint8_t status) {
  auto find_key = keymap.find(key);
  if (find_key != keymap.end()) {
    // For waiting
    if (chip8system->waiting && status == 0x0) {
      chip8system->key_released = find_key->second;
    }

    chip8system->keys.at(find_key->second) = status;
  }
}

void Window::poll_events() {
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

void Window::clear() {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);
}

void Window::draw() {
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  for (int x = 0; x < 64; x++) {
    for (int y = 0; y < 32; y++) {
      if (chip8system->display.at((y * 64) + x) == 1) {
        SDL_RenderPoint(renderer, x, y);
      }
    }
  }
}

void Window::present() {
  SDL_RenderPresent(renderer);
}

void Window::main_loop() {
  uint64_t fps_time = SDL_GetTicks();
  uint64_t cycle_time = SDL_GetTicks();

  while (running) {
    uint64_t current_time = SDL_GetTicks();
    uint64_t fps_delta = current_time - fps_time;
    uint64_t cycle_delta = current_time - cycle_time;
    
    if (cycle_delta > (1000.0 / 700.0)) {
      chip8system->cycle();

      cycle_time = current_time;
    }

    if (fps_delta > (1000.0 / 60.0)) {
      if (chip8system->update_timers()) {
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
