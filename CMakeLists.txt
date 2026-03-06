cmake_minimum_required(VERSION 3.16)

# Force a stable C++ standard across all components (including esp_matter/CHIP)
# to avoid toolchain-specific C++20/23 regressions seen with some GCC 14 builds.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(xiao_esp32c6_matter_thread)