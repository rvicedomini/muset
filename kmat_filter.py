#!/usr/bin/env python3
import sys, os, argparse
import lz4.frame


def main(argv=None):
    parser = argparse.ArgumentParser(description='k-mer matrix statistics')
    parser.add_argument('matrix', metavar='MATRIX', help='k-mer matrix file')
    parser.add_argument('prefix', help='prefix of output files')
    args = parser.parse_args()

    n_gt255 = 0     # number of k-mers with abundance >= 255 in all samples
    n_erroneous = 0 # number of k-mers with abundance in [1,9] in at most 10 samples
    n_kmers = 0     # number of k-mers in the matrix
    n_samples = 0   # number of samples

    with lz4.frame.open(args.matrix, mode='rt') as matrix:
        line = matrix.readline()
        n_samples = len(line.strip().split(' '))-1

    with lz4.frame.open(args.matrix,'rt') as matrix, lz4.frame.open(f'{args.prefix}.flt.mat.lz4','wt') as flt, lz4.frame.open(f'{args.prefix}.high.mat.lz4','wt') as high, lz4.frame.open(f'{args.prefix}.low.mat.lz4','wt') as low:
        for line in matrix:
            line = line.strip()
            if line == "":
                continue
            n_kmers += 1
            kmer,cols_str = line.split(' ',1)
            cols = list(map(int,cols_str.split(' ')))
            # count high abundance k-mer in all samples
            if all(n >= 255 for n in cols):
                n_gt255 += 1
                high.write(f'{line}\n')
                continue
            # count putative erroneous k-mers
            n_zeros_col = sum(1 for n in cols if n==0)
            n_low_ab = sum(1 for n in cols if n >= 1 and n <= 9)
            if (n_low_ab <= 10) and (n_low_ab + n_zeros_col == n_samples):
                n_erroneous += 1
                low.write(f'{line}\n')
                continue
            flt.write(f'{line}\n')

    print(f'{n_kmers}\tk-mers')
    print(f'{n_samples}\tsamples')
    print(f'{n_gt255}\thigh-abundance k-mers filtered (>= 255 in all samples)')
    print(f'{n_erroneous}\tlow-abundance k-mers filtered (>=1 and <=9 in at most 10 samples, zeros in all others)')

    return 0

if __name__ == "__main__":
    sys.exit(main())
