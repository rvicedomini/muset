from setuptools import setup, Extension
from Cython.Build import cythonize

# List of C scripts to compile
c_scripts = [
    "km_reset",
    "km_reverse",
    "km_select",
    "km_merge",
    "km_fasta",
    "km_diff",
    "km_basic_filter",
    "km_unitig",
    "km_fafmt",
]

extra_compile_args = [
    "-std=c++17",
    "-O3",
    "-ggdb",
    "-pthread",
    "-Wall",
    "-Wextra",
    "-Wno-maybe-uninitialized",
    "-Wno-unused-function",
    "-pedantic",
]


# Cythonize the C files
ext_modules = cythonize([
    Extension(
        f"cython_helpers.{script}",
        sources=[f"cython_helpers/src/{script}.cpp"],
        include_dirs=["cython_helpers/include/*.h"],
        language="c++",
        language_level=3,
        extra_compile_args=extra_compile_args,  # Specify the C++ standard here
    )
    for script in c_scripts
])


setup(
    name="kmat_tools",
    version="0.2",
    entry_points={
        'console_scripts': [
            'kmat_tools = kmat_tools.__main__:main',
        ],
    },
    author="Camila Duitama, Francesco Andreace, Riccardo Vicedomini",
    author_email="cduitama@pasteur.fr",
    description="Python package for creating unitig matrix from fof.txt file and set of FASTA/FASTQ files using kmat_tools",
    url="https://github.com/CamilaDuitama/kmat_tools",
    packages=['kmat_tools'],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
        install_requires=[
        "colorama",
    ],
    ext_modules=ext_modules,
    zip_safe=False,
)