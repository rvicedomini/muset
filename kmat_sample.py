#!/usr/bin/env python3
import sys, os, argparse, random

def main(argv=None):
    parser = argparse.ArgumentParser(description='k-mer matrix statistics')
    parser.add_argument('-p', dest='prob', type=float, default=0.125, help='probability to take a k-mer')
    parser.add_argument('-n', dest='num', type=int, default=100000, help='maximum number of k-mer to take')
    parser.add_argument('matrix', metavar='MATRIX', help='k-mer matrix file')
    parser.add_argument('prefix', help='prefix of output files')
    args = parser.parse_args()

    chosen = 0
    with open(args.matrix,'r') as mat, open(f'{args.prefix}.subsample.mat','w') as out:
        for line in mat:
            line = line.strip()
            if line == "":
                continue
            pk = random.random()
            if pk <= args.prob:
                out.write(f'{line}\n')
                chosen += 1
            if chosen >= args.num:
                break
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
