#!/bin/bash
set -e
set -x

mkdir -p ${PREFIX}/bin

mkdir build-conda
cd build-conda
cmake -DCONDA_BUILD=OFF ..
make -j4
cd ..

echo "Current directory: ${PWD}"
ls -lh

cp ./bin/kmat_tools ${PREFIX}/bin
cp ./bin/muset ${PREFIX}/bin
