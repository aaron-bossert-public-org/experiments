
# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

BUILD_LIB(
	vulkan 
	"batch buffer context defines material system texture utility window")

IF (CMAKE_CL_64)

	target_include_directories(vulkan
		PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glfw-3.2.1.bin.WIN64/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/tinyobjloader-master"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/stb-master"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/vulkan/Include/Vulkan"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/VulkanMemoryAllocator-master"

		PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}" 
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/vulkan/Include/Vulkan"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/VulkanMemoryAllocator-master"
	)

	target_link_libraries(vulkan 
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glfw-3.2.1.bin.WIN64/lib-vc2015/glfw3.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/vulkan/Lib/vulkan-1.lib")

ELSE()

	target_include_directories(vulkan
		PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glfw-3.2.1.bin.WIN32/inlcude"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/tinyobjloader-master"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/stb-master"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/vulkan/Include/Vulkan"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/VulkanMemoryAllocator-master"

		PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}" 
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/vulkan/Include/Vulkan"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/VulkanMemoryAllocator-master"
	)
	
	target_link_libraries(vulkan 
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/glfw-3.2.1.bin.WIN32/lib-vc2015/glfw3.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/../third_party/vulkan/Lib32/vulkan-1.lib")

ENDIF()

target_link_libraries(vulkan igpu framework)
