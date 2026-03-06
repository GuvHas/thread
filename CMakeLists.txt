cmake_minimum_required(VERSION 3.16)

# Force CHIP/esp-matter to compile with a stable language mode on newer toolchains.
# C++20 avoids C++23 regressions seen with some ESP GCC 14 Windows toolchains
# while keeping required features for current esp_matter/connectedhomeip.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(xiao_esp32c6_matter_thread)
