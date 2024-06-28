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

# for each accession in the text file
while IFS= read -r accession
do
  # prefetch the accession
  echo "Prefetching $accession ..."
  prefetch -O $OUTDIR $accession

  # use fasterq-dump to download the sequence data
  echo "Running fasterq-dump on $accession ..."
  fasterq-dump --split-3 --threads $THREADS -O $OUTDIR $accession

  # gzip the files and write to the fof.txt file
  if [[ -f "$OUTDIR/${accession}_1.fastq" && -f "$OUTDIR/${accession}_2.fastq" ]]; then
    echo "Compressing files ..."
    gzip $OUTDIR/${accession}_1.fastq $OUTDIR/${accession}_2.fastq
    echo "${accession} : ${accession}_1.fastq.gz ; ${accession}_2.fastq.gz" >> $FOF
  elif [[ -f "$OUTDIR/${accession}.fastq" ]]; then
    echo "Compressing file ..."
    gzip $OUTDIR/${accession}.fastq
    echo "${accession} : ${accession}.fastq.gz" >> $FOF
  else
    echo "No fastq files found for $accession"
  fi
done < "$FILE"
