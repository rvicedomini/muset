#!/bin/bash
set -e

# Script Name: reads_2_unitig_matrix.sh
# Author: Camila Duitama, Francesco Andreace, Riccardo Vicedomini
# Date: 2nd of April 2024
# Description: bash script to create unitig matrix from fof.txt file and set of FASTA/FASTQ files using kmat_tools

SCRIPT_NAME=$(basename $0)
LOG_DIR="logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_${TIMESTAMP}.log"

# Ensure log directory exists
mkdir -p $LOG_DIR

# Redirect stdout and stderr to log file
exec > >(tee -a "$LOG_FILE") 2>&1

# Function to log and run commands
log_and_run() {
    echo "[COMMAND]:: Running: $@" | tee -a "$LOG_FILE"
    "$@"
}

# SETTING DEFAULT PARAMETERS
DEFAULT_KSIZE=31
DEFAULT_WSIZE=14
DEFAULT_THREADS=4
DEFAULT_UTG_MIN_SIZE=100
DEFAULT_MIN_COUNT=1
DEFAULT_KMAT_ABUNDANCE=1
DEFAULT_NUM_SAMPLES_ABSENT=10
DEFAULT_NUM_SAMPLES_PRESENT=10
DEFAULT_FRAC_SAMPLES_ABSENT=0.1
DEFAULT_FRAC_SAMPLES_PRESENT=0.1
DEFAULT_UTG_MIN=100
DEFAULT_MIN_REC=3
DEFAULT_MINIMIZER_LENGTH=15
DEFAULT_OUTDIR=output

USAGE=$'\nUsage: '"${SCRIPT_NAME}"' [-k KMER-SIZE] [-t NUM-THREADS] [-u MIN-UTG-SIZE] [-c MIN-COUNT] [-o OUT-DIR] [-r MIN-REC] [-m MINIMIZER-LENGTH] [-a KMAT-ABUNDANCE] [-n MIN-ZERO-COLUMNS | -f FRAC-SAMPLES-ABSENT] [-N MIN-NONZERO-COLUMNS | -F FRAC-SAMPLES-PRESENT] <input_seqfile>

Arguments:
     -h              print this help and exit
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
'


# Parsing input options
k_len=$DEFAULT_KSIZE
thr=$DEFAULT_THREADS
utg_len=$DEFAULT_UTG_MIN_SIZE
min_count=$DEFAULT_MIN_COUNT
output_dir=$DEFAULT_OUTDIR
kmt_abundance=$DEFAULT_KMAT_ABUNDANCE
num_samples_absent=$DEFAULT_NUM_SAMPLES_ABSENT
num_samples_present=$DEFAULT_NUM_SAMPLES_PRESENT
rec_min=$DEFAULT_MIN_REC
minimizer_length=$DEFAULT_MINIMIZER_LENGTH
frac_samples_absent=$DEFAULT_FRAC_SAMPLES_ABSENT
frac_samples_present=$DEFAULT_FRAC_SAMPLES_PRESENT
param_n=""
param_N=""

# Function to check if a value is an integer
is_integer() {
    local s="$1"
    if ! [[ "$s" =~ ^-?[0-9]+$ ]]; then
        return 1
    fi
    return 0
}


while getopts ":hk:t:u:c:a:n:N:f:F:r:o:m:" opt; do
    case "${opt}" in
    h) echo "${USAGE}" >&2
       exit 0
       ;;
    k) k_len=${OPTARG}
       ;;
    t) thr=${OPTARG}
       ;;
    u) utg_len=${OPTARG}
       ;;
    c) min_count=${OPTARG}
       ;;
    o) output_dir=${OPTARG}
       ;;
    n)
       if [ -n "$param_n" ]; then
           echo "Error: Cannot combine -n with -f" >&2
           echo "${USAGE}" >&2
           exit 1
       fi
       if ! is_integer "${OPTARG}"; then
           echo "Error: -n argument must be an integer." >&2
           exit 1
       fi
       param_n="-n ${OPTARG}"
       ;;
    N)
       if [ -n "$param_N" ]; then
           echo "Error: Cannot combine -N with -F" >&2
           echo "${USAGE}" >&2
           exit 1
       fi
       if ! is_integer "${OPTARG}"; then
           echo "Error: -N argument must be an integer." >&2
           exit 1
       fi
       param_N="-N ${OPTARG}"
       ;;
    f) 
       if [ -n "$param_n" ]; then
           echo "Error: Cannot combine -f with -n" >&2
           echo "${USAGE}" >&2
           exit 1
       fi
       param_n="-f ${OPTARG}"
       ;;
    F) 
       if [ -n "$param_N" ]; then
           echo "Error: Cannot combine -F with -N" >&2
           echo "${USAGE}" >&2
           exit 1
       fi
       param_N="-F ${OPTARG}"
       ;;
    a) kmt_abundance=${OPTARG}
       ;;
    r) rec_min=${OPTARG}
       ;;
    m) minimizer_length=${OPTARG}
       ;;
    \?) echo "Invalid option: -$OPTARG" >&2
        echo "${USAGE}" >&2
        exit 1
        ;;
    :)  echo "Option -$OPTARG requires an argument." >&2
        echo "${USAGE}" >&2
        exit 1
        ;;
    esac
