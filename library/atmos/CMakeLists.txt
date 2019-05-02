# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)


# atmos_src construction
foreach(folder app compute concurrent model perf shader system tests texture)
	file(GLOB files
		"${CMAKE_CURRENT_SOURCE_DIR}/${folder}/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/${folder}/*.cpp"
		)

	set(atmos_src ${atmos_src} ${files})
endforeach()


# atmos creation
add_library(atmos STATIC ${atmos_src})

target_include_directories(atmos 
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/.."
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm"
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/gli"
	INTERFACE
		"${CMAKE_CURRENT_SOURCE_DIR}/.."

)

add_subdirectory(platform)
message("ATMOS_PLATFORM " ${ATMOS_PLATFORM})
target_link_libraries(atmos ${ATMOS_PLATFORM})

WARNINGS_AS_ERRORS(atmos)