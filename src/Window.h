#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>

#include <memory>
#include <unordered_map>

#include "Beeper.h"
#include "Chip8Emulator.h"

class Window {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  std::unique_ptr<Beeper> beeper;
  std::unique_ptr<Chip8Emulator> chip8system;

  void parse_keymap(uint8_t key, uint8_t status) const;

 public:
  bool running = true;

  Window(std::string_view filename);

  void init_callback() const;
  void main_loop();
  void poll_events();
  void clear() const;
  void draw() const;
  void present() const;

  ~Window();
};

static const std::unordered_map<uint8_t, uint8_t> keymap = {
    {SDL_SCANCODE_1, 0x1}, {SDL_SCANCODE_2, 0x2}, {SDL_SCANCODE_3, 0x3},
    {SDL_SCANCODE_4, 0xC}, {SDL_SCANCODE_Q, 0x4}, {SDL_SCANCODE_W, 0x5},
    {SDL_SCANCODE_E, 0x6}, {SDL_SCANCODE_R, 0xD}, {SDL_SCANCODE_A, 0x7},
    {SDL_SCANCODE_S, 0x8}, {SDL_SCANCODE_D, 0x9}, {SDL_SCANCODE_F, 0xE},
    {SDL_SCANCODE_Z, 0xA}, {SDL_SCANCODE_X, 0x0}, {SDL_SCANCODE_C, 0xB},
    {SDL_SCANCODE_V, 0xF},
};
#endif
