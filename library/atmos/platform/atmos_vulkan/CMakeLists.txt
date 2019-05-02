# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

# atmos_vulkan_src construction
foreach(folder app compute model perf shader system texture)
	file(GLOB files
		"${CMAKE_CURRENT_SOURCE_DIR}/${folder}/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/${folder}/*.cpp"
		)

	set(atmos_vulkan_src ${atmos_vulkan_src} ${files})
endforeach()

# atmos_vulkan creation
add_library(atmos_vulkan STATIC ${atmos_vulkan_src})
target_include_directories(atmos_vulkan 
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/.."
		"${CMAKE_CURRENT_SOURCE_DIR}/../../.."
		"${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/glm")

IF (CMAKE_CL_64)

	target_include_directories(atmos_vulkan PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.2.1.bin.WIN64/include")
	target_link_libraries(atmos_vulkan 
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.2.1.bin.WIN64/lib-vc2015/glfw3.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/vulkan/Lib/vulkan-1.lib")

ELSE()

	target_include_directories(atmos_vulkan PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.2.1.bin.WIN32/inlcude")
	target_link_libraries(atmos_vulkan 
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.2.1.bin.WIN32/lib-vc2015/glfw3.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/vulkan/Lib32/vulkan-1.lib")

ENDIF()

set(ATMOS_PLATFORM "atmos_vulkan"
	CACHE INTERNAL "Platform implementation library to link atmos against" FORCE
)

WARNINGS_AS_ERRORS(atmos_vulkan)