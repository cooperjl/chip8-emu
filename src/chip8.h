#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <stack>
#include <array>

#include <SDL3/SDL_scancode.h>

#include "instructions.h"

class Chip8System {
  // chip8 machine
  std::array<std::uint8_t, 4096> memory_ = {};
  std::uint16_t pc_ = 0; // program counter
  std::uint16_t i_ = 0; // index register
  std::stack<std::uint16_t> stack_;
  std::uint8_t dt_ = 0; // delay_timer
  std::uint8_t st_ = 0; // sound_timer
  std::array<std::uint8_t, 16> v_ = {}; // registers

  public:
    Chip8System();

    std::array<std::uint8_t, 32 * 64> display = {};

    std::array<std::uint8_t, 0x10> keys = {};
    std::uint8_t key_released = 0xFF;
    bool waiting = false;

    // public interface for internal state for debugging and testing
    [[nodiscard]] auto pc() const -> std::uint16_t { return pc_; }
    [[nodiscard]] auto v(std::uint8_t i) const -> std::uint8_t { return v_.at(i); }

    // public functions
    void load_rom(std::string &filename);
    auto update_timers() -> bool;
    auto cycle() -> int;

  // Decode function
  auto decode(Instruction instruction) -> int;

  auto get_cur_inst() -> Instruction;

  // 0x0 Instructions
  void CLS(); // 0E00
  void RET(); // 00EE
  // 0x1-0x7 Instructions
  void JMP(Instruction instruction); // 1NNN
  void CALL(Instruction instruction); // 2NNN
  void SEQ_VX_NN(Instruction instruction); // 3XNN
  void SNE_VX_NN(Instruction instruction); // 4XNN
  void SEQ_VX_VY(Instruction instruction); // 5XY0
  void MOV_VX_NN(Instruction instruction); // 6XNN
  void ADD_VX_NN(Instruction instruction); // 7XNN
  // 0x8 Instructions
  void MOV_VX_VY(Instruction instruction); // 8XY0
  void OR_VX_VY(Instruction instruction);  // 8XY1
  void AND_VX_VY(Instruction instruction); // 8XY2
  void XOR_VX_VY(Instruction instruction); // 8XY3
  void ADD_VX_VY(Instruction instruction); // 8XY4
  void SUB_VX_VY(Instruction instruction); // 8XY5
  void LSR_VX_VY(Instruction instruction); // 8XY6
  void RSB_VX_VY(Instruction instruction); // 8XY7
  void LSL_VX_VY(Instruction instruction); // 8XYE
  // 0x9-0xD Instructions
  void SNE_VX_VY(Instruction instruction); // 9XY0
  void MOV_I_NNN(Instruction instruction); // ANNN
  void JMP_V0_NNN(Instruction instruction); // BNNN
  void RND_VX_NN(Instruction instruction); // CNNN
  void DRW(Instruction instruction); // DXYN
  // 0xE Instructions  
  void SPK_VX(Instruction instruction); // EX9E
  void SNK_VX(Instruction instruction); // EXA1
  // 0xF Instructions
  void MOV_VX_DT(Instruction instruction); // FX07
  void MOV_VX_WAIT_K(Instruction instruction); // FX0A
  void MOV_DT_VX(Instruction instruction); // FX15
  void MOV_ST_VX(Instruction instruction); // FX18
  void ADD_I_VX(Instruction instruction); // FX1E
  void MOV_I_F_VX(Instruction instruction); // FX29
  void MOV_I_BCD_VX(Instruction instruction); // FX33
  void MOV_I_VX(Instruction instruction); // FX55
  void MOV_VX_I(Instruction instruction); // FX65
};

// We want to copy this font to 050-09F memory every time
static constexpr std::array<uint8_t, 0x50> font = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

#endif
