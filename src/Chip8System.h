#ifndef CHIP8_SYSTEM_H
#define CHIP8_SYSTEM_H

#include <array>
#include <cstdint>
#include <stack>
#include <vector>
#include <functional>

enum class CallbackType : std::uint8_t
{
  CHIP8_CALLBACK_EXIT,
  CHIP8_CALLBACK_LORES,
  CHIP8_CALLBACK_HIRES,
};

struct Chip8System
{
  static constexpr std::uint16_t MEMORY_SIZE    { 4096 };
  static constexpr std::uint8_t  REGISTER_COUNT {   16 };
  static constexpr std::uint8_t  NUM_KEYS       { 0x10 };

  static constexpr std::uint8_t LORES_WIDTH  {  64 };
  static constexpr std::uint8_t LORES_HEIGHT {  32 };
  static constexpr std::uint8_t HIRES_WIDTH  { 128 };
  static constexpr std::uint8_t HIRES_HEIGHT {  64 };

  static constexpr std::uint8_t FLAG_REGISTER_IDX{ 0xF };

  std::array<std::uint8_t, MEMORY_SIZE> memory{};
  std::uint16_t program_counter{ 0 };
  std::uint16_t index_register{ 0 };
  std::stack<std::uint16_t> stack;
  std::uint8_t delay_timer{ 0 };
  std::uint8_t sound_timer{ 0 };
  std::array<std::uint8_t, REGISTER_COUNT> registers{};

  std::array<std::uint8_t, NUM_KEYS> keys{};
  std::uint8_t key_released{ 0xFF };
  bool waiting{ false };

  std::vector<std::uint8_t> display;
  std::uint8_t current_width { LORES_WIDTH  };
  std::uint8_t current_height{ LORES_HEIGHT };

  using CallbackFunction = std::function<void(CallbackType callback_type)>;
  CallbackFunction callback_function;

  // Callback function
  void set_callback(CallbackFunction callback_function) {
    this->callback_function = callback_function;
  }
};

#endif
