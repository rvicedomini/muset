# kmtools

A collection of C++ tools to process k-mer matrices (e.g., built using [kmtricks](https://github.com/tlemane/kmtricks)) in text format.

+ [Installation](#installation)
+ [Usage](#usage)
+ [k-mer matrix operations](#k-mer-matrix-operations)
+ [Unitig matrix operations](#unitig-matrix-operations)
+ [Unitig matrix construction](#unitig-matrix-construction-pipeline)
    - [Required tools](#required-tools)
    - [Build a k-mer matrix](#1-build-a-k-mer-matrix)
    - [Output a sorted text matrix](#2-output-a-sorted-text-matrix)
    - [Filter the k-mer matrix](#3-filter-the-k-mer-matrix)
    - [Build unitigs](#4-build-unitigs)
    - [Build a unitig matrix](#5-build-a-unitig-matrix)

## Installation

Requirements:
 - a recent version of GCC (or clang) that supports the C++17 standard
 - cmake >= 3.15

To clone the repository:
```
git clone https://github.com/rvicedomini/kmat_tools.git
```

To build the tool:
```
mkdir build && cd build
cmake ..
make
```


## Usage

```
kmtools v0.2

DESCRIPTION
  kmtools - a collection of tools to process text-based k-mer matrices

USAGE
  kmtools <command> <arguments>

COMMANDS
  diff    - difference between two sorted k-mer matrices
  fasta   - output a k-mer matrix in FASTA format
  filter  - filter a k-mer matrix by selecting k-mers that are potentially differential
  merge   - merge two input sorted k-mer matrices
  reverse - reverse complement k-mers in a matrix
  select  - select only a subset of k-mers
  unitig  - build a unitig matrix
  version - print version
```

## Unitig matrix construction pipeline

### Required tools

* kmtricks to build k-mer matrices
* GGCAT (or BCALM2) to build unitigs

### 1. Build a k-mer matrix

Run `kmtricks pipeline` with an appropriate value of `--kmer-size` and `--hard-min`. Use parameter `--mode kmer:count:bin` to build a count matrix, `kmer:pa:bin` for a presence-absence matrix. 

Consider using the `--cpr` parameter to reduce space usage during computation.

### 2. Output a sorted text matrix

Merging and sorting kmtricks partitions can be done simply with the command `kmtricks aggregate` with parameters `--matrix kmer --format text --cpr-in --sorted`. By default, the output is printed on stdout, so you might want to use the `--output` parameter to output the sorted text matrix on a given file.

### 3. Filter the k-mer matrix

* _(optional)_ Remove k-mers belonging to a given collection of reference sequences
    + Run steps 1. and 2. with such a collection as input
    + Run `kmtools diff -k [kmer-size] -z [matrix_samples] [matrix_references]` (__Note__: the `-z` parameter is mandatory if the input matrix have been generated with `kmtricks`!)
* Retain only potentially differential k-mers with `kmtools filter`

### 4. Build unitigs

* Output the filtered matrix in FASTA format with `kmtools fasta`
* Build unitigs (_e.g._, with GGCAT)
* Filter out short unitigs (e.g., shorter than 100 bp)

### 5. Build a unitig matrix

Run the command `kmtools unitig`, providing the unitig file generated in step #4 and the filtered matrix obtained from step #3.

__Note__: at present it is not possible to output an abundance version of a unitig matrix (i.e., with average/median k-mer abundance); it should be pretty straightforward to extend the functionality of the `kmtools unitig` command.

## Acknowledgments

kmtools is based on the two following libraries (included in the `external` directory along with their license):

- [kseq++](https://github.com/cartoonist/kseqpp) : parsing of FASTA file
- [SSHash](https://github.com/jermp/sshash) : Sparse and Skew Hashing of K-Mers
