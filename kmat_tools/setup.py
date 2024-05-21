from setuptools import setup,Extension,find_packages
from Cython.Build import cythonize

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

EXTENSION_PATH = "cython_helpers"

ext1 = Extension("km_basic_filter",
                 sources=[f"{EXTENSION_PATH}/km_basic_filter.pyx", f"{EXTENSION_PATH}/lib/src/km_basic_filter.c"],
                 include_dirs=[f"{EXTENSION_PATH}/lib/include"],
                 extra_compile_args=['-Wall', '-Wno-unused-function', '-O3', '-pedantic'],
                 libraries=['z', 'm'])

ext2 = Extension("km_fasta",
                 sources=[f"{EXTENSION_PATH}/km_fasta.pyx", f"{EXTENSION_PATH}/lib/src/km_fasta.c"],
                 include_dirs=[f"{EXTENSION_PATH}/lib/include"],
                 extra_compile_args=['-Wall', '-Wno-unused-function', '-O3', '-pedantic'],
                 libraries=['z', 'm'])

ext3 = Extension("km_unitig",
                 sources=[f"{EXTENSION_PATH}/km_unitig.pyx", f"{EXTENSION_PATH}/lib/src/km_unitig.c"],
                 include_dirs=[f"{EXTENSION_PATH}/lib/include"],
                 extra_compile_args=['-Wall', '-Wno-unused-function', '-O3', '-pedantic'],
                 libraries=['z', 'm'])

ext4 = Extension("km_reset",
                 sources=[f"{EXTENSION_PATH}/km_reset.pyx", f"{EXTENSION_PATH}/lib/src/km_reset.c"],
                 include_dirs=[f"{EXTENSION_PATH}/lib/include"],
                 extra_compile_args=['-Wall', '-Wno-unused-function', '-O3', '-pedantic'],
                 libraries=['z', 'm'])

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="kmat_tools",
    version="0.0.18",
    author="Riccardo Vicedomini, Francesco Andreace, Camila Duitama",
    author_email="cduitama@pasteur.fr",
    include_package_data=True,
    packages=find_packages(include=["kmat_tools", "utils", "cython_helpers", "cython_helpers.km_tools_wrappers"]),
    package_data={"": ["kmat_tools/*",
                       "cython_helpers/*.pxd",
                       "cython_helpers/*.pyx",
                       "cython_helpers/lib/include/*.h",
                       "cython_helpers/lib/src/*.c",
                       "cython_helpers/km_tools_wrappers/*.py"]},
    ext_modules = cythonize([ext1,ext2,ext3,ext4]),
    entry_points={
        "console_scripts": [
            "kmat_tools = kmat_tools.__main__:main",
            'km_basic_filter = cython_helpers.km_tools_wrappers.km_basic_filter_wrapper:km_basic_filter_main',
            'km_fasta = cython_helpers.km_tools_wrappers.km_fasta_wrapper:km_fasta_main',
            'km_unitig = cython_helpers.km_tools_wrappers.km_unitig_wrapper:km_unitig_main'
       ]
    },
        install_requires=[
        "colorama",
        "Cython",
        "wheel",
        "setuptools",
    ],
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/CamilaDuitama/kmat_tools",
    project_urls={
        "Bug Tracker": "https://github.com/CamilaDuitama/kmat_tools/issues",
    },
        classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
)
