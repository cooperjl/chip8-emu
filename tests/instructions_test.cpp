#include "doctest/doctest.h"

#include <algorithm>

#include "chip8.h"

TEST_CASE("testing the CLS instruction") {
  Chip8System chip8system;

  chip8system.display.fill(0x1);
  chip8system.CLS();

  CHECK(std::all_of(chip8system.display.begin(), chip8system.display.end(), [](int i) {
    return i == 0;
  }));
}

TEST_CASE("testing the JMP instruction") {
  Chip8System chip8system;

  chip8system.JMP(0x300);

  CHECK_EQ(0x300, chip8system.pc());
}

TEST_CASE("testing the RET and CALL instructions") {
  Chip8System chip8system;

  chip8system.CALL(0x300);

  CHECK_EQ(0x300, chip8system.pc());

  chip8system.RET();

  CHECK_EQ(0x0, chip8system.pc());
}

TEST_CASE("testing the set and add VX NN instructions") {
  Chip8System chip8system;

  std::uint8_t x = 0x0;

  chip8system.MOV_VX_NN(x, 0xA0);
  CHECK_EQ(chip8system.v(x), 0xA0);

  chip8system.ADD_VX_NN(x, 0x0A);
  CHECK_EQ(chip8system.v(x), 0xAA);
}

TEST_CASE("testing the conditional skip instructions") {
  Chip8System chip8system;

  std::uint8_t x = 0x0;
  chip8system.MOV_VX_NN(x, 0xF0);

  SUBCASE("the skip instructions which compare with NN behave properly") {
    // Test that a skip successfully occurs when equal
    chip8system.SEQ_VX_NN(x, 0xF0);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does not occur when not equal
    chip8system.SEQ_VX_NN(x, 0x00);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does not occur when equal
    chip8system.SNE_VX_NN(x, 0xF0);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does occur when not equal
    chip8system.SNE_VX_NN(x, 0x00);
    CHECK_EQ(4, chip8system.pc());
  }

  SUBCASE("the skip instructions which compare with VY bahave properly") {
    std::uint8_t y = 0x1;
    std::uint8_t a = 0xA; // using register VA for not equal comparisons 
    chip8system.MOV_VX_NN(y, 0xF0);

    // Test that a skip successfully occurs when equal
    chip8system.SEQ_VX_VY(x, y);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does not occur when not equal
    chip8system.SEQ_VX_VY(x, a);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does not occur when equal
    chip8system.SNE_VX_VY(x, y);
    CHECK_EQ(2, chip8system.pc());
    // Test that a skip does occur when not equal
    chip8system.SNE_VX_VY(x, a);
    CHECK_EQ(4, chip8system.pc());
  }
}

TEST_CASE("testing logical and arithmetic (0x8) instructions") {
  Chip8System chip8system;

  std::uint8_t x = 0x0;
  std::uint8_t y = 0x1;

  chip8system.MOV_VX_NN(x, 0x0B);
  chip8system.MOV_VX_NN(y, 0xB0);

  SUBCASE("the set VX to VY instruction") {
    chip8system.MOV_VX_VY(x, y);
    CHECK_EQ(0xB0, chip8system.v(x));
  }

  SUBCASE("the binary OR instruction") {
    chip8system.OR_VX_VY(x, y);
    CHECK_EQ(0xBB, chip8system.v(x));
  }

  SUBCASE("the binary AND instruction") {
    chip8system.AND_VX_VY(x, y);
    CHECK_EQ(0x00, chip8system.v(x));
  }

  SUBCASE("the binary XOR instruction") {
    chip8system.XOR_VX_VY(x, y);
    CHECK_EQ(0xBB, chip8system.v(x));
  }

  SUBCASE("the add VY to VX instruction") {
    chip8system.ADD_VX_VY(x, y);
    CHECK_EQ(0xBB, chip8system.v(x));
    // Check carry flag
    CHECK_EQ(0x0, chip8system.v(0xF));
  }

  SUBCASE("the subtract instructions") {
    //chip8system.SUB_VX_VY(x, y);
    
  }
}
