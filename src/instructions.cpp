#include <random>

#include "instructions.h"
#include "chip8.h"

static std::random_device r;
static std::mt19937 rng{r()};
static std::uniform_int_distribution<uint8_t> dist{0};

void Chip8System::CLS() {
  display.fill(0x0);
}

void Chip8System::RET() {
  pc_ = stack_.top();
  stack_.pop();
}

void Chip8System::JMP(Instruction instruction) {
  pc_ = instruction.nnn();
}

void Chip8System::CALL(Instruction instruction) {
  stack_.push(pc_);
  pc_ = instruction.nnn();
}

void Chip8System::SEQ_VX_NN(Instruction instruction) {
  if (v_[instruction.x()] == instruction.nn()) {
    pc_ += 2;
  }
}

void Chip8System::SNE_VX_NN(Instruction instruction) {
  if (v_[instruction.x()] != instruction.nn()) {
    pc_ += 2;
  }
}

void Chip8System::SEQ_VX_VY(Instruction instruction) {
  if (v_[instruction.x()] == v_[instruction.y()]) {
    pc_ += 2;
  }
}
void Chip8System::MOV_VX_NN(Instruction instruction) {
  v_.at(instruction.x()) = instruction.nn();
}

void Chip8System::ADD_VX_NN(Instruction instruction) {
  v_.at(instruction.x()) += instruction.nn();
}
void Chip8System::MOV_VX_VY(Instruction instruction) {
  v_.at(instruction.x()) = v_.at(instruction.y());
  
}
void Chip8System::OR_VX_VY(Instruction instruction) {
  v_.at(instruction.x()) |= v_.at(instruction.y());

  // quirk
  v_[0xF] = 0;
}
void Chip8System::AND_VX_VY(Instruction instruction) {
  v_.at(instruction.x()) &= v_.at(instruction.y());


  // quirk
  v_[0xF] = 0;
}
void Chip8System::XOR_VX_VY(Instruction instruction) {
  v_.at(instruction.x()) ^= v_.at(instruction.y());

  // quirk
  v_[0xF] = 0;
}
void Chip8System::ADD_VX_VY(Instruction instruction) {
  uint8_t vx = v_.at(instruction.x());
  uint8_t vy = v_.at(instruction.y());
  
  v_.at(instruction.x()) = vx + vy;

  // Set flag register to the carry value
  v_[0xF] = (vx > 255 - vy) ? 1 : 0;
  
}
void Chip8System::SUB_VX_VY(Instruction instruction) {
  uint8_t vx = v_.at(instruction.x());
  uint8_t vy = v_.at(instruction.y());

  v_.at(instruction.x()) = vx - vy;

  // Set flag register to the borrow value
  v_[0xF] = (vx >= vy) ? 1 : 0;
}

void Chip8System::LSR_VX_VY(Instruction instruction) {
  v_.at(instruction.x()) = v_.at(instruction.y()); // Separate to allow configurability with quirks

  uint8_t vx = v_.at(instruction.x());
  
  v_.at(instruction.x()) >>= 1; // Perform shift
  v_[0xF] = vx & 0b00000001; // Set flag register to the bit shifted out
}

void Chip8System::RSB_VX_VY(Instruction instruction) {
  uint8_t vx = v_.at(instruction.x());
  uint8_t vy = v_.at(instruction.y());

  v_.at(instruction.x()) = vy - vx;

  // Set flag register to the borrow value
  v_[0xF] = (vy >= vx) ? 1 : 0;
}

void Chip8System::LSL_VX_VY(Instruction instruction) {
  v_.at(instruction.x()) = v_.at(instruction.y()); // Separate to allow configurability with quirks

  uint8_t vx = v_.at(instruction.x());
  
  v_.at(instruction.x()) <<= 1; // Perform shift
  v_[0xF] = vx >> 7; // Set flag register to the bit shifted out
}

void Chip8System::SNE_VX_VY(Instruction instruction) {
  if (v_.at(instruction.x()) != v_.at(instruction.y())) {
    pc_ += 2;
  }
}

void Chip8System::MOV_I_NNN(Instruction instruction) {
  i_ = instruction.nnn();
}

void Chip8System::JMP_V0_NNN(Instruction instruction) {
  pc_ = v_[0x0] + instruction.nnn();
}

void Chip8System::RND_VX_NN(Instruction instruction) {
  v_.at(instruction.x()) = dist(rng) & instruction.nn();
}

void Chip8System::DRW(Instruction instruction) {
  std::uint8_t vx = v_.at(instruction.x()) % 64;
  std::uint8_t vy = v_.at(instruction.y()) % 32;

  v_[0xF] = 0;

  for (std::uint8_t row = 0; row < instruction.n() && vy + row < 32; row++) {
    std::uint8_t byte = memory_.at(i_ + row);

    for (std::uint8_t px = 0; px < 8 && vx + px < 64; px++) {
      // Extract current pixel from byte using bit shift and binary and
      if (byte & (0b10000000 >> px)) {
        uint16_t idx = (vy + row) * 64 + (vx + px);
        if (display.at(idx) == 0x01) {
          v_[0xF] = 1;
        }
        display.at(idx) ^= 1;
      }
    }
  }
}

void Chip8System::SPK_VX(Instruction instruction) {
  uint8_t vx = v_.at(instruction.x());

  if (vx < keys.size() && keys.at(vx) == 0x1) {
    pc_ += 2;
  }
}

void Chip8System::SNK_VX(Instruction instruction) {
  uint8_t vx = v_.at(instruction.x());

  if (vx < keys.size() && keys.at(vx) == 0x0) {
    pc_ += 2;
  }
}

void Chip8System::MOV_VX_DT(Instruction instruction) {
  v_.at(instruction.x()) = dt_;
}
void Chip8System::MOV_VX_WAIT_K(Instruction instruction) {
  if (key_released == 0xFF) {
    waiting = true;
    pc_ -= 2;
  } else {
    v_.at(instruction.x()) = key_released;
    waiting = false;
    key_released = 0xFF;
  }
}

void Chip8System::MOV_DT_VX(Instruction instruction) {
  dt_ = v_.at(instruction.x());
}

void Chip8System::MOV_ST_VX(Instruction instruction) {
  st_ = v_.at(instruction.x());
}

void Chip8System::ADD_I_VX(Instruction instruction) {
  // TODO: overflow flag quirk
  i_ += v_.at(instruction.x());
}

void Chip8System::MOV_I_F_VX(Instruction instruction) {
  // 5 bytes per character, plus memory_ offset of 0x50
  i_ = (v_.at(instruction.x()) * 0x05) + 0x50;
}

void Chip8System::MOV_I_BCD_VX(Instruction instruction) {
  memory_.at(i_) = v_.at(instruction.x()) / 100;
  memory_.at(i_ + 1) = (v_.at(instruction.x()) / 10) % 10;
  memory_.at(i_ + 2) = v_.at(instruction.x()) % 10;
}

void Chip8System::MOV_I_VX(Instruction instruction) {
  for (std::uint8_t idx = 0; idx <= instruction.x(); idx++) {
    memory_.at(i_ + idx) = v_.at(idx);
  }
  // quirk
  i_ = instruction.x() + 1;
}

void Chip8System::MOV_VX_I(Instruction instruction) {
  for (std::uint8_t idx = 0; idx <= instruction.x(); idx++) {
    v_.at(idx) = memory_.at(i_ + idx);
  }
  // quirk
  i_ = instruction.x() + 1;
}
