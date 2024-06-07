# muset

A collection of C++ utilities to construct a unitig matrix from sequencing data.

+ [Installation](#installation)
  - [via CMake](#via-cmake)
  - [via Conda](#via-conda)
  - [via Singularity](#via-singularity)
+ [Usage](#usage)
  + [k-mer matrix operations](#k-mer-matrix-operations)
  + [Use case: construction of a unitig matrix with abundances](#use-case-construction-of-a-unitig-matrix-with-abundances)
    - [I don't have a k-mer matrix ready](#i-dont-have-a-k-mer-matrix-ready)
    - [I already have my k-mer matrix](#i-already-have-my-k-mer-matrix)
+ [Acknowledgements](#acknowledgements)

## Installation
### via CMake

Requirements:
 - a recent version of GCC (or clang) that supports the C++17 standard
 - cmake >= 3.15
 - ggcat(https://github.com/algbio/ggcat?tab=readme-ov-file#installation).

To clone the repository:
```
git clone https://github.com/camiladuitama/muset.git
```

To build the tool:
```
cd muset
mkdir build && cd build
cmake ..
make
```
To test the tool make sure the binary is on your PATH and try running from the main muset folder:

````
muset pipeline test/fof.txt
````


### Via Conda

Requirements:

First install [ggcat](https://github.com/algbio/ggcat?tab=readme-ov-file#installation).

Then you can install `muset` via conda:
```
git clone https://github.com/camiladuitama/muset.git
cd muset
conda env create -n muset --file environment.yaml
conda deactivate
conda activate muset

```
You can test it is working by running the pipeline with default parameters to create unitig matrices:

```
muset pipeline test/fof.txt
```

### via Singularity

Requirements:

- Singularity installed on your system. Refer to the [Singularity Installation Guide](https://sylabs.io/guides/3.0/user-guide/installation.html) for detailed instructions.

Build singularity image from repo:
   
```
git clone https://github.com/CamilaDuitama/muset.git
cd muset/singularity
sudo singularity build ../muset.sif Singularity.def
```

To run `muset` and see a help message, use the following command:

```
singularity exec ../muset.sif bash -c '. /opt/miniconda/etc/profile.d/conda.sh && conda activate muset && muset -h'
```

Example running `muset`pipeline:

```
singularity exec ../muset.sif bash -c '. /opt/miniconda/etc/profile.d/conda.sh && conda activate muset && muset pipeline test/fof.txt'
```

## Usage

### K-mer matrix operations

```
muset v0.2

DESCRIPTION
  muset - a collection of tools to process k-mer matrices towards the construction of a unitig matrix

USAGE
  muset <command> <arguments>

COMMANDS
  diff     - difference between two sorted k-mer matrices
  fasta    - output a k-mer matrix in FASTA format
  fafmt    - filter a FASTA file by length and write sequences in single lines
  filter   - filter a k-mer matrix by selecting k-mers that are potentially differential
  merge    - merge two input sorted k-mer matrices
  reverse  - reverse complement k-mers in a matrix
  select   - select only a subset of k-mers
  unitig   - build a unitig matrix
  pipeline - run the pipeline to produce an abundance unitig matrix from a set of FASTA/FASTQ files
  version  - print version
```

### Use case: construction of a unitig matrix with abundances
````
Usage : muset pipeline [-s] [-k KMER-SIZE] [-t NUM-THREADS] [-u MIN-UTG-SIZE] [-c MIN-COUNT] [-o OUT-DIR] [-r MIN-REC] [-m MINIMIZER-LENGTH] [-a KMAT-ABUNDANCE] [-n MIN-ZERO-COLUMNS | -f FRAC-SAMPLES-ABSENT] [-N MIN-NONZERO-COLUMNS | -F FRAC-SAMPLES-PRESENT] <input_seqfile>

muset pipeline produces an abundance unitig matrix from a set of FASTA/FASTQ files

Arguments:
     -h              print this help and exit
     -s              skip the matrix construction step (Step 1)
     -k              k-mer size for ggcat and kmtricks (default: 31)
     -t              number of cores (default: 4)
     -u              minimum size of the unitigs to be retained in the final matrix (default: 100)
     -c              minimum count of k-mers to be retained (default: 1)
     -o              output directory (default: output)
     -r              minimum recurrence to keep a k-mer (default: 3)
     -m              length of the minimizer  (default: 15)
     -a              minimum abundance for kmtool (default: 1)
     -n              minimum number of zero columns for each k-mer or min number of samples for which a k-mer should be absent (mutually exclusive with -f)
     -f              fraction of samples in which a k-mer should be absent (default: 0.1, mutually exclusive with -n)
     -N              minimum number of non-zero columns for each k-mer or min number of samples for which a k-mer should be present (mutually exclusive with -F)
     -F              fraction of samples in which a k-mer should be present (default: 0.1, mutually exclusive with -N)

Positional arguments:
     <input_file>         input seqfile (fof)

For options -n and -f, if neither is provided, -f with its default value is used. 
For options -N and -F, if neither is specified, -F with its default value is used.
````
#### I don't have a k-mer matrix ready

If you don't have a k-mer matrix ready make sure you create an fof file which is a file with one sample per line, with an ID and a list of files:

**Input data:**

` <Sample ID> : <1.fastq.gz> ; ... ; <N.fastq.gz> !`

You could produce this input fof if you have a folder containing many input files in the following manner:

```
ls -1 folder/*  | sort -n -t/ -k 2 |awk '{print ++count" : "$1}' > fof.txt
```

Then simply run:

```
muset pipeline fof.txt
```

#### I already have my k-mer matrix
We recommend you run `muset pipeline` with the flag `-s` if you are familiar with `kmtricks`and/or have already produced a k-mer matrix on your own. Since this is a lengthy step, if you want to run the pipeline without doing the matrix creation, you should use this flag. 

If you already have your k-mer matrix, make sure you used `kmtricks` following these guidelines:
  - Run `kmtricks pipeline` with an appropriate value of `--kmer-size` and `--hard-min`. Use parameter `--mode kmer:count:bin` to build a count matrix, `kmer:pa:bin` for a presence-absence matrix. 
  - Consider using the `--cpr` parameter to reduce space usage during computation.
  - Merging and sorting kmtricks partitions can be done simply with the command `kmtricks aggregate` with parameters `--matrix kmer --format text --cpr-in --sorted`. By default, the output is printed on stdout, so you **MUST** set the `--output` parameter to output the sorted text matrix to a file called sorted_matrix.txt. Ex: `kmtricks aggregate --matrix kmer --format text --cpr-in --sorted --output $output_dir/sorted_matrix --run-dir $output_dir`

Then the way to run the pipeline should be:

```
muset pipeline -s <input_fof.txt>
```

## Acknowledgements

muset is based on the following libraries (included in the `external` directory along with their license):

- [kseq++](https://github.com/cartoonist/kseqpp): parsing of FASTA file
- [PTHash](https://github.com/jermp/pthash): compact minimal perfect hash
- [SSHash](https://github.com/jermp/sshash): Sparse and Skew Hashing of K-Mers
