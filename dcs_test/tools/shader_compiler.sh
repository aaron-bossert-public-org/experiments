#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

RAW_DIR="${DIR}/../raw_assets/shaders"
COOKED_DIR="${DIR}/../cooked_assets/shaders"

echo "RAW_DIR: ${RAW_DIR}"
echo "COOKED_DIR: ${COOKED_DIR}"

cd $RAW_DIR
for i in *;do C:/VulkanSDK/1.1.106.0/Bin32/glslangValidator.exe -V "$i" -o "${COOKED_DIR}/${i}.spv";done
#find "${RAW_DIR}/" -name "*.*" -exec cmd

# | echo 


# find "${RAW_DIR}/" | while read FILE; do
#     magick convert -quality 75% "$FILE" "$FILE"    
# done





# C:/VulkanSDK/1.1.106.0/Bin32/glslangValidator.exe


# echo "remove/make ${BUILD_DIR}"

# rm -rf ${BUILD_DIR}
# mkdir ${BUILD_DIR}

