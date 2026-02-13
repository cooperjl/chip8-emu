cmake_minimum_required(VERSION 3.24...4.0)

FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG v2.4.12
)

FetchContent_MakeAvailable(doctest)

add_executable(testlib main.cpp instructions_test.cpp
        1-chip8-logo.cpp)

target_compile_features(testlib PRIVATE cxx_std_23)

target_link_libraries(testlib PRIVATE doctest)

target_link_libraries(testlib PRIVATE chip8-lib)

add_test(NAME instructions_test COMMAND testlib)
