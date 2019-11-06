#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BUILD_DIR="${DIR}/build"

echo "remove/make ${BUILD_DIR}"

rm -rf ${BUILD_DIR}
mkdir ${BUILD_DIR}

cmake -G "Visual Studio 16 2019" -S ${DIR} -B ${BUILD_DIR}

start "${BUILD_DIR}/experiments.sln"
