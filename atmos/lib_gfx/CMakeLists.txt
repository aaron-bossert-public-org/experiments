
# specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)
                    
BUILD_LIB(
	gfx 
	"context material mesh mesh/primitives perf perf/material perf/mesh perf/context perf/texture scene texture")

target_link_libraries(gfx igpu framework)
