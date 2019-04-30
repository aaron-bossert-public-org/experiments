# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

IF (WIN32)
	add_subdirectory(vulkan)
ELSE()
	message(FATAL_ERROR "Non windows platforms not implemented yet!")
ENDIF()
