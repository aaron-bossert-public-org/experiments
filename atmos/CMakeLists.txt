# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

add_subdirectory(lib_framework)
add_subdirectory(lib_igpu)
add_subdirectory(lib_gl)
add_subdirectory(lib_vulkan)