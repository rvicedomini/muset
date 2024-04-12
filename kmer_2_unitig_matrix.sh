#!/bin/bash

# Script Name: kmer_2_unitig_matrix
# Author: Camila Duitama, Francesco Andreace, Riccardo Vicedomini
# Date: 2nd of April 2024
# Description: bash script to create unitig matrix from fof.txt file and set of FASTA/FASTQ files using kmat_tools


SCRIPT_NAME=$(basename $0)

#SETTING DEFAULT PARAMETERS
DEFAULT_KSIZE=31
DEFAULT_WSIZE=14
DEFAULT_THREADS=4
DEFAULT_UTG_MIN_SIZE=100
DEFAULT_MIN_COUNT=1
DEFAULT_KMAT_ABUNDANCE=1
DEFAULT_KMAT_MIN_ZEROS=0
DEFAULT_KMAT_MIN_NONZEROS=0
DEFAULT_UTG_MIN=100
DEFAULT_MIN_REC=3
DEFAULT_OUTDIR=output

USAGE=$'\nUsage: '"${SCRIPT_NAME}"' [-k KMER-SIZE] [-t NUM-THREADS] [-u MIN-UTG-SIZE] [-c MIN-COUNT] [-o OUT-DIR] [-r MIN-REC]<input_seqfile> 

Arguments:
     -h              print this help and exit
     -k              kmer size for ggcat and kmtricks (default: 31)
     -t              number of cores (default: 4)
     -u              minimum size of the unitigs to be retained in the final matrix (default: 100)
     -c              minimum count of kmers to be retained (default: 1)
     -a              minimum abundance for kmtool (default: 1)
     -n              minimum number of zero columns for each kmers (for kmtools) (default: 0)
     -N              minimum number of non-zero columns  for each kmers (for kmtools) (default: 0)
     -r              minimum recurrence to keep a k-mer (default: 3).
     -o              output directory (default:output)

-
Positional arguments:
     <input_file>         input seqfile (fof)
'


#PARSING INPUT OPTIONS
k_len=$DEFAULT_KSIZE
thr=$DEFAULT_THREADS
utg_len=$DEFAULT_UTG_MIN_SIZE
min_count=$DEFAULT_MIN_COUNT
output_dir=$DEFAULT_OUTDIR
kmt_abundance=$DEFAULT_KMAT_ABUNDANCE
kmt_min_zeros=$DEFAULT_KMAT_MIN_ZEROS
kmt_min_nonzeros=$DEFAULT_KMAT_MIN_NONZEROS
rec_min=$DEFAULT_MIN_REC

while getopts ":hk:t:u:c:a:n:N:r:o:" opt; do
    case "${opt}" in
	h) $(>&2 echo "${USAGE}")
	   exit 0
	   ;;
	k) k_len=${OPTARG}
	   ;;
   t) thr=${OPTARG}
	   ;;
   u) utg_len=${OPTARG}
	   ;;
	c) maxlen=${OPTARG}
	   ;;
   o) output_dir=${OPTARG}
      ;;
   n) kmt_min_zeros=${OPTARG}
      ;;
   N) kmt_min_nonzeros=${OPTARG}
      ;;
   a) kmt_abundance=${OPTARG}
      ;;
   r) rec_min=${OPTARG}
      ;;

    esac
done

#ADDING INPUT FILE AND OUTPUT FOLDER

input_file=""

#Check user inputs right number of arguments
if [[ $# -lt $((${OPTIND})) ]]
then
    (>&2 echo "ERROR: Wrong number of arguments.")
    (>&2 echo "")
    (>&2 echo "${USAGE}")
    exit 1
fi

input_file=${@:$OPTIND:1}

#Check input file exists
if [ ! -f "$input_file" ]; then
    echo "Error: Input file '$input_file' not found." >&2
    echo "${USAGE}">&2
    exit 1
fi

# Parameters
echo "kmer size: ${k_len}"
echo "Number of threads: ${thr}"
echo "Minimum unitig size: ${utg_len}"
echo "Minimum kmer count: ${min_count}"
echo "Output directory: ${output_dir}"
echo "Minimum recurrence: ${rec_min}"
echo "Additional options:"
echo "Minimum abundance: ${kmt_abundance}"
echo "Minimum number of zero columns: ${kmt_min_zeros}"
echo "Minimum number of non-zero columns: ${kmt_min_nonzeros}"
echo "Input file: $input_file"

start=`date +%s%3N`
#building output directory if not present already
if [ -d $output_dir ]; then rm -rf $output_dir; fi

#Step 1: Produce sorted input k-mer matrix from fof.txt file
runtime=$((`date +%s%3N`-$start)) && echo "[PIPELINE]::[running kmtricks]::[$runtime ms]"

kmtricks pipeline --file $input_file --kmer-size $k_len --hard-min $min_count --mode kmer:count:bin --cpr --run-dir $output_dir -t $thr --recurrence-min $rec_min

kmtricks aggregate --matrix kmer --format text --cpr-in --sorted --output $output_dir/sorted_matrix.txt --run-dir $output_dir -t $thr

# Step 2: Perform some basic filtering with km_basic_filter
runtime=$((`date +%s%3N`-$start)) && echo "[PIPELINE]::[filtering kmers]::[$runtime ms]"

kmtools filter $output_dir/sorted_matrix.txt -a $kmt_abundance -n $kmt_min_zeros -N $kmt_min_nonzeros -o $output_dir/sorted_filtered_matrix.txt

#Step 3: Build unitigs
# Step 3.1: Convert sorted filtered matrix into a fasta file
runtime=$((`date +%s%3N`-$start)) && echo "[PIPELINE]::[exporting filtered kmers]::[$runtime ms]"

kmtools fasta $output_dir/sorted_filtered_matrix.txt -o $output_dir/kmer_matrix.fasta

#Step 3.2: Use ggcat to produce unitigs
runtime=$((`date +%s%3N`-$start)) && echo "[PIPELINE]::[generating unitigs]::[$runtime ms]"

ggcat build $output_dir/kmer_matrix.fasta -o $output_dir/unitigs.fa -j $thr -s $DEFAULT_MIN_COUNT -k $k_len 

#Step 4: Build unitig matrix 
runtime=$((`date +%s%3N`-$start))
echo "[PIPELINE]::[building unitigs matrix]::[$runtime ms]"

kmtools unitig -l $utg_len -k $k_len -o $output_dir/unitigs.mat $output_dir/unitigs.fa $output_dir/sorted_filtered_matrix.txt

#Step 5: Replace unitig identifier in unitigs.mat by unitig sequence
sort -n -o $output_dir/unitigs.mat $output_dir/unitigs.mat
(
awk '{print $1}' $output_dir/unitigs.mat | while read line; do
    awk -v term=">$line" '$1 == term {getline; print; exit}' $output_dir/unitigs.fa
done
) > $output_dir/unitigs_tmp.fa

cut -f 2- -d ' ' $output_dir/unitigs.mat | paste $output_dir/unitigs_tmp.fa - -d " " > $output_dir/unitigs_tmp.mat
rm $output_dir/unitigs.mat $output_dir/unitigs_tmp.fa && mv $output_dir/unitigs_tmp.mat $output_dir/unitigs.mat

runtime=$((`date +%s%3N`-$start)) && echo "[PIPELINE]::[END]::[$runtime ms]"
exit 0