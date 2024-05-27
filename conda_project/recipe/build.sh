#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Print all executed commands
set -x

# Create and move to the build directory
mkdir -p build
cd build

# Configure the project, specifying the installation prefix
cmake -DCMAKE_INSTALL_PREFIX=$PREFIX ..

# Compile the project
make -j${CPU_COUNT}

# Print debug information about installation paths
echo "CMAKE_INSTALL_PREFIX: $PREFIX"
echo "Build directory: $(pwd)"
echo "File paths pre-install:"
find . -name 'kmtools*'


# Install the project to the specified prefix
make install

echo "Contents of the $PREFIX/bin directory after installation:"
ls -la $PREFIX/bin

# Verify the installation of kmtools
if [[ -f "$PREFIX/bin/kmtools" ]]; then
  echo "kmtools installed successfully"
else
  echo "Error: kmtools not found in $PREFIX/bin"
  exit 1
fi
