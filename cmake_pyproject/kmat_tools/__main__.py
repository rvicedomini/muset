#!/usr/bin/env python3
import os
import logging
import sys
import subprocess
import shutil
import argparse
import km_reset
from kmat_tools.utils.utils import eprint,print_error, print_warning, print_status
from kmat_tools.__version__ import __version__
# Default parameters
DEFAULT_KSIZE = 31
DEFAULT_THREADS = 4
DEFAULT_UTG_MIN_SIZE = 100
DEFAULT_MIN_COUNT = 1
DEFAULT_KMAT_ABUNDANCE = 1
DEFAULT_KMAT_MIN_ZEROS = 0
DEFAULT_KMAT_MIN_NONZEROS = 0
DEFAULT_MIN_REC = 3
DEFAULT_OUTDIR = "output"

kmat_tools_root = os.path.dirname(os.path.abspath(os.path.realpath(os.__file__)))


def print_version():
    kmat_tools_version = f'{__version__}'
    git = subprocess.run(['git', '-C', kmat_tools_root, 'describe', '--always'], capture_output=True, stderr=None, text=True)
    if git.returncode == 0:
        git_hash = git.stdout.strip().rsplit('-', 1)[-1]
        kmat_tools_version += f'-{git_hash}'
    return kmat_tools_version

import subprocess
import os

def replace_unitig_identifier(output_dir):
    # Command to sort unitigs.mat file
    sort_command = f"sort -n -o {output_dir}/unitigs.mat {output_dir}/unitigs.mat"

    # Command to extract unitig sequences
    awk_commands = f"""
    awk '{{print $1}}' {output_dir}/unitigs.mat | while read line; do
        awk -v term=">$line" '$1 == term {{getline; print; exit}}' {output_dir}/unitigs.fa
    done > {output_dir}/unitigs_tmp.fa
    """

    # Command to paste and cut
    cut_paste_command = f"""
    cut -f 2- -d ' ' {output_dir}/unitigs.mat |
    paste {output_dir}/unitigs_tmp.fa - -d " " > {output_dir}/unitigs_tmp.mat
    """

    # Execute commands
    subprocess.run(sort_command, shell=True, check=True)
    subprocess.run(awk_commands, shell=True, check=True)
    subprocess.run(cut_paste_command, shell=True, check=True)

    # Clean up
    subprocess.run(f"rm {output_dir}/unitigs.mat {output_dir}/unitigs_tmp.fa", shell=True, check=True)
    subprocess.run(f"mv {output_dir}/unitigs_tmp.mat {output_dir}/unitigs.mat", shell=True, check=True)
    

