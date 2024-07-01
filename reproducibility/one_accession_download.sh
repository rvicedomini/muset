#!/bin/bash
set -e


# checking if gzip, prefetch, and fasterq-dump are available
command -v gzip >/dev/null 2>&1 || { echo >&2 "gzip is required but it's not installed. Exiting."; exit 1; }
command -v prefetch >/dev/null 2>&1 || { echo >&2 "prefetch is required but it's not installed. Exiting."; exit 1; }
command -v fasterq-dump >/dev/null 2>&1 || { echo >&2 "fasterq-dump is required but it's not installed. Exiting."; exit 1; }


# usage function
usage(){
  echo "Script to download SRA for MUSET's experiments"
  echo "Usage: \$0 [-a <accession>] [-o <outputdir>] [-f <fileoffiles>]"
  echo "   -a     Specify accession number to download"
  echo "   -o     Specify output directory for downloaded files"
  echo "   -f     File of file to register the outputs"
  exit 1
}

# No arguments provided
if [ $# -eq 0 ]; then
  usage
fi

# Parse command line arguments
while getopts ":a:o:f:" opt; do
  case $opt in
    a)
      ACCESS=$OPTARG
      ;;
    o)
      OUTDIR=$OPTARG
      ;;
    f)
        FOF=$OPTARG
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

# Check if output directory exists
if [ -z "$OUTDIR" ]; then
  echo "Output directory not specified!"
  usage
fi

if [ ! -d "$OUTDIR" ]; then
  echo "Output directory must be present"
  usage
fi


# prefetch the accession
echo "Prefetching $ACCESS ..."
prefetch -O $OUTDIR $ACCESS

# use fasterq-dump to download the sequence data
echo "Running fasterq-dump on $ACCESS ..."
fasterq-dump --split-3 -O $OUTDIR $ACCESS

echo "Compressing files ..."
if [[ -f "$OUTDIR/${ACCESS}_1.fastq" && -f "$OUTDIR/${ACCESS}_2.fastq" ]]; then
    gzip $OUTDIR/${ACCESS}_1.fastq $OUTDIR/${ACCESS}_2.fastq
    echo "${ACCESS} : ${ACCESS}_1.fastq.gz ; ${ACCESS}_2.fastq.gz" >> $FOF
elif [[ -f "$OUTDIR/${ACCESS}.fastq" ]]; then
    echo "Compressing file ..."
    gzip $OUTDIR/${ACCESS}.fastq
    echo "${ACCESS} : ${ACCESS}.fastq.gz" $FOF
else
    echo "No fastq files found for $ACCESS"
fi

rm -rf $OUTDIR/$ACCESS