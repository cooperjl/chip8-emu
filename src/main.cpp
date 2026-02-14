#include <cstdlib>
#include <filesystem>
#include <print>

#include "window/window.h"

int main(int const argc, char const* const argv[]) {
    std::string filename{};
    // Right now, only want to get the rom path
    for (int i = 1; i < argc; i++) {
        if (i == argc - 1) {
            filename = argv[i];
        }
    }

    if (!std::filesystem::exists(filename)) {
        std::println(stderr, "Error: file not found at path: {}", filename);

        return EXIT_FAILURE;
    }

    Window window{filename};

    window.main_loop();

    return EXIT_SUCCESS;
}
