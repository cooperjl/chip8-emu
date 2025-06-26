#include <random>

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

void Chip8System::JMP(std::uint16_t nnn) {
  pc_ = nnn;
}

void Chip8System::CALL(std::uint16_t nnn) {
  stack_.push(pc_);
  pc_ = nnn;
}

void Chip8System::SEQ_VX_NN(std::uint8_t x, std::uint8_t nn) {
  if (v_[x] == nn) {
    pc_ += 2;
  }
}

void Chip8System::SNE_VX_NN(std::uint8_t x, std::uint8_t nn) {
  if (v_[x] != nn) {
    pc_ += 2;
  }
}

void Chip8System::SEQ_VX_VY(std::uint8_t x, std::uint8_t y) {
  if (v_[x] == v_[y]) {
    pc_ += 2;
  }
}
void Chip8System::MOV_VX_NN(std::uint8_t x, std::uint8_t nn) {
  v_[x] = nn;
}

void Chip8System::ADD_VX_NN(std::uint8_t x, std::uint8_t nn) {
  v_[x] += nn;
}
void Chip8System::MOV_VX_VY(std::uint8_t x, std::uint8_t y) {
  v_[x] = v_[y];
  
}
void Chip8System::OR_VX_VY(std::uint8_t x, std::uint8_t y) {
  v_[x] |= v_[y];

  // quirk
  v_[0xF] = 0;
}
void Chip8System::AND_VX_VY(std::uint8_t x, std::uint8_t y) {
  v_[x] &= v_[y];

  // quirk
  v_[0xF] = 0;
}
void Chip8System::XOR_VX_VY(std::uint8_t x, std::uint8_t y) {
  v_[x] ^= v_[y];

  // quirk
  v_[0xF] = 0;
}
void Chip8System::ADD_VX_VY(std::uint8_t x, std::uint8_t y) {
  uint8_t vx = v_[x];
  uint8_t vy = v_[y];
  
  v_[x] = vx + vy;

  // Set flag register to the carry value
  v_[0xF] = (vx > 255 - vy) ? 1 : 0;
  
}
void Chip8System::SUB_VX_VY(std::uint8_t x, std::uint8_t y) {
  uint8_t vx = v_[x];
  uint8_t vy = v_[y];

  v_[x] = vx - vy;

  // Set flag register to the borrow value
  v_[0xF] = (vx >= vy) ? 1 : 0;
}

void Chip8System::LSR_VX_VY(std::uint8_t x, std::uint8_t y) {
  v_[x] = v_[y]; // Separate to allow configurability with quirks

  uint8_t vx = v_[x];
  
  v_[x] >>= 1; // Perform shift
  v_[0xF] = vx & 0b00000001; // Set flag register to the bit shifted out
}

void Chip8System::RSB_VX_VY(std::uint8_t x, std::uint8_t y) {
  uint8_t vx = v_[x];
  uint8_t vy = v_[y];

  v_[x] = vy - vx;

  // Set flag register to the borrow value
  v_[0xF] = (vy >= vx) ? 1 : 0;
}

void Chip8System::LSL_VX_VY(std::uint8_t x, std::uint8_t y) {
  v_[x] = v_[y]; // Separate to allow configurability with quirks

  uint8_t vx = v_[x];
  
  v_[x] <<= 1; // Perform shift
  v_[0xF] = vx >> 7; // Set flag register to the bit shifted out
}

void Chip8System::SNE_VX_VY(std::uint8_t x, std::uint8_t y) {
  if (v_[x] != v_[y]) {
    pc_ += 2;
  }
}

void Chip8System::MOV_I_NNN(std::uint16_t nnn) {
  i_ = nnn;
}

void Chip8System::JMP_V0_NNN(std::uint16_t nnn) {
  pc_ = v_[0x0] + nnn;
}

void Chip8System::RND_VX_NN(std::uint8_t x, std::uint8_t nn) {
  v_[x] = dist(rng) & nn;
}

void Chip8System::DRW(std::uint8_t x, std::uint8_t y, std::uint8_t n) {
  std::uint8_t vx = v_[x] % 64;
  std::uint8_t vy = v_[y] % 32;

  v_[0xF] = 0;

  for (std::uint8_t row = 0; row < n && vy + row < 32; row++) {
    std::uint8_t byte = memory_[i_ + row];

    for (std::uint8_t px = 0; px < 8 && vx + px < 64; px++) {
      // Extract current pixel from byte using bit shift and binary and
      if (byte & (0b10000000 >> px)) {
        uint16_t idx = (vy + row) * 64 + (vx + px);
        if (display[idx] == 0x01) {
          v_[0xF] = 1;
        }
        display[idx] ^= 1;
      }
    }
  }
}

void Chip8System::SPK_VX(std::uint8_t x) {
  if (v_[x] < keys.size() && keys.at(v_[x]) == 0x1) {
    pc_ += 2;
  }
}
void Chip8System::SNK_VX(std::uint8_t x) {
  if (v_[x] < keys.size() && keys.at(v_[x]) == 0x0) {
    pc_ += 2;
  }
}
void Chip8System::MOV_VX_DT(std::uint8_t x) {
  v_[x] = dt_;
}
void Chip8System::MOV_VX_WAIT_K(std::uint8_t x) {
  if (key_released == 0xFF) {
    waiting = true;
    pc_ -= 2;
  } else {
    v_[x] = key_released;
    waiting = false;
    key_released = 0xFF;
  }
}

void Chip8System::MOV_DT_VX(std::uint8_t x) {
  dt_ = v_[x];
}
void Chip8System::MOV_ST_VX(std::uint8_t x) {
  st_ = v_[x];
}
void Chip8System::ADD_I_VX(std::uint8_t x) {
  // TODO: overflow flag quirk
  i_ += v_[x];
}
void Chip8System::MOV_I_F_VX(std::uint8_t x) {
  // 5 bytes per character, plus memory_ offset of 0x50
  i_ = (v_[x] * 0x05) + 0x50;
}
void Chip8System::MOV_I_BCD_VX(std::uint8_t x) {
  memory_[i_] = v_[x] / 100;
  memory_[i_ + 1] = (v_[x] / 10) % 10;
  memory_[i_ + 2] = v_[x] % 10;
}
void Chip8System::MOV_I_VX(std::uint8_t x) {
  for (std::uint8_t idx = 0; idx <= x; idx++) {
    memory_[i_ + idx] = v_[idx];
  }
  // quirk
  i_ = x + 1;
}
void Chip8System::MOV_VX_I(std::uint8_t x) {
  for (std::uint8_t idx = 0; idx <= x; idx++) {
    v_[idx] = memory_[i_ + idx];
  }
  // quirk
  i_ = x + 1;
}
