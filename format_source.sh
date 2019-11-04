#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

FRAMEWORK_DIR="${DIR}/atmos/lib_framework/framework"
GFX_DIR="${DIR}/atmos/lib_gfx/gfx"
GL_DIR="${DIR}/atmos/lib_gl/gl"
IGPU_DIR="${DIR}/atmos/lib_igpu/igpu"
VULKAN_DIR="${DIR}/atmos/lib_vulkan/vulkan"


#for file in *.{h,c,hpp,cpp,cc,mm}
for FORMAT_TARGET in "${FRAMEWORK_DIR}" "${GFX_DIR}" "${GL_DIR}" "${IGPU_DIR}" "${VULKAN_DIR}"
do

	echo "switch directory ${FORMAT_TARGET} ..."
	cd $"${FORMAT_TARGET}"
	for file in **/*.{cpp,h} 
	do 
		echo "Formatting file: $file ..."
		${DIR}/clang-format.exe -style=file --i $file
	done
done