#!/usr/bin/env python3
import sys
import argparse
import contextlib


@contextlib.contextmanager
def open_file_or_stdout(filename=None):
    fh = open(filename,'w') if filename else sys.stdout
    try:
        yield fh
    finally:
        if fh is not sys.stdout:
            fh.close()

def num_cols(filename):
    with open(filename,'r') as fh:
        line = fh.readline()
        return len(line.strip().split()) if line else None

def next_kmer(fh):
    # retrieve next non-empty line
    line = fh.readline()
    while line and line.strip() == '':
        line = fh.readline()
    line = line.strip()
    return (line.split(maxsplit=1)[0],line) if line else (None,'')


def main(argv=None):
    parser = argparse.ArgumentParser(description='Remove a sorted set of kmers from a sorted matrix')
    parser.add_argument('-o','--output', dest='output', metavar='PATH', help='output matrix file')
    parser.add_argument('kfile', metavar='KMERS', help='sorted file of k-mers to be removed')
    parser.add_argument('mat', metavar='MATRIX', help='sorted k-mer matrix file')
    args = parser.parse_args()

    count = 0
    with open_file_or_stdout(args.output) as out:
        with open(args.kfile,'r') as kf, open(args.mat,'r') as mat:
            # retrieve first kmer of each file
            rm_kmer,_ = next_kmer(kf)
            mat_kmer,mat_line = next_kmer(mat)
            while rm_kmer and mat_kmer:
                if rm_kmer == mat_kmer:
                    rm_kmer,_ = next_kmer(kf)
                    mat_kmer,mat_line = next_kmer(mat)
                    count += 1
                    if count % 16000000 == 0:
                        sys.stderr.write(f'{count} kmers processed in the matrix\n')
                elif rm_kmer < mat_kmer:
                    rm_kmer,_ = next_kmer(kf)
                else: # mat_kmer < rm_kmer
                    out.write(f'{mat_line}\n')
                    mat_kmer,mat_line = next_kmer(mat)
                    count += 1
                    if count % 16000000 == 0:
                        sys.stderr.write(f'{count} kmers processed in the matrix\n')
            while mat_kmer:
                out.write(f'{mat_line}\n')
                mat_kmer,mat_line = next_kmer(mat)
                count += 1
                if count % 16000000 == 0:
                    sys.stderr.write(f'{count} kmers processed in the matrix\n')

    return 0

if __name__ == "__main__":
    sys.exit(main())
