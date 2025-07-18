#include "chip8.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <print>
#include <sstream>

#include "config.h"

Chip8System::Chip8System() {
  // Reserve max potential size TODO only larger if superchip
  display.reserve(128 * 64);
  // Size to base size
  display.resize(static_cast<size_t>(width * height));
  // read font in
  std::copy(font.begin(), font.end(), std::begin(memory_));
  std::copy(big_font.begin(), big_font.end(), std::begin(memory_) + font.size());

  Config::load_schip();
}

void Chip8System::load_rom(std::string &filename) {
  std::uint16_t const start_idx = 0x200;

  if (std::ifstream file{filename, std::ios::binary}) {
    std::println("Loading rom from path: {}", filename);
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string rom = buffer.str();

    std::copy(std::begin(rom), std::end(rom), std::begin(memory_) + start_idx);
  } else {
    throw std::runtime_error(std::string("ERROR: file read went wrong"));
  }

  pc_ = start_idx;
}

auto Chip8System::decode(Instruction instruction) -> int {
  switch (instruction.opcode()) {
    case 0x00:
      if (instruction.y() == 0xC) {
        SCROLL_DOWN_N(instruction);
        return 0;
      }
      switch (instruction.nn()) {
        case 0xE0:
          CLS();
          return 0;
        case 0xEE:
          RET();
          return 0;
        case 0xFB:
          SCROLL_RIGHT();
          return 0;
        case 0xFC:
          SCROLL_LEFT();
          return 0;
        case 0xFD:
          EXIT();
          return 0;
        case 0xFE:
          LORES();
          return 0;
        case 0xFF:
          HIRES();
          return 0;
        default:
          break;
      }
      break;
    case 0x01:
      JMP(instruction.nnn());
      return 0;
    case 0x02:
      CALL(instruction.nnn());
      return 0;
    case 0x03:
      SEQ_VX_NN(instruction);
      return 0;
    case 0x04:
      SNE_VX_NN(instruction);
      return 0;
    case 0x05:
      SEQ_VX_VY(instruction);
      return 0;
    case 0x06:
      MOV_VX_NN(instruction);
      return 0;
    case 0x07:
      ADD_VX_NN(instruction);
      return 0;
    case 0x08:
      switch (instruction.n()) {
        case 0x00:
          MOV_VX_VY(instruction);
          return 0;
        case 0x01:
          OR_VX_VY(instruction);
          return 0;
        case 0x02:
          AND_VX_VY(instruction);
          return 0;
        case 0x03:
          XOR_VX_VY(instruction);
          return 0;
        case 0x04:
          ADD_VX_VY(instruction);
          return 0;
        case 0x05:
          SUB_VX_VY(instruction);
          return 0;
        case 0x06:
          LSR_VX_VY(instruction);
          return 0;
        case 0x07:
          RSB_VX_VY(instruction);
          return 0;
        case 0x0E:
          LSL_VX_VY(instruction);
          return 0;
        default:
          break;
      }
      break;
    case 0x09:
      SNE_VX_VY(instruction);
      return 0;
    case 0x0A:
      MOV_I_NNN(instruction);
      return 0;
    case 0x0B:
      JMP_VX_NNN(instruction);
    case 0x0C:
      RND_VX_NN(instruction);
      return 0;
    case 0x0D:
      DRW(instruction);
      return 0;
    case 0x0E:
      switch (instruction.nn()) {
        case 0x9E:
          SPK_VX(instruction);
          return 0;
        case 0xA1:
          SNK_VX(instruction);
          return 0;
        default:
          break;
      }
      break;
    case 0x0F:
      switch (instruction.nn()) {
        case 0x07:
          MOV_VX_DT(instruction);
          return 0;
        case 0x0A:
          MOV_VX_WAIT_K(instruction);
          return 0;
        case 0x15:
          MOV_DT_VX(instruction);
          return 0;
        case 0x18:
          MOV_ST_VX(instruction);
          return 0;
        case 0x1E:
          ADD_I_VX(instruction);
          return 0;
        case 0x29:
          MOV_I_F_VX(instruction);
          return 0;
        case 0x30:
          MOV_I_BF_VX(instruction);
          return 0;
        case 0x33:
          MOV_I_BCD_VX(instruction);
          return 0;
        case 0x55:
          MOV_I_VX(instruction);
          return 0;
        case 0x65:
          MOV_VX_I(instruction);
          return 0;
        default:
          break;
      }
      break;
    default:
      break;
  }
  // If here, invalid instruction
  std::println(stderr,
               "Error: Invalid (or unimplemented) chip8 instruction: {:04X}",
               instruction.data());
  return -1;
}

auto Chip8System::update_timers() -> bool {
  if (dt_ > 0) {
    dt_--;
  }
  if (st_ > 0) {
    st_--;
  }
  return st_ > 0;
}

auto Chip8System::get_cur_inst() -> Instruction {
  // Fetch current instruction from memory using value at pc_ and pc_ + 1
  // Instruction cur_inst {memory_.at(pc_), memory_.at(pc_ + 1)};
  // return cur_inst;
  return Instruction{memory_.at(pc_), memory_.at(pc_ + 1)};
}

auto Chip8System::cycle() -> int {
  Instruction cur_inst = get_cur_inst();
  // Increment PC
  pc_ += 2;
  // Decode and execute instruction
  return decode(cur_inst);
}
