#ifndef CHIP8_SYSTEM_H
#define CHIP8_SYSTEM_H

#include "Instruction.h"

#include <array>
#include <functional>
#include <stack>
#include <vector>

enum class CallbackType : std::uint8_t {
    CHIP8_CALLBACK_EXIT,
    CHIP8_CALLBACK_LORES,
    CHIP8_CALLBACK_HIRES,
};

class Chip8System {
public:
    Chip8System() = default;

    static constexpr std::uint16_t MEMORY_SIZE{4096};
    static constexpr std::uint8_t REGISTER_COUNT{16};
    static constexpr std::uint8_t NUM_KEYS{0x10};

    static constexpr std::uint8_t LORES_WIDTH{64};
    static constexpr std::uint8_t LORES_HEIGHT{32};
    static constexpr std::uint8_t HIRES_WIDTH{128};
    static constexpr std::uint8_t HIRES_HEIGHT{64};

    static constexpr std::uint8_t FLAG_REGISTER_IDX{0xF};

    std::array<std::uint8_t, MEMORY_SIZE> memory{};
    std::uint16_t program_counter{0};
    std::uint16_t index_register{0};
    std::stack<std::uint16_t> stack;
    std::uint8_t delay_timer{0};
    std::uint8_t sound_timer{0};
    std::array<std::uint8_t, REGISTER_COUNT> registers{};

    std::array<std::uint8_t, NUM_KEYS> keys{};
    std::uint8_t key_released{0xFF};
    bool waiting{false};

    std::vector<std::uint8_t> display;
    std::uint8_t current_width{LORES_WIDTH};
    std::uint8_t current_height{LORES_HEIGHT};

    using CallbackFunction = std::function<void(CallbackType callback_type)>;
    CallbackFunction callback_function;

    // Callback function
    void set_callback(CallbackFunction callback_function) {
        this->callback_function = callback_function;
    }

    void sc_down(Instruction instruction) noexcept;
    void cls(Instruction instruction) noexcept;
    void ret(Instruction instruction) noexcept;
    void sc_right(Instruction instruction) noexcept;
    void sc_left(Instruction instruction) noexcept;
    void exit(Instruction instruction) noexcept;
    void lores(Instruction instruction) noexcept;
    void hires(Instruction instruction) noexcept;
    void jmp(Instruction instruction) noexcept;
    void call(Instruction instruction) noexcept;
    void seq_vx_nn(Instruction instruction) noexcept;
    void sne_vx_nn(Instruction instruction) noexcept;
    void seq_vx_vy(Instruction instruction) noexcept;
    void mov_vx_nn(Instruction instruction) noexcept;
    void add_vx_nn(Instruction instruction) noexcept;
    void mov_vx_vy(Instruction instruction) noexcept;
    void or_vx_vy(Instruction instruction) noexcept;
    void and_vx_vy(Instruction instruction) noexcept;
    void xor_vx_vy(Instruction instruction) noexcept;
    void add_vx_vy(Instruction instruction) noexcept;
    void sub_vx_vy(Instruction instruction) noexcept;
    void shr_vx_vy(Instruction instruction) noexcept;
    void rsb_vx_vy(Instruction instruction) noexcept;
    void shl_vx_vy(Instruction instruction) noexcept;
    void sne_vx_vy(Instruction instruction) noexcept;
    void mov_i_nnn(Instruction instruction) noexcept;
    void jmp_vx_nnn(Instruction instruction) noexcept;
    void rnd_vx_nn(Instruction instruction) noexcept;
    void drw(Instruction instruction) noexcept;
    void spr_vx(Instruction instruction) noexcept;
    void sup_vx(Instruction instruction) noexcept;
    void mov_vx_dt(Instruction instruction) noexcept;
    void wait_mov_vx_key(Instruction instruction) noexcept;
    void mov_dt_vx(Instruction instruction) noexcept;
    void mov_st_vx(Instruction instruction) noexcept;
    void add_i_vx(Instruction instruction) noexcept;
    void mov_i_font_vx(Instruction instruction) noexcept;
    void mov_i_bfont_vx(Instruction instruction) noexcept;
    void mov_i_bcd_vx(Instruction instruction) noexcept;
    void mov_i_vx(Instruction instruction) noexcept;
    void mov_vx_i(Instruction instruction) noexcept;
};

#endif // CHIP8_SYSTEM_H
