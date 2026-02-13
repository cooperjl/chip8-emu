FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.16
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        FIND_PACKAGE_ARGS NAMES SDL3
)

FetchContent_MakeAvailable(SDL3)

set(SOURCES Config.cpp
        Chip8System.cpp
        Chip8Emulator.cpp
        Beeper.cpp
        Window.cpp
)

add_library(chip8-lib ${SOURCES})

target_compile_features(chip8-lib PUBLIC cxx_std_23)

target_include_directories(chip8-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(chip8-lib PUBLIC SDL3)

add_executable(chip8-app main.cpp)

target_compile_features(chip8-app PUBLIC cxx_std_23)

target_link_libraries(chip8-app PUBLIC chip8-lib)
