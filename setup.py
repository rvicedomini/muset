from setuptools import setup, Extension
import subprocess
from Cython.Build import cythonize

# List of C scripts to compile
c_scripts = [
    "km_reverse",
    "km_select",
    "km_tools",
    "km_merge",
    "km_fasta",
    "km_diff",
    "km_basic_filter",
    "km_unitig"
]

# Cythonize the C files
ext_modules = cythonize([
    Extension(
        f"kmat_tools.src.{script}",
        sources=[f"kmat_tools/src/{script}.c"],
        include_dirs=["kmat_tools/include/"],
    )
    for script in c_scripts
])


setup(
    name="kmat_tools",
    version="0.1",
    entry_points={
        'console_scripts': [
            'kmat_tools = kmat_tools.__main__:main',
        ],
    },
    author="Camila Duitama, Francesco Andreace, Riccardo Vicedomini",
    author_email="cduitama@pasteur.fr",
    description="Python package for creating unitig matrix from fof.txt file and set of FASTA/FASTQ files using kmat_tools",
    url="https://github.com/CamilaDuitama/kmat_tools",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
        install_requires=[
        "colorama",
    ],
    packages=['kmat_tools'],
    ext_modules=ext_modules,
    zip_safe=False,
)