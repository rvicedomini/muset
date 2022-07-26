#!/usr/bin/env python3
import sys, os, argparse

import numpy as np
from scipy.stats import shapiro


def main(argv=None):
    parser = argparse.ArgumentParser(description='K-mer matrix statistics')
    parser.add_argument('matrix', metavar='MATRIX', help='K-mer matrix file')
    args = parser.parse_args()

    n_gt255 = 0 # number of k-mers with abundance >= 255 in all samples
    n_erroneous = 0 # number of k-mers with abundance in [1,9] in at most 10 samples
    n_norm = 0 # number of k-mers with abundance which follows a normal distribution among the samples (P-value > 0.05)
    n_zeros = 0 # number of zeros in the matrix
    n_kmers = 0 # number of k-mers in the matrix
    n_samples = 0 # number of samples

    with open(args.matrix,'r') as matrix:
        line = matrix.readline()
        n_samples = len(line.strip().split(' '))-1

    with open(args.matrix,'r') as matrix:
        for line in matrix:
            line = line.strip()
            if line == "":
                continue
            n_kmers += 1
            kmer,cols_str = line.split(' ',1)
            cols = list(map(int,cols_str.split(' ')))
            n_zeros_col = sum(1 for n in cols if n==0)
            n_zeros += n_zeros_col
            # count high abundance k-mer in all samples
            if all(n >= 255 for n in cols):
                n_gt255 += 1
                continue
            # count putative erroneous k-mers
            n_low_ab = sum(1 for n in cols if n >= 1 and n <= 9)
            if (n_low_ab <= 10) and (n_low_ab + n_zeros_col == n_samples):
                n_erroneous += 1
                continue
            # count k-mers that have are likely to have a normal distribution
            # among the samples
            #_,pvalue = shapiro(np.array(cols,dtype=float))
            #if pvalue > 0.05:
            #    n_norm += 1

    print(f'kmers\t{n_kmers}')
    print(f'samples\t{n_samples}')
    print(f'gt255\t{n_gt255} ({100*n_gt255/n_kmers:.2f}%)')
    print(f'erroneous\t{n_erroneous} ({100*n_erroneous/n_kmers:.2f}%)')
    print(f'norm_dist\t{n_norm} ({100*n_norm/n_kmers:.2f}%)')
    print(f'zeros/elements\t{n_zeros}/{n_kmers*n_samples} ({(100*n_zeros)/(n_kmers*n_samples):.2f})%')

    return 0

if __name__ == "__main__":
    sys.exit(main())
