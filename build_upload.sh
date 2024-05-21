#!/bin/bash

# Script: <build_upload>
# Description: <Script to build and upload to testpy and pypi>
# Author: <Camila Duitama>
# Date: <13th of May 2024>

rm -rf dist/

python -m build

python3 -m twine upload --repository testpypi dist/kmat_tools-0.0.*.tar.gz 

pip install --index-url https://test.pypi.org/simple/ --extra-index-url https://pypi.org/simple/ kmat-tools
