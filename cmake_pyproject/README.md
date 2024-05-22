# kmat tools

## Description
`kmat_tools` is a tool that creates unitig abundance matrices from a list of FASTA/FASTQ files. It requires the installation of zlib, gcc, and gccat.

## Installation
Before using `kmat_tools`, make sure you have the following dependencies installed on your system:
- zlib
- gcc
- gccat

To install `kmat_tools`, follow these steps:

1. Clone the repository:
    ```shell
    git clone https://github.com/your-username/kmat_tools.git
    ```

2. Build the tool:
    ```shell
    cd kmat_tools
    make
    ```

## Usage
To use `kmat_tools`, follow these steps:

1. Prepare a list of FASTA/FASTQ files that you want to process.

2. Run the tool with the following command:
    ```shell
    kmat_tools /path/to/your/list_of_files.txt
    ```

    Replace `/path/to/your/list_of_files.txt` with the actual path to your list of files.

## Example
To test `kmat_tools` with a sample dataset, you can use the provided `fof.test` file. Run the following command: