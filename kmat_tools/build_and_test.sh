#!/bin/bash
module load gcc
module load zlib

# Run the build command
python -m build

#Remove that is in dist
rm -rf distpoip

# Install the built package
pip install dist/*.tar.gz

# Run test commands
kmat_tools -h
km_fasta -h
km_unitig -h
km_basic_filter -h
# Run the kmat_tools command
#kmat_tools data/fof.txt