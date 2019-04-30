# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

# vulkan_src construction
foreach(folder app compute model shader system texture)
	file(GLOB files
		"${CMAKE_CURRENT_SOURCE_DIR}/${folder}/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/${folder}/*.cpp"
		)

	set(vulkan_src ${vulkan_src} ${files})
endforeach()

# vulkan creation
add_library(vulkan STATIC ${vulkan_src})
target_include_directories(vulkan 
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}/../../.."
		"${CMAKE_CURRENT_SOURCE_DIR}/../../third_party"
		"${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/glm"
		"${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/gli"
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/vulkan/Include")

target_include_directories(vulkan PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/vulkan/Include")

IF (CMAKE_CL_64)

	target_include_directories(vulkan PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.2.1.bin.WIN64/include")
	target_link_libraries(vulkan 
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.2.1.bin.WIN64/lib-vc2015/glfw3.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/vulkan/Lib/vulkan-1.lib")

ELSE()

	target_include_directories(vulkan PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.2.1.bin.WIN32/inlcude")
	target_link_libraries(vulkan 
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.2.1.bin.WIN32/lib-vc2015/glfw3.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/third_party/vulkan/Lib32/vulkan-1.lib")

ENDIF()

set(ATMOS_PLATFORM "vulkan"
	CACHE INTERNAL "Platform implementation library to link atmos against" FORCE
)