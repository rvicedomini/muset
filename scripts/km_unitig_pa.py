#!/usr/bin/env python3
import sys, os, argparse, logging
import operator
from collections import defaultdict

from Bio.SeqIO.FastaIO import SimpleFastaParser

logger = logging.getLogger()

RC_TABLE = str.maketrans("ACTGNactgn", "TGACNtgacn")
def reverse_complement(seq):
    return seq.translate(RC_TABLE)[::-1]

def init_logging():
    global logger
    log_formatter = logging.Formatter('[{asctime}] {levelname}: {message}', datefmt='%Y-%m-%d %H:%M:%S', style='{')
    stream_handler = logging.StreamHandler(stream=sys.stderr)
    stream_handler.setFormatter(log_formatter)
    logger.setLevel(logging.INFO)
    logger.addHandler(stream_handler)

def kmer_set(seq,k):
    fwd_kmers = set(seq[i:i+k] for i in range(len(seq)-k+1))
    rev_kmers = set(map(reverse_complement,fwd_kmers))
    return (fwd_kmers|rev_kmers), len(seq)-k+1


def main(argv=None):
    parser = argparse.ArgumentParser(description='Partition a k-mer matrix according to a set of unitigs')
    parser.add_argument('-m','--mat', dest='kmat', metavar='PATH', required=True, help='Input k-mer matrix')
    parser.add_argument('-f','--fasta', dest='fasta', metavar='PATH', required=True, help='Unitig file in FASTA format')
    parser.add_argument('-o','--out', dest='out', metavar='PATH', required=True, help='Output file')
    parser.add_argument('-k', dest='ksize', metavar='INT', type=int, default=31, help='k-mer size')
    parser.add_argument('-c', dest='min_kc', metavar='INT', type=int, default=5, help='minimum k-mer count to define it as present in a sample')
    args = parser.parse_args()

    init_logging()

    is_input_valid=True
    if not os.path.isfile(args.kmat):
        is_input_valid=False
        logger.error(f'-m/--mat file "{args.kmat}" does not exist.')
    elif not os.path.isfile(args.fasta):
        is_input_valid=False
        logger.error(f'-f/--fasta file "{args.fasta}" does not exist.')
    if not is_input_valid:
        return 1

    utg_kmers = {}
    utg_size = {}
    kmer_utg = {}
    with open(args.fasta,'r') as infas:
        for header, sequence in SimpleFastaParser(infas):
            utgid = header.split()[0]
            utg_kmers[utgid],utg_size[utgid] = kmer_set(sequence,args.ksize)
            for kmer in utg_kmers[utgid]:
                kmer_utg[kmer] = utgid
    logger.info(f'{len(utg_kmers)} unitig processed -> {sum(map(len,utg_kmers.values()))} kmers')
    
    logger.info(f'Splitting k-mers from the matrix')
    n_samples = None
    utg_dict = {}
    with open(args.kmat,'r') as mat:
        for line in mat:
            kmer,col_str = line.strip().split(' ',1)
            if kmer in kmer_utg:
                utgid = kmer_utg[kmer]
                cols = col_str.split()
                if n_samples is None:
                    n_samples = len(cols)
                if utgid not in utg_dict:
                    utg_dict[utgid] = [0]*n_samples
                utg_dict[utgid] = list(map(operator.add,utg_dict[utgid],(int(kc>=args.min_kc) for kc in map(int,cols))))
    
    logger.info(f'Writing utg matrix')
    with open(args.out,'w') as of:
        for utgid, utglist in utg_dict.items():
            of.write(f'{utgid} ')
            utg_n_kmers = utg_size[utgid]
            of.write(' '.join((str(int(s==utg_n_kmers)) for s in utglist)))
            of.write('\n')
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
