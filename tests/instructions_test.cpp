#include "Instruction.h"

#include <algorithm>

#include "Chip8Emulator.h"
#include "doctest/doctest.h"
/*
TEST_CASE("testing the CLS instruction") {
  Chip8Two chip8system;

  std::fill(chip8system.display.begin(), chip8system.display.end(), 0x1);
  chip8system.CLS();

  CHECK(std::all_of(chip8system.display.begin(), chip8system.display.end(),
                    [](int i) { return i == 0; }));
}

TEST_CASE("testing the JMP instruction") {
  Chip8Two chip8system;

  std::uint16_t const nnn = 0x300;

  // Full instruction is not required, since we are testing the instruction
  Instruction instruction{nnn};

  chip8system.JMP(instruction);

  CHECK_EQ(nnn, chip8system.pc());
}

TEST_CASE("testing the RET and CALL instructions") {
  Chip8System chip8system;

  std::uint16_t const nnn = 0x300;

  Instruction instruction{nnn};

  chip8system.CALL(instruction);

  CHECK_EQ(nnn, chip8system.pc());

  chip8system.RET();

  CHECK_EQ(0x0, chip8system.pc());
}

TEST_CASE("testing the set and add VX NN instructions") {
  Chip8System chip8system;

  std::uint8_t const x = 0x0;
  std::uint8_t const nn1 = 0xA0;
  std::uint8_t const nn2 = 0x0A;

  Instruction mov_inst{nn1};
  Instruction add_inst{nn2};

  chip8system.MOV_VX_NN(mov_inst);
  CHECK_EQ(chip8system.v(x), nn1);

  chip8system.ADD_VX_NN(add_inst);
  CHECK_EQ(chip8system.v(x), 0xAA);
}

TEST_CASE("testing the conditional skip instructions") {
  Chip8System chip8system;

  std::uint8_t const x = 0x0;
  std::uint8_t const nn1 = 0xF0;
  std::uint8_t const nn2 = 0x00;

  Instruction mov_inst{nn1};
  Instruction zero_inst{nn2};

  chip8system.MOV_VX_NN(mov_inst);

  SUBCASE("the skip instructions which compare with NN behave properly") {
    // Test that a skip successfully occurs when equal
    chip8system.SEQ_VX_NN(mov_inst);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does not occur when not equal
    chip8system.SEQ_VX_NN(zero_inst);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does not occur when equal
    chip8system.SNE_VX_NN(mov_inst);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does occur when not equal
    chip8system.SNE_VX_NN(zero_inst);
    CHECK_EQ(4, chip8system.pc());
  }

  SUBCASE("the skip instructions which compare with VY bahave properly") {
    std::uint8_t const y = 0x1;
    std::uint8_t const a = 0xA;  // using register VA for not equal comparisons

    Instruction mov_y_inst{(y << 8) | nn1};
    Instruction xy_inst{(x << 8) | (y << 4)};
    Instruction xa_inst{(x << 8) | (a << 4)};

    chip8system.MOV_VX_NN(mov_y_inst);

    // Test that a skip successfully occurs when equal
    chip8system.SEQ_VX_VY(xy_inst);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does not occur when not equal
    chip8system.SEQ_VX_VY(xa_inst);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does not occur when equal
    chip8system.SNE_VX_VY(xy_inst);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does occur when not equal
    chip8system.SNE_VX_VY(xa_inst);
    CHECK_EQ(4, chip8system.pc());
  }
}

TEST_CASE("testing logical and arithmetic (0x8) instructions") {
  Chip8System chip8system;

  std::uint8_t const x = 0x0;
  std::uint8_t const y = 0x1;

  std::uint8_t const nn1 = 0x0B;
  std::uint8_t const nn2 = 0xB0;

  Instruction mov_x_inst{nn1};
  Instruction mov_y_inst{(y << 8) | nn2};

  chip8system.MOV_VX_NN(mov_x_inst);
  chip8system.MOV_VX_NN(mov_y_inst);

  Instruction xy_inst{(x << 8) | (y << 4)};

  SUBCASE("the set VX to VY instruction") {
    chip8system.MOV_VX_VY(xy_inst);
    CHECK_EQ(0xB0, chip8system.v(x));
  }

  SUBCASE("the binary OR instruction") {
    chip8system.OR_VX_VY(xy_inst);
    CHECK_EQ(0xBB, chip8system.v(x));
  }

  SUBCASE("the binary AND instruction") {
    chip8system.AND_VX_VY(xy_inst);
    CHECK_EQ(0x00, chip8system.v(x));
  }

  SUBCASE("the binary XOR instruction") {
    chip8system.XOR_VX_VY(xy_inst);
    CHECK_EQ(0xBB, chip8system.v(x));
  }

  SUBCASE("the add VY to VX instruction") {
    chip8system.ADD_VX_VY(xy_inst);
    CHECK_EQ(0xBB, chip8system.v(x));
    // Check carry flag
    CHECK_EQ(0x0, chip8system.v(0xF));
  }

  SUBCASE("the subtract instructions") {
    // chip8system.SUB_VX_VY(x, y);
  }
}
*/
