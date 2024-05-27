#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Ensure that the script receives one argument (the path to the recipe)
if [ "$#" -ne 1 ]; then
    echo "Error: You must provide the path to the recipe directory."
    echo "Usage: $0 <path_to_recipe>"
    exit 1
fi

RECIPE_DIR=$1

# Check if the provided path is a directory
if [ ! -d "$RECIPE_DIR" ]; then
    echo "Error: The path provided is not a directory or does not exist."
    echo "Usage: $0 <path_to_recipe>"
    exit 1
fi

# Purge old builds
echo "Purging old Conda builds..."
conda build purge-all

# Build the recipe
echo "Building the Conda recipe from directory: $RECIPE_DIR"
conda build --croot /tmp/conda-build "$RECIPE_DIR"

echo "Build completed successfully."
