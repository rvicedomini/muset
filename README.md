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
kmtools v0.1

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

## Commands helpful for building a unitig matrix

### `kmtools filter`
Filters a matrix by selecting k-mers that are potentially differential (present in a minimum number of samples/columns and absent in a minimum number of samples/columns)

### `kmtools diff`
Given two input sorted matrices _M1_ and _M2_, removes from _M1_ the k-mers belonging to _M2_.
In other words outputs the matrix obtained from the difference between _M1_ and _M2_.

### `kmtools fasta`
Convert a k-mer matrix in FASTA format

### `kmtools unitig`
Given a unitig file in FASTA format and a k-mer matrix, it outputs a presence-absence unitig matrix:
- each row correspont to a unitig.
- the first column is the unitig ID.
- the _i_-th column is set to `1` only if __ALL__ unitig's k-mers are above a given threshold (`-c` parameter) in the _i_-th column of the input matrix.

__Warning__: this program loads in memory each (canonical) k-mer of the input unitigs; it might be advisable to only consider a relatively small set of unitigs.

__Note__: in principle this program could be easily generalized to output an abundance unitig matrix (e.g., taking the mean/median abundance of k-mers)


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

## External libraries

kmtools is based on the two following libraries (included in the `external` directory):

- [kseq++](https://github.com/cartoonist/kseqpp) : parsing of FASTA file
- [sshash](https://github.com/jermp/sshash) : efficient k-mer dictionary construction
