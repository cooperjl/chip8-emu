#include "instructions.h"

#include <cstddef>
#include <iterator>
#include <print>
#include <random>

#include "config.h"
#include "chip8.h"

static std::random_device r;
static std::mt19937 rng{r()};
static std::uniform_int_distribution<uint8_t> dist{0};

void Chip8System::CLS() {
   std::fill(display.begin(), display.end(), 0x0);
 }

void Chip8System::RET() {
  pc_ = stack_.top();
  stack_.pop();
}

void Chip8System::SCROLL_DOWN_N(Instruction instruction) {
  std::uint16_t scroll_n = width * instruction.n();
  // Remove pixels scrolled down off the bottom of the screen
  display.erase(display.end() - scroll_n, display.end());
  // Insert blank pixels at start of vector, pushing the display down
  display.insert(display.begin(), scroll_n, 0x0);
}

void Chip8System::SCROLL_RIGHT() {
  auto iter = display.begin();
  // Iterate row by row, performing scroll
  while (iter != display.end()) {
    // Remove pixels scrolled right off the row
    display.erase(iter + width - 4, iter + width);
    // Insert blank pixels at the start of the row, pushing the display right
    display.insert(iter, 4, 0x0);

    if (std::distance(iter, display.end()) > width) {
      std::advance(iter, width);
    } else {
      iter = display.end();
    }
  }
}

void Chip8System::SCROLL_LEFT() {
  auto iter = display.begin();
  // Iterate row by row, performing scroll
  while (iter != display.end()) {
    // Remove pixels scrolled left off the row
    display.erase(iter, iter + 4);
    // Insert blank pixels at the end of the row, pushing the display left
    display.insert(iter + width - 4, 4, 0x0);

    if (std::distance(iter, display.end()) > width) {
      std::advance(iter, width);
    } else {
      iter = display.end();
    }
  }
}

void Chip8System::LORES() {
  if (callback_function_) {
    callback_function_(CallbackType::CHIP8_CALLBACK_LORES);
  } else {
    std::println(stderr, "WARN: callback not set. SUPER-CHIP lores instruction needs to modify window state");
  }

  width = 64;
  height = 32;
  display.resize(static_cast<size_t>(width * height));
}

void Chip8System::HIRES() {
  if (callback_function_) {
    callback_function_(CallbackType::CHIP8_CALLBACK_HIRES);
  } else {
    std::println(stderr, "WARN: callback not set. SUPER-CHIP hires instruction needs to modify window state");
  }

  width = 128;
  height = 64;
  display.resize(static_cast<size_t>(width * height));
}

void Chip8System::EXIT() {
  if (callback_function_) {
    callback_function_(CallbackType::CHIP8_CALLBACK_EXIT);
  } else {
    std::println(stderr, "WARN: callback not set. SUPER-CHIP exit instruction needs to modify window state");
  }
}

void Chip8System::JMP(Instruction instruction) { pc_ = instruction.nnn(); }

void Chip8System::CALL(Instruction instruction) {
  stack_.push(pc_);
  pc_ = instruction.nnn();
}

void Chip8System::SEQ_VX_NN(Instruction instruction) {
  if (v_.at(instruction.x()) == instruction.nn()) {
    pc_ += 2;
  }
}

void Chip8System::SNE_VX_NN(Instruction instruction) {
  if (v_.at(instruction.x()) != instruction.nn()) {
    pc_ += 2;
  }
}

void Chip8System::SEQ_VX_VY(Instruction instruction) {
  if (v_.at(instruction.x()) == v_.at(instruction.y())) {
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
  if (!Config::shift_quirk) {
    v_.at(instruction.x()) = v_.at(instruction.y());
  }

  uint8_t vx = v_.at(instruction.x());

  v_.at(instruction.x()) >>= 1;  // Perform shift
  v_[0xF] = vx & 0b00000001;     // Set flag register to the bit shifted out
}

void Chip8System::RSB_VX_VY(Instruction instruction) {
  uint8_t vx = v_.at(instruction.x());
  uint8_t vy = v_.at(instruction.y());

  v_.at(instruction.x()) = vy - vx;

  // Set flag register to the borrow value
  v_[0xF] = (vy >= vx) ? 1 : 0;
}

void Chip8System::LSL_VX_VY(Instruction instruction) {
  if (!Config::shift_quirk) {
    v_.at(instruction.x()) = v_.at(instruction.y());
  }

  uint8_t vx = v_.at(instruction.x());

  v_.at(instruction.x()) <<= 1;  // Perform shift
  v_[0xF] = vx >> 7;             // Set flag register to the bit shifted out
}

void Chip8System::SNE_VX_VY(Instruction instruction) {
  if (v_.at(instruction.x()) != v_.at(instruction.y())) {
    pc_ += 2;
  }
}

void Chip8System::MOV_I_NNN(Instruction instruction) { i_ = instruction.nnn(); }

void Chip8System::JMP_VX_NNN(Instruction instruction) {
  if (Config::jump_quirk) {
    pc_ = instruction.nnn() + v_.at(instruction.x());
  } else {
    pc_ = instruction.nnn() + v_.at(0x0);
  }
}

void Chip8System::RND_VX_NN(Instruction instruction) {
  v_.at(instruction.x()) = dist(rng) & instruction.nn();
}

void Chip8System::DRW(Instruction instruction) {
  std::uint8_t vx = v_.at(instruction.x()) % width;
  std::uint8_t vy = v_.at(instruction.y()) % height;

  v_[0xF] = 0;

  bool schip = instruction.n() == 0;

  std::uint8_t n_col = schip ? 16 : 8;
  std::uint8_t n_row = schip ? 16 : instruction.n();

  for (std::uint8_t row = 0; row < n_row && vy + row < height; row++) {
    for (std::uint8_t col = 0; col < n_col && vx + col < width; col++) {
      std::uint8_t byte = 0;
      if (schip) {
        byte = memory_.at(i_ + (2 * row) + (col >= 8 ? 1 : 0));
      } else {
        byte = memory_.at(i_ + row);
      }
      
      // Extract current pixel from byte using bit shift and binary and
      uint8_t pixel = byte & (0b10000000 >> (col >= 8 ? (col & 0x7) : col));
      if (pixel != 0) {
        uint16_t idx = ((vy + row) * width) + (vx + col);
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
  i_ += v_.at(instruction.x());
}

void Chip8System::MOV_I_F_VX(Instruction instruction) {
  // 5 bytes per character, plus memory_ offset
  i_ = (v_.at(instruction.x()) * 0x5);
}

void Chip8System::MOV_I_BF_VX(Instruction instruction) {
  // 5 bytes per character, plus memory_ offset of 0x50
  i_ = (v_.at(instruction.x()) * 0xA) + 0x50;
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

  if (!Config::memory_quirk) {
    i_ = instruction.x() + 1;
  }
}

void Chip8System::MOV_VX_I(Instruction instruction) {
  for (std::uint8_t idx = 0; idx <= instruction.x(); idx++) {
    v_.at(idx) = memory_.at(i_ + idx);
  }

  if (!Config::memory_quirk) {
    i_ = instruction.x() + 1;
  }
}
