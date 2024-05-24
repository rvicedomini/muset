#!/bin/bash
set -e
module load gcc/11.3.0
module load cmake/3.27.7

#Remove that is in dist
rm -rf dist/*
rm -rf build/*

# Run the build command
python -m build

# Install the built package
pip install dist/*.tar.gz

# Run test commands
kmat_tools -h
km_fasta -h
km_unitig -h
km_basic_filter -h
km_fafmt -h

# Run the kmat_tools command
kmat_tools data/fof.txt