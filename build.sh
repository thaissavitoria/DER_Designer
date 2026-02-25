#!/bin/bash
# filepath: build.sh

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' 

echo -e "${YELLOW}DER Designer Build Script for Ubuntu${NC}"

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: cmake is not installed${NC}"
    echo "Install it with: sudo apt-get install cmake"
    exit 1
fi

if ! command -v qmake6 &> /dev/null && ! find /usr/lib -name "Qt6Core*" -o -name "qt6-config.cmake" 2>/dev/null | grep -q .; then
    echo -e "${RED}Error: Qt6 development files may not be installed${NC}"
    echo "Install with: sudo apt-get install qt6-base-dev qt6-tools-dev"
    # Don't exit, let cmake try anyway
fi

BUILD_DIR="cmake-build-linux"

if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo -e "${YELLOW}Configuring project...${NC}"
cmake -DCMAKE_BUILD_TYPE=Release ..

echo -e "${YELLOW}Building project...${NC}"
cmake --build . -j$(nproc)

echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${GREEN}Executable: ./cmake-build-linux/DERDesigner${NC}"