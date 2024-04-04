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
DEFAULT_OUTDIR=output

USAGE=$'\nUsage: '"${SCRIPT_NAME}"' [-k KMER-SIZE] [-w WINDOW-SIZE] [-t NUM-THREADS] [-u MIN-UTG-SIZE] [-c MIN-COUNT] [-o OUT-DIR] <input_seqfile> <name>

Arguments:
     -h              print this help and exit
     -k              kmer size for ggcat and kmtricks (default:31)
     -w              window size for ggcat (default:14)
     -t              number of cores (default:4)
     -u              minimum size of the unitigs to be retained in the final matrix (default:100)
     -c              minimum count of kmers to be retained (default:1)
     -o              output directory (default:output)


Positional arguments:
     <input_file>         input seqfile (fof)
     <name>               name for output files
'


#PARSING INPUT OPTIONS
k_len=$DEFAULT_KSIZE
w_len=$DEFAULT_WSIZE
thr=$DEFAULT_THREADS
utg_len=$DEFAULT_UTG_MIN_SIZE
min_count=$DEFAULT_MIN_COUNT
output_dir=$DEFAULT_OUTDIR

while getopts "s:k:w:t:c:m:o:lhp" flag; do
    case "${flag}" in
	h) $(>&2 echo "${USAGE}")
	   exit 0
	   ;;
	k) k_len=${OPTARG}
	   ;;
	w) w_len=${OPTARG}
	   ;;
    t) thr=${OPTARG}
	   ;;
    u) utg_len=${OPTARG}
	   ;;
	c) maxlen=${OPTARG}
	   ;;
    o) output_dir=${OPTARG}
       ;;
    esac
done

#ADDING INPUT FILE AND OUTPUT FOLDER

input_file=""
name=""

if [[ $# -lt $((${OPTIND} + 1)) ]]
then
    (>&2 echo "ERROR: Wrong number of arguments.")
    (>&2 echo "")
    (>&2 echo "${USAGE}")
    exit 1
fi

input_file=${@:$OPTIND:1}
name=${@:$OPTIND+1:1}

#Step 1: Produce sorted input k-mer matrix from fof.txt file
kmtricks pipeline --file $input_file --kmer-size $k_len --hard-min $min_count --mode kmer:count:bin --cpr --run-dir $output_dir

kmtricks aggregate --matrix kmer --format text --cpr-in --sorted --output $output_dir/sorted_matrix.txt --run-dir $output_dir 

# Step 2: Perform some basic filtering with km_basic_filter
./bin/km_basic_filter $output_dir/sorted_matrix.txt -a 1 -n 0 -N 0 -o $output_dir/sorted_filtered_matrix.txt

#Step 3: Build unitigs
# Step 3.1: Convert sorted filtered matrix into a fasta file
./bin/km_fasta $output_dir/sorted_filtered_matrix.txt -o $output_dir/kmer_matrix.fasta

#Step 3.2: Use ggcat to produce unitigs
ggcat build $output_dir/kmer_matrix.fasta -o $output_dir/unitigs -j $thr -k 30 

#Step 3.3 & 3.4: Filter unitigs that are <100 nt and rename filtered unitigs to be labelled sequentially
awk 'BEGIN {count=0} /^>/ {next} {if(length($0) > 100) print ">" count++ "\n" $0;}' $output_dir/unitigs > $output_dir/unitigs_filtered_sequential
#seqkit seq -m $utg_len $output_dir/unitigs > $output_dir/unitigs_filtered
#awk 'BEGIN{i=-1} /^>/{print ">" ++i; next} 1' $output_dir/unitigs_filtered > $output_dir/unitigs_filtered_sequential

#Step 4: Build unitig matrix
python3 scripts/km_unitig_pa.py -f $output_dir/unitigs_filtered_sequential -c $min_count -o $output_dir/unitig_matrix -m $output_dir/sorted_filtered_matrix.txt
