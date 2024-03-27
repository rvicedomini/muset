# kmat_tools

A collection of simple C programs and Python scripts to process k-mer matrices (e.g., built using [kmtricks](https://github.com/tlemane/kmtricks)) in text format.

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

## Usage

To clone the repository, run the following command:
```
git clone https://github.com/rvicedomini/kmat_tools.git
```

To build the tools:
```
cd kmat_tools
make
```

## k-mer matrix operations

### `km_basic_filter`
Filters a matrix by selecting k-mers that are potentially differential (present in a minimum number of samples/columns and absent in a minimum number of samples/columns)

### `km_diff`
Given two input sorted matrices _M1_ and _M2_, removes from _M1_ the k-mers belonging to _M2_.
In other words outputs the matrix obtained from the difference between _M1_ and _M2_.

### `km_fasta`
Convert a k-mer matrix in FASTA format


## Unitig matrix operations

This operations have been implemented (in a possibly inefficient way) in the python scripts inside the `scripts` directory.

### `km_unitig_pa.py`
Given a unitig file in FASTA format and a k-mer matrix, it outputs a presence-absence unitig matrix:
- each row correspont to a unitig.
- the first column is the unitig ID.
- the _i_-th column is set to `1` only if __ALL__ unitig's k-mers are above a given threshold (`-c` parameter) in the _i_-th column of the input matrix.

__Warning__: the script loads in memory each k-mer (and its reverse complement) of the input unitigs; it might be advisable to only consider a relatively small set of unitigs.

__Note__: this script could be easily generalized to output a abundance unitig matrix (e.g., taking the mean/median abundance of k-mers)


## Unitig matrix construction pipeline

### Required tools

* kmtricks to build k-mer matrices
* BCALM2 or GGCAT to build unitigs
* Python 3

### 1. Build a k-mer matrix

Run `kmtricks pipeline` with an appropriate value of `--kmer-size` and `--hard-min`. Use parameter `--mode kmer:count:bin` to build a count matrix, `kmer:pa:bin` for a presence-absence matrix. 

Consider using the `--cpr` parameter to reduce space usage during computation.

### 2. Output a sorted text matrix

Merging and sorting kmtricks partitions can be done simply with the command `kmtricks aggregate` with parameters `--matrix kmer --format text --cpr-in --sorted`. By default, the output is printed on stdout, so you might want to use the `--output` parameter to output the sorted text matrix on a given file.

### 3. Filter the k-mer matrix

* Remove k-mers belonging to a given collection of reference sequences
    + Run steps 1. and 2. with such a collection as input
    + Run `km_diff -k [kmer-size] -z [matrix_samples] [matrix_references]` (__Note__: the use of the `-z` parameter is mandatory if the input matrix have been generated with `kmtricks`!)
* Retain only potentially differential k-mers with `km_basic_filter`

### 4. Build unitigs

* Output the filtered matrix in FASTA format with `km_fasta`
* Build unitigs (with BCALM2 or, maybe better, GGCAT)
* (Rename unitigs with integer numbers?)
* Filter out short unitigs (e.g., shorter than 100 bp)

### 5. Build a unitig matrix

Run the python script `km_unitig_pa.py`, providing the unitig file generated in step #4 and the filtered matrix obtained from step #3.
Remember to set `-c` to `1` if the input is a presence-absence matrix.

__Note__: at present there is no script to output an abundance version of a unitig matrix (i.e., with average/median k-mer abundance); it should be pretty straightforward to write one from `km_unitig_pa.py`.
