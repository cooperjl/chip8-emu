#include <algorithm>
#include <array>
#include <iterator>
#include <fstream>
#include <print>
#include <sstream>

#include "chip8.h"

Chip8System::Chip8System() {
  // read font in
  std::copy(std::begin(font), std::end(font), std::begin(memory_) + 0x50);
}

void Chip8System::load_rom(std::string &filename) {  
  if (std::ifstream file{ filename, std::ios::binary}) {
    std::println("Loading rom from path: {}", filename);
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string rom = buffer.str();

    std::copy(std::begin(rom), std::end(rom), std::begin(memory_) + 0x200);
  } else {
    throw std::runtime_error(std::string("ERROR: file read went wrong"));
  }

  pc_ = 0x200;
}

int Chip8System::decode(Instruction instruction) {
  switch (instruction.opcode()) {
    case 0x00:
      switch (instruction.nnn()) {
        case 0x0E0:
          CLS();
          return 0;
        case 0x0EE:
          RET();
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
      SEQ_VX_NN(instruction.x(), instruction.nn());
      return 0;
    case 0x04:
      SNE_VX_NN(instruction.x(), instruction.nn());
      return 0;
    case 0x05:
      SEQ_VX_VY(instruction.x(), instruction.y());
      return 0;
    case 0x06:
      MOV_VX_NN(instruction.x(), instruction.nn());
      return 0;
    case 0x07:
      ADD_VX_NN(instruction.x(), instruction.nn());
      return 0;
    case 0x08:
      switch (instruction.n()) {
        case 0x00:
          MOV_VX_VY(instruction.x(), instruction.y());
          return 0;
        case 0x01:
          OR_VX_VY(instruction.x(), instruction.y());
          return 0;
        case 0x02:
          AND_VX_VY(instruction.x(), instruction.y());
          return 0;
        case 0x03:
          XOR_VX_VY(instruction.x(), instruction.y());
          return 0;
        case 0x04:
          ADD_VX_VY(instruction.x(), instruction.y());
          return 0;
        case 0x05:
          SUB_VX_VY(instruction.x(), instruction.y());
          return 0;
        case 0x06:
          LSR_VX_VY(instruction.x(), instruction.y());
          return 0;
        case 0x07:
          RSB_VX_VY(instruction.x(), instruction.y());
          return 0;
        case 0x0E:
          LSL_VX_VY(instruction.x(), instruction.y());
          return 0;
        default:
          break;
      }
      break;
    case 0x09:
      SNE_VX_VY(instruction.x(), instruction.y());
      return 0;
    case 0x0A:
      MOV_I_NNN(instruction.nnn());
      return 0;
    case 0x0B:
      JMP_V0_NNN(instruction.nnn());
    case 0x0C:
      RND_VX_NN(instruction.x(), instruction.nn());      
      return 0;
    case 0x0D:
      DRW(instruction.x(), instruction.y(), instruction.n());
      return 0;
    case 0x0E:
      switch (instruction.nn()) {
        case 0x9E:
          SPK_VX(instruction.x());
          return 0;
        case 0xA1:
          SNK_VX(instruction.x());
          return 0;
        default:
          break;
      }
      break;
    case 0x0F:
      switch (instruction.nn()) {
        case 0x07:
          MOV_VX_DT(instruction.x());
          return 0;
        case 0x0A:
          MOV_VX_WAIT_K(instruction.x());
          return 0;
        case 0x15:
          MOV_DT_VX(instruction.x());
          return 0;
        case 0x18:
          MOV_ST_VX(instruction.x());
          return 0;
        case 0x1E:
          ADD_I_VX(instruction.x());
          return 0;
        case 0x29:
          MOV_I_F_VX(instruction.x());
          return 0;
        case 0x33:
          MOV_I_BCD_VX(instruction.x());
          return 0;
        case 0x55:
          MOV_I_VX(instruction.x());
          return 0;
        case 0x65:
          MOV_VX_I(instruction.x());
          return 0;
        default:
          break;
      }
      break;
    default:
      break;
  }
  // If here, invalid instruction
  std::println(stderr, "Error: Invalid (or unimplemented) chip8 instruction: {:04X}", instruction.data);
  return -1;
}

bool Chip8System::update_timers() {
  if (dt_ > 0) {
    dt_--;
  }
  if (st_ > 0) {
    st_--;
  }
  return st_ > 0;
}
  
int Chip8System::cycle() {
  // Fetch current instruction from memory
  Instruction cur_inst = memory_[pc_] << 8 | memory_[pc_ + 1];
  // Increment PC
  pc_ += 2;
  // Decode and execute instruction
  return decode(cur_inst);
}