def main():
    parser = argparse.ArgumentParser(description='Create unitig matrix from fof.txt file and set of FASTA/FASTQ files using kmat_tools')
    parser.add_argument('input_file', metavar='input_seqfile', type=str, help='input seqfile (fof)')
    parser.add_argument('-k', '--kmer-size', type=int, default=DEFAULT_KSIZE, help='kmer size for ggcat and kmtricks (default: 31)')
    parser.add_argument('-t', '--threads', type=int, default=DEFAULT_THREADS, help='number of cores (default: 4)')
    parser.add_argument('-u', '--min-utg-size', type=int, default=DEFAULT_UTG_MIN_SIZE, help='minimum size of the unitigs to be retained in the final matrix (default: 100)')
    parser.add_argument('-c', '--min-count', type=int, default=DEFAULT_MIN_COUNT, help='minimum count of kmers to be retained (default: 1)')
    parser.add_argument('-o', '--outdir', type=str, default=DEFAULT_OUTDIR, help='output directory (default: output)')
    parser.add_argument('-r', '--min-rec', type=int, default=DEFAULT_MIN_REC, help='minimum recurrence to keep a k-mer (default: 3)')
    parser.add_argument('-a', '--min-abundance', type=int, default=DEFAULT_KMAT_ABUNDANCE, help='minimum abundance for kmtool (default: 1)')
    parser.add_argument('-n', '--min-zeros', type=int, default=DEFAULT_KMAT_MIN_ZEROS, help='minimum number of zero columns for each kmers (for kmtools) (default: 0)')
    parser.add_argument('-N', '--min-nonzeros', type=int, default=DEFAULT_KMAT_MIN_NONZEROS, help='minimum number of non-zero columns for each kmers (for kmtools) (default: 0)')
    parser.add_argument('-V', '--version', action='version', version=f'kmat_tools {__version__}',help='Show version number and exit')
    
    args = parser.parse_args()

    if os.path.exists("kmat_tools.log"):
        os.remove("kmat_tools.log")

    logging.basicConfig(filename='kmat_tools.log', level=logging.INFO,
                        format='%(asctime)s - %(levelname)s - %(message)s')
    #sys.stdout = sys.stderr = open('kmat_tools.log', 'a')

    print_status("Starting kmat_tools version: " + __version__)


    # Parameters
    print_status("kmer size:" + str(args.kmer_size))
    print_status("Number of threads:" + str(args.threads))
    print_status("Minimum unitig size:"+ str(args.min_utg_size))
    print_status("Minimum kmer count:"+ str(args.min_count))
    print_status("Output directory:"+ str(args.outdir))
    print_status("Minimum recurrence:"+ str(args.min_rec))
    print_status("Additional options:")
    print_status("Minimum abundance:"+ str(args.min_abundance))
    print_status("Minimum number of zero columns:"+ str(args.min_zeros))
    print_status("Minimum number of non-zero columns:"+ str(args.min_nonzeros))
    print_status("Input file:"+ args.input_file)


    # Building output directory if not present already
    if os.path.isdir(str(args.outdir)):
        print_warning("Folder "+ args.outdir + " is going to be overwritten.")
        shutil.rmtree(str(args.outdir))

    # Step 1: Produce sorted input k-mer matrix from fof.txt file
    print_status("[PIPELINE]::[running kmtricks]")

    try:
        pipeline = subprocess.Popen(["kmtricks", "pipeline", "--file", args.input_file, "--kmer-size", str(args.kmer_size), "--hard-min",
                    str(args.min_count), "--mode", "kmer:count:bin", "--cpr", "--run-dir", args.outdir, "-t", str(args.threads),
                    "--recurrence-min", str(args.min_rec)],stdout=subprocess.PIPE, text=True,stderr=subprocess.STDOUT)
        for line in pipeline.stdout:
            if line != "\n":
                line = line.rstrip('\n')  # Strip the final newline character
                print_status(line)
        
        pipeline.wait()

    except Exception as e:
        print_error(e)
        return 1

    try:
        aggregate = subprocess.Popen(["kmtricks", "aggregate", "--matrix", "kmer", "--format", "text", "--cpr-in", "--sorted", "--output",
                    os.path.join(args.outdir, "sorted_matrix.txt"), "--run-dir", args.outdir, "-t", str(args.threads)],\
                        stdout=subprocess.PIPE, text=True,stderr=subprocess.STDOUT)
        for line in aggregate.stdout:
            if line != "\n":
                line = line.rstrip('\n')  # Strip the final newline character
                print_status(line)
        aggregate.wait()

    except Exception as e:
        print_error(e)
        return 1
    
    # Step 2: Perform some basic filtering with km_basic_filter
    print_status("[PIPELINE]::[filtering kmers]")
    km_reset.reset_getopt()
    try:
    # Run km_basic_filter.py_main_basic_filter as a subprocess
        command_km_basic_filter = ["km_basic_filter", "-a",
                str(args.min_abundance), "-n", str(args.min_zeros), "-N", str(args.min_nonzeros), "-o",
                os.path.join(args.outdir, "sorted_filtered_matrix.txt"),
                os.path.join(args.outdir, "sorted_matrix.txt")]
        
        # Open the log file for appending
        with open('kmat_tools.log', 'a') as log_file:
            process = subprocess.Popen(command_km_basic_filter, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

            # Capture the output and write it to the log file
            for line in process.stdout:
                line = line.rstrip('\n')  # Strip the final newline character
                print_status(line)

            # Wait for the subprocess to finish
            process.wait()
        if process.returncode != 0:
            print_error("Error in km_basic_filter.py_main_basic_filter")
            return(1)
    except Exception as e:
        print_error(e)
        print_error("Error in km_basic_filter.py_main_basic_filter")
        return(1)

    # Step 3: Build unitigs
    # Step 3.1: Convert sorted filtered matrix into a fasta file
    print_status("[PIPELINE]::[exporting filtered kmers]")
    #Reset getopts global variable
    km_reset.reset_getopt()
    try:
        command_km_fasta=["km_fasta", "-o", os.path.join(args.outdir, "kmer_matrix.fasta"),
                                os.path.join(args.outdir, "sorted_filtered_matrix.txt")]
        process = subprocess.Popen(command_km_fasta,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    
        # Capture the output and write it to the log file
        for line in process.stdout:
            line = line.rstrip('\n')  # Strip the final newline character
            print_status(line)

        # Wait for the subprocess to finish
        process.wait()

        if process.returncode != 0:
            print_error("Error in km_fasta.py_main_fasta")
            return 1
    except Exception as e:
        print_error(e)
        print_error("Error in km_fasta.py_main_fasta")
        return 1
    
    # Step 3.2: Use ggcat to produce unitigs
    print_status("[PIPELINE]::[generating unitigs]")
    try:
        command_ggcat=["ggcat", "build", os.path.join(args.outdir, "kmer_matrix.fasta"), "-o",
                    os.path.join(args.outdir, "unitigs.fa"), "-j", str(args.threads), "-s", str(args.min_count), "-k",
                    str(args.kmer_size)]
        process=subprocess.Popen(command_ggcat,stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

            # Capture the output and write it to the log file
        for line in process.stdout:
            line = line.rstrip('\n')  # Strip the final newline character
            print_status(line)

        # Wait for the subprocess to finish
        process.wait()
    
    except Exception as e:
        print_error(e)
        print_error("Error in ggcat")
        return(1)

    # Step 4: Build unitig matrix
    print_status("[PIPELINE]::[building unitigs matrix]")
    km_reset.reset_getopt()
    command_km_unitig=["km_unitig","-l", str(args.min_utg_size), "-k", str(args.kmer_size), "-o",
                   os.path.join(args.outdir, "unitigs.mat"),os.path.join(args.outdir, "unitigs.fa"),
                   os.path.join(args.outdir, "sorted_filtered_matrix.txt")]
    try:
        # Open the log file for appending
        with open('kmat_tools.log', 'a') as log_file:
            process = subprocess.Popen(command_km_unitig, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

            # Capture the output and write it to the log file
            for line in process.stdout:
                line = line.rstrip('\n')  # Strip the final newline character
                print_status(line)

            # Wait for the subprocess to finish
            process.wait()
        if process.returncode != 0:
            print_error("Error in km_unitig")
            return(1)
    except Exception as e:
        print_error(e)
        print_error("Error in km_unitig")
        return(1) 

    # Step 5: Replace unitig identifier in unitigs.mat by unitig sequence
    print_status("[PIPELINE]::[replacing unitig identifier]")
    replace_unitig_identifier(args.outdir)  

    print_status("kmat_tools finished successfully")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        eprint(e)
        print_error(e)  

