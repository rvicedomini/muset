#!/bin/bash
set -e

# checking if gzip, prefetch, and fasterq-dump are available
command -v gzip >/dev/null 2>&1 || { echo >&2 "gzip is required but it's not installed. Exiting."; exit 1; }
command -v prefetch >/dev/null 2>&1 || { echo >&2 "prefetch is required but it's not installed. Exiting."; exit 1; }
command -v fasterq-dump >/dev/null 2>&1 || { echo >&2 "fasterq-dump is required but it's not installed. Exiting."; exit 1; }

# usage function
usage(){
  echo "Script to download SRA for MUSET's experiments"
  echo "Usage: \$0 [-i <inputfile>] [-o <outputdir>] [-t <threads>]"
  echo "   -i     Specify input file of accessions"
  echo "   -o     Specify output directory for downloaded files"
  echo "   -t     Specify number of threads for fasterq-dump"
  exit 1
}

# No arguments provided
if [ $# -eq 0 ]; then
  usage
fi

# Parse command line arguments
while getopts ":i:o:t:" opt; do
  case $opt in
    i)
      FILE=$OPTARG
      ;;
    o)
      OUTDIR=$OPTARG
      ;;
    t)  # changed 'e' to 't'
      THREADS=$OPTARG
      ;;
    \?)
      usage
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      usage
      ;;
  esac
done

# Check if input file exists
if [ -z "$FILE" ]; then
  echo "Input file not specified!"
  usage
fi

if [ ! -f "$FILE" ]; then
  echo "Input file $FILE not found!"
  usage
fi

# Check if output directory exists
if [ -z "$OUTDIR" ]; then
  echo "Output directory not specified!"
  usage
fi

if [ ! -d "$OUTDIR" ]; then
  mkdir $OUTDIR
fi

# Check if number of threads is specified
if [ -z "$THREADS" ]; then
  echo "Number of threads not specified! Defaulting to 1"
  THREADS=1
fi

FOF="$OUTDIR/fof.txt"

task() {
  bash one_accession_download.sh -a $1 -o $2 -f $3
}
export -f task

cat $FILE | parallel -j $THREADS task {} "$OUTDIR" "$FOF"