done

# ADDING INPUT FILE AND OUTPUT FOLDER

input_file=""

# Check if user inputs the right number of arguments
if [[ $# -lt $((${OPTIND})) ]]; then
    (>&2 echo "ERROR: Wrong number of arguments.")
    (>&2 echo "")
    (>&2 echo "${USAGE}")
    exit 1
fi

input_file=${@:$OPTIND:1}

# Check input file exists
if [ ! -f "$input_file" ]; then
    echo "Error: Input file '$input_file' not found." >&2
    echo "${USAGE}" >&2
    exit 1
fi

# Validate that minimizer length is smaller than k_len
if [ "$minimizer_length" -ge "$k_len" ]; then
    echo "Error: Minimizer length (-m) value ($minimizer_length) must be smaller than k-mer size (-k) value ($k_len)." >&2
    exit 1
fi

# Strip trailing slash from output_dir if present
if [[ "$output_dir" == */ ]]; then
    output_dir="${output_dir%/}"
fi

# Parameters
echo "k-mer size (-k): ${k_len}"
echo "Number of threads (-t): ${thr}"
echo "Minimum unitig size (-u): ${utg_len}"
echo "Minimum k-mer count (-c): ${min_count}"
echo "Output directory (-o): ${output_dir}"
echo "Minimum recurrence (-r): ${rec_min}"
echo "Minimizer length (-m): ${minimizer_length}"
echo "Minimum abundance (-a): ${kmt_abundance}"
echo "Input file: ${input_file}"

# Check for default settings if -n, -N, -f, -F, are not set
if [ -z "$param_n" ] && [ -z "$param_N" ]; then
    param_n="-f $frac_samples_absent"
    param_N="-F $frac_samples_present"
elif [ -z "$param_n" ]; then
    echo "Error: Either -n or -f must be specified." >&2
    echo "${USAGE}" >&2
    exit 1
elif [ -z "$param_N" ]; then
    echo "Error: Either -N or -F must be specified." >&2
    echo "${USAGE}" >&2
    exit 1
fi

echo "Selected options:"
echo "Basic filter settings: $param_n $param_N"

start=$(date +%s%3N)

# Step 1: Produce sorted input k-mer matrix from fof.txt file
#log_and_run kmtricks pipeline --file $input_file --kmer-size $k_len --hard-min $min_count --mode kmer:count:bin --cpr --run-dir $output_dir -t $thr --recurrence-min $rec_min

#log_and_run kmtricks aggregate --matrix kmer --format text --cpr-in --sorted --output $output_dir/sorted_matrix.txt --run-dir $output_dir -t $thr

# Step 2: Perform some basic filtering with kmtools filter
log_and_run kmtools filter $output_dir/sorted_matrix.txt -a $kmt_abundance $param_n $param_N -o $output_dir/sorted_filtered_matrix.txt

# Check if filter was too stringent
if [ ! -s "$output_dir/sorted_filtered_matrix.txt" ]; then
    echo "Your filters were too stringent. The output file sorted_filtered_matrix.txt is empty." >&2
    exit 1
fi

# Step 3.1: Convert sorted filtered matrix into a fasta file
log_and_run kmtools fasta $output_dir/sorted_filtered_matrix.txt -o $output_dir/kmer_matrix.fasta

# Step 3.2: Use ggcat to produce unitigs
log_and_run ggcat build $output_dir/kmer_matrix.fasta -o $output_dir/unitigs.fa -j $thr -s $DEFAULT_MIN_COUNT -k $k_len

# Step 3.3: Filter out short unitigs
log_and_run kmtools fafmt -l $utg_len -o $output_dir/unitigs_filtered.fa $output_dir/unitigs.fa

# Check if the kmtools filter output file is empty
if [ ! -s "$output_dir/unitigs_filtered.fa" ]; then
    echo "Your filters were too stringent. The output file unitigs.mat is empty." >&2
    exit 1
fi

# Step 4: Build unitig matrix
log_and_run kmtools unitig -k $k_len -m $minimizer_length -o $output_dir/unitigs.mat -t $thr $output_dir/unitigs_filtered.fa $output_dir/sorted_filtered_matrix.txt

# Step 5: Replace unitig identifier in unitigs.mat by unitig sequence
log_and_run sort -n --parallel=$thr -o $output_dir/unitigs.mat $output_dir/unitigs.mat
(
awk '{print $1}' $output_dir/unitigs.mat | while read line; do
    awk -v term=">$line" '$1 == term {getline; print; exit}' $output_dir/unitigs.fa
done
) > $output_dir/unitigs_tmp.fa

cut -f 2- -d ' ' $output_dir/unitigs.mat | paste $output_dir/unitigs_tmp.fa - -d " " > $output_dir/unitigs_tmp.mat
#rm $output_dir/unitigs.mat $output_dir/unitigs_tmp.fa && mv $output_dir/unitigs_tmp.mat $output_dir/unitigs.mat


runtime=$((`date +%s%3N`-$start)) && echo "[PIPELINE]::[END]::[$runtime ms]"
exit 0
