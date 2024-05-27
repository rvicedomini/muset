#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Define the paths
CONDA_BUILD_DIR="/tmp/conda-build"
CONDA_ENV_DIR="/pasteur/appa/homes/cduitama/anaconda3"
PACKAGE_PATTERN="$CONDA_BUILD_DIR/linux-64/kmat_tools*.tar.bz2"

# Source the conda setup script to make conda commands available
source "$CONDA_ENV_DIR/etc/profile.d/conda.sh"

# Activate the specific environment
conda activate $CONDA_ENV_DIR

# Navigate to the conda build directory
cd $CONDA_BUILD_DIR || { echo "Cannot change to directory $CONDA_BUILD_DIR"; exit 1; }

cd $CONDA_BUILD_DIR || { echo "Cannot change to directory $CONDA_BUILD_DIR"; exit 1; }

# Convert the package to supported platforms, skipping Windows platforms
for PACKAGE in $PACKAGE_PATTERN
do
  echo "Converting package: $PACKAGE"
  conda convert --platform osx-64 --platform linux-64 $PACKAGE -o $CONDA_BUILD_DIR
done

# Upload the packages
for PLATFORM in osx-64 osx-arm64 linux-32 linux-64 linux-ppc64 linux-ppc64le linux-s390x linux-armv6l linux-armv7l linux-aarch64
do
  for FILE in $CONDA_BUILD_DIR/$PLATFORM/*.tar.bz2
  do
    if [[ -f "$FILE" ]]; then
      echo "Uploading package: $FILE"
      anaconda upload $FILE --force
    fi
  done
done

echo "All packages have been converted and uploaded successfully!"
