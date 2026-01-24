#ifndef CHIP8_INSTRUCTIONS_H
#define CHIP8_INSTRUCTIONS_H

#include "Chip8System.h"
#include "Instruction.h"

class Chip8Instructions
{
public:
  Chip8Instructions() = delete;

  static void sc_down(Chip8System& system, Instruction instruction) noexcept;
  static void cls(Chip8System& system) noexcept;
  static void ret(Chip8System& system) noexcept;
  static void sc_right(Chip8System& system) noexcept;
  static void sc_left(Chip8System& system) noexcept;
  static void exit(const Chip8System & system) noexcept;
  static void lores(Chip8System& system) noexcept;
  static void hires(Chip8System& system) noexcept;
  static void jmp(Chip8System& system, Instruction instruction) noexcept;
  static void call(Chip8System& system, Instruction instruction) noexcept;
  static void seq_vx_nn(Chip8System& system, Instruction instruction) noexcept;
  static void sne_vx_nn(Chip8System& system, Instruction instruction) noexcept;
  static void seq_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void mov_vx_nn(Chip8System& system, Instruction instruction) noexcept;
  static void add_vx_nn(Chip8System& system, Instruction instruction) noexcept;
  static void mov_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void or_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void and_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void xor_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void add_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void sub_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void shr_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void rsb_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void shl_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void sne_vx_vy(Chip8System& system, Instruction instruction) noexcept;
  static void mov_i_nnn(Chip8System& system, Instruction instruction) noexcept;
  static void jmp_vx_nnn(Chip8System& system, Instruction instruction) noexcept;
  static void rnd_vx_nn(Chip8System& system, Instruction instruction) noexcept;
  static void drw(Chip8System& system, Instruction instruction) noexcept;
  static void spr_vx(Chip8System& system, Instruction instruction) noexcept;
  static void sup_vx(Chip8System& system, Instruction instruction) noexcept;
  static void mov_vx_dt(Chip8System& system, Instruction instruction) noexcept;
  static void wait_mov_vx_key(Chip8System& system, Instruction instruction) noexcept;
  static void mov_dt_vx(Chip8System& system, Instruction instruction) noexcept;
  static void mov_st_vx(Chip8System& system, Instruction instruction) noexcept;
  static void add_i_vx(Chip8System& system, Instruction instruction) noexcept;
  static void mov_i_font_vx(Chip8System& system, Instruction instruction) noexcept;
  static void mov_i_bfont_vx(Chip8System& system, Instruction instruction) noexcept;
  static void mov_i_bcd_vx(Chip8System& system, Instruction instruction) noexcept;
  static void mov_i_vx(Chip8System& system, Instruction instruction) noexcept;
  static void mov_vx_i(Chip8System& system, Instruction instruction) noexcept;
};

#endif
