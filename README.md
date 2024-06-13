# muset

A pipeline for building an abundance unitig matrix from a list of FASTA/FASTQ files.

+ [Installation](#installation)
  - [Conda installation](#conda-installation)
  - [Build from source](#build-from-source)
  - [Build a Singularity image](#build-a-singularity-image)
+ [Usage](#usage)
  + [I do not have a k-mer matrix](#i-do-not-have-a-k-mer-matrix)
  + [I already have a k-mer matrix](#i-already-have-a-k-mer-matrix)
  + [k-mer matrix operations](#k-mer-matrix-operations)
+ [Acknowledgements](#acknowledgements)

## Installation

### Conda installation

Requirements:
  - [GGCAT](https://github.com/algbio/ggcat?tab=readme-ov-file#installation)

Then you can install `muset` by creating conda environment:
```
git clone https://github.com/camiladuitama/muset.git
cd muset
conda env create -n muset --file environment.yaml
```

To run `muset` remember to activate the conda environment with:
```
conda activate muset
```

You can check if `muset` is correctly installed as follows:
```
cd test
muset fof.txt
```

### Build from source

Requirements:
  - a recent version of GCC (or clang) that supports the C++17 standard
  - cmake >= 3.15
  - [GGCAT](https://github.com/algbio/ggcat?tab=readme-ov-file#installation)
  - [kmtricks](https://github.com/tlemane/kmtricks/wiki/Installation) >= 1.4.0

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
Executables will be made available in the `bin` sub-directory relative to the root folder of the repository.

Include the absolute path of MUSET's executables in your PATH environment variable by adding the following line to your `~/.bashrc` file:
```
export PATH=/absolute/path/to/muset/bin:${PATH}
```

You can check if `muset` is correctly installed as follows:
```
cd test
muset fof.txt
```

### Build a Singularity image

Requirements:
  - Singularity installed on your system. Refer to the [Singularity Installation Guide](https://sylabs.io/guides/3.0/user-guide/installation.html) for detailed instructions.


To build a singularity image (e.g., `muset.sif`):
```
git clone https://github.com/CamilaDuitama/muset.git
cd muset/singularity
sudo singularity build muset.sif Singularity.def
```

To run `muset` and see the help message, use the following command:
```
singularity exec /path/to/muset.sif muset -h'
```

To try `muset` with example data, `cd` to the `test` directory within the repository, then run:
```
singularity exec /path/to/muset.sif muset fof.txt'
```

## Usage

````
muset v0.2

DESCRIPTION:
   muset - a pipeline for building an abundance unitig matrix from a list of FASTA/FASTQ files.

USAGE:
   muset [options] INPUT_FILE

OPTIONS:
   -i PATH    skip matrix construction and run the pipeline with a previosuly computed matrix
   -k INT     k-mer size (default: 31)
   -a INT     min abundance to keep a k-mer (default: 2)
   -u INT     minimum size of the unitigs to be retained in the final matrix (default: 100)
   -o PATH    output directory (default: output)
   -r INT     minimum recurrence to keep a k-mer (default: 3)
   -m INT     minimizer length  (default: 15)
   -n INT     minimum number of samples from which a k-mer should be absent (mutually exclusive with -f)
   -f FLOAT   fraction of samples from which a k-mer should be absent (default: 0.1, mutually exclusive with -n)
   -N INT     minimum number of samples in which a k-mer should be present (mutually exclusive with -F)
   -F FLOAT   fraction of samples in which a k-mer should be present (default: 0.1, mutually exclusive with -N)
   -t INT     number of cores (default: 4)
   -h         print this help and exit

POSITIONAL ARGUMENTS:
    INPUT_FILE   Input file (fof) containing the description of input samples.
                 It is ignored if -i option is used.

NOTES:
   Options -n and -f are mutually exclusive, as well as options -N and -F.
   When either -n or -f is used, -N or -F must also be provided, and vice versa.
   If none of the -n, -N, -f, -F options are used the last two options are used with their default values.
````

### I do not have a k-mer matrix

If you do not have a k-mer matrix ready, make sure to create a "fof" file, that is a file which contains one line per sample with the following syntax:
  - `<Sample ID> : <1.fastq.gz> ; ... ; <N.fastq.gz>`

Files could be in either FASTA or FASTQ format, gzipped or not.
Multiple files per sample can be provided by separating them with a semicolon.

<ins>Example:</ins>
```
A1 : /path/to/fastq_A1_1
B1 : /path/to/fastq_B1_1 ; /with/mutiple/fasta_B1_2
```

You can generate such an input file from a folder containing many input files as follows:
```
ls -1 folder/*  | sort -n -t/ -k2 | xargs -L1 readlink -f | awk '{ print ++count" : "$1 }' >fof.txt
```

Then simply run:
```
muset fof.txt
```

### I already have a k-mer matrix
If you are familiar with `kmtricks` and/or have already produced a k-mer matrix on your own, you can run `muset` with the `-i` option and provide your own input matrix (and skip the possibly long matrix construction).

Make sure to provide a matrix in text format. You can easily output one from a kmtricks run using the command `kmtricks aggregate` with parameters `--matrix kmer --format text`.
By default, `kmtricks` will write it on stdout, so you might want to set the `--output` parameter.
Ex: `kmtricks aggregate --matrix kmer --format text --cpr-in --sorted --output sorted_matrix.txt --run-dir kmtricks_output_dir`

The pipeline can be then run as follows:
```
muset -i sorted_matrix.txt <input_fof.txt>
```


### K-mer matrix operations

MUSET includes a `kmat_tools`, an auxiliary executable allowing to perform some basic operations on a (text) k-mer matrix.

```
kmat_tools v0.2

DESCRIPTION
  kmat_tools - a collection of tools to process text-based k-mer matrices

USAGE
  kmat_tools <command> <arguments>

COMMANDS
  diff     - difference between two sorted k-mer matrices
  fasta    - output a k-mer matrix in FASTA format
  fafmt    - filter a FASTA file by length and write sequences in single lines
  filter   - filter a k-mer matrix by selecting k-mers that are potentially differential
  merge    - merge two input sorted k-mer matrices
  reverse  - reverse complement k-mers in a matrix
  select   - select only a subset of k-mers
  unitig   - build a unitig matrix
  version  - print version
```

## Acknowledgements

MUSET is based on the following libraries (included in the `external` directory along with their license):

- [kseq++](https://github.com/cartoonist/kseqpp): parsing of FASTA file
- [PTHash](https://github.com/jermp/pthash): compact minimal perfect hash
- [SSHash](https://github.com/jermp/sshash): Sparse and Skew Hashing of K-Mers