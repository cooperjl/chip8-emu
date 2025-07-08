#ifndef CHIP8_H
#define CHIP8_H

#include <SDL3/SDL_scancode.h>

#include <array>
#include <cstdint>
#include <functional>
#include <stack>

#include "instructions.h"

enum class CallbackType {
  CHIP8_CALLBACK_EXIT,
  CHIP8_CALLBACK_LORES,
  CHIP8_CALLBACK_HIRES,
};

class Chip8System {
  // chip8 machine
  std::array<std::uint8_t, 4096> memory_ = {};
  std::uint16_t pc_ = 0;  // program counter
  std::uint16_t i_ = 0;   // index register
  std::stack<std::uint16_t> stack_;
  std::uint8_t dt_ = 0;                  // delay_timer
  std::uint8_t st_ = 0;                  // sound_timer
  std::array<std::uint8_t, 16> v_ = {};  // registers

  using CallbackFunction = std::function<void(CallbackType callback_type)>;
  CallbackFunction callback_function_;

 public:
  Chip8System();

  //std::array<std::uint8_t, 32 * 64> display = {};
  std::vector<std::uint8_t> display;

  std::array<std::uint8_t, 0x10> keys = {};
  std::uint8_t key_released = 0xFF;
  bool waiting = false;

  std::uint8_t width = 64;
  std::uint8_t height = 32;

  // public interface for internal state for debugging and testing
  [[nodiscard]] auto pc() const -> std::uint16_t {
     return pc_;
  }
  [[nodiscard]] auto v(std::uint8_t i) const -> std::uint8_t {
    return v_.at(i);
  }

  // public functions
  void load_rom(std::string &filename);
  auto update_timers() -> bool;
  auto cycle() -> int;

  // Decode function
  auto decode(Instruction instruction) -> int;
  auto get_cur_inst() -> Instruction;

  // Callback function
  void set_callback(CallbackFunction callback_function) {
    callback_function_ = callback_function;
  }

  // 0x0 Instructions
  void CLS();   // 0E00
  void RET();   // 00EE
  void SCROLL_DOWN_N(Instruction instruction); // 00CN
  void SCROLL_RIGHT(); // 00FB
  void SCROLL_LEFT(); // 00FC
  void EXIT();  // 00FD
  void LORES(); // 00FE
  void HIRES(); // 00FF
  // 0x1-0x7 Instructions
  void JMP(Instruction instruction);        // 1NNN
  void CALL(Instruction instruction);       // 2NNN
  void SEQ_VX_NN(Instruction instruction);  // 3XNN
  void SNE_VX_NN(Instruction instruction);  // 4XNN
  void SEQ_VX_VY(Instruction instruction);  // 5XY0
  void MOV_VX_NN(Instruction instruction);  // 6XNN
  void ADD_VX_NN(Instruction instruction);  // 7XNN
  // 0x8 Instructions
  void MOV_VX_VY(Instruction instruction);  // 8XY0
  void OR_VX_VY(Instruction instruction);   // 8XY1
  void AND_VX_VY(Instruction instruction);  // 8XY2
  void XOR_VX_VY(Instruction instruction);  // 8XY3
  void ADD_VX_VY(Instruction instruction);  // 8XY4
  void SUB_VX_VY(Instruction instruction);  // 8XY5
  void LSR_VX_VY(Instruction instruction);  // 8XY6
  void RSB_VX_VY(Instruction instruction);  // 8XY7
  void LSL_VX_VY(Instruction instruction);  // 8XYE
  // 0x9-0xD Instructions
  void SNE_VX_VY(Instruction instruction);   // 9XY0
  void MOV_I_NNN(Instruction instruction);   // ANNN
  void JMP_V0_NNN(Instruction instruction);  // BNNN
  void RND_VX_NN(Instruction instruction);   // CNNN
  void DRW(Instruction instruction);         // DXYN
  // 0xE Instructions
  void SPK_VX(Instruction instruction);  // EX9E
  void SNK_VX(Instruction instruction);  // EXA1
  // 0xF Instructions
  void MOV_VX_DT(Instruction instruction);      // FX07
  void MOV_VX_WAIT_K(Instruction instruction);  // FX0A
  void MOV_DT_VX(Instruction instruction);      // FX15
  void MOV_ST_VX(Instruction instruction);      // FX18
  void ADD_I_VX(Instruction instruction);       // FX1E
  void MOV_I_F_VX(Instruction instruction);     // FX29
  void MOV_I_BF_VX(Instruction instruction);    // FX30
  void MOV_I_BCD_VX(Instruction instruction);   // FX33
  void MOV_I_VX(Instruction instruction);       // FX55
  void MOV_VX_I(Instruction instruction);       // FX65
};

// normal font, size is 5 x F = 50 
static constexpr std::array<std::uint8_t, 0x50> font = {
  0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
  0x20, 0x60, 0x20, 0x20, 0x70,  // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
  0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
  0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
  0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
  0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
  0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

// big font, size is A x F = A0
static constexpr std::array<std::uint8_t, 0xA0> big_font = {
  0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, // 0
  0x18, 0x78, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF, // 1
  0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, // 2
  0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, // 3
  0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03, // 4
  0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, // 5
  0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, // 6
  0xFF, 0xFF, 0x03, 0x03, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x18, // 7
  0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, // 8
  0xFF, 0xFF, 0xC3, 0xC3, 0xFF, 0xFF, 0x03, 0x03, 0xFF, 0xFF, // 9
  0x7E, 0xFF, 0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xC3, // A
  0xFC, 0xFC, 0xC3, 0xC3, 0xFC, 0xFC, 0xC3, 0xC3, 0xFC, 0xFC, // B
  0x3C, 0xFF, 0xC3, 0xC0, 0xC0, 0xC0, 0xC0, 0xC3, 0xFF, 0x3C, // C
  0xFC, 0xFE, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xFE, 0xFC, // D
  0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, // E
  0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0  // F
};

#endif
