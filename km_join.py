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

def extract_next_kmer(fh):
    # retrieve next non-empty line
    line = fh.readline()
    while line and line.strip() == '':
        line = fh.readline()
    # extract k-mer and columns strings
    kmer,cols = line.strip().split(' ',1) if line else [None,None]
    return kmer,cols


def main(argv=None):
    parser = argparse.ArgumentParser(description='Outer join of two k-mer matrices')
    parser.add_argument('-o','--output', dest='output', metavar='PATH', help='write output matrix to file')
    parser.add_argument('mat_1', metavar='MATRIX_1', help='first k-mer matrix file')
    parser.add_argument('mat_2', metavar='MATRIX_2', help='second k-mer matrix file')
    args = parser.parse_args()

    ncols_1 = num_cols(args.mat_1)
    ncols_2 = num_cols(args.mat_2)

    if ncols_1 is None or ncols_1 < 2:
        print(f'Matrix "{args.mat_1}" does not contain any row or any sample',file=sys.stderr)
        return 1
    if ncols_2 is None or ncols_2 < 2:
        print(f'Matrix "{args.mat_2}" does not contain any row or any sample',file=sys.stderr)
        return 1

    nsamples_1 = ncols_1-1
    nsamples_2 = ncols_2-1
    with open_file_or_stdout(args.output) as of:
        with open(args.mat_1,'r') as mat_1, open(args.mat_2,'r') as mat_2:
            # retrieve first kmer of each file
            kmer_1,cols_1 = extract_next_kmer(mat_1)
            kmer_2,cols_2 = extract_next_kmer(mat_2)
            # output the outer join of the two matrices (0 columns are outputted for missing kmers)
            while (kmer_1 is not None) and (kmer_2 is not None):
                if kmer_1 == kmer_2:
                    of.write(f'{kmer_1} {cols_1} {cols_2}\n')
                    kmer_1,cols_1 = extract_next_kmer(mat_1)
                    kmer_2,cols_2 = extract_next_kmer(mat_2)
                elif kmer_1 < kmer_2:
                    of.write(f'{kmer_1} {cols_1} {" ".join(["0"] * nsamples_2)}\n')
                    kmer_1,cols_1 = extract_next_kmer(mat_1)
                else: # kmer_2 < kmer_1
                    of.write(f'{kmer_2} {" ".join(["0"] * nsamples_1)} {cols_2}\n')
                    kmer_2,cols_2 = extract_next_kmer(mat_2)
            # at this point we finished processing either one of the two files
            # output remaining rows
            while kmer_1 is not None:
                of.write(f'{kmer_1} {cols_1} {" ".join(["0"] * nsamples_2)}\n')
                kmer_1,cols_1 = extract_next_kmer(mat_1)
            while kmer_2 is not None:
                of.write(f'{kmer_2} {" ".join(["0"] * nsamples_1)} {cols_2}\n')
                kmer_2,cols_2 = extract_next_kmer(mat_2)

    return 0

if __name__ == "__main__":
    sys.exit(main())
