#!/bin/bash

# Script Name: kmer_2_unitig_matrix
# Author: Camila Duitama, Francesco Andreace, Riccardo Vicedomini
# Date: 2nd of April 2024
# Description: bash script to create unitig matrix from fof.txt file and set of FASTA/FASTQ files using kmat_tools

#Step 1: Produce sorted input k-mer matrix from fof.txt file
kmtricks pipeline --file test/fof.txt --kmer-size 31 --hard-min 1 --mode kmer:count:bin --cpr --run-dir output/

kmtricks aggregate --matrix kmer --format text --cpr-in --sorted --output output/sorted_matrix.txt --run-dir output/ 

# Step 2: Perform some basic filtering with km_basic_filter
./bin/km_basic_filter output/sorted_matrix.txt -a 1 -n 0 -N 0 -o output/sorted_filtered_matrix.txt

#Step 3: Build unitigs
# Step 3.1: Convert sorted filtered matrix into a fasta file
./bin/km_fasta output/sorted_filtered_matrix.txt -o output/kmer_matrix.fasta

#Step 3.2: Use ggcat to produce unitigs
ggcat build output/kmer_matrix.fasta -o output/unitigs -j 10 -k 30 

#Step 3.3: Filter unitigs that are <100 nt
seqkit seq -m 100 output/unitigs > output/unitigs_filtered

#Step 3.4: Rename filtered unitigs to be labelled sequentially
awk 'BEGIN{i=-1} /^>/{print ">" ++i; next} 1' output/unitigs_filtered > output/unitigs_filtered_sequential

#Step 4: Build unitig matrix
python3 scripts/km_unitig_pa.py -f output/unitigs_filtered_sequential -c 1 -o output/unitig_matrix -m output/sorted_filtered_matrix.txt

