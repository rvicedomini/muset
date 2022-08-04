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
    out = set()
    for i in range(len(seq)-k+1):
        out.add(seq[i:i+k])
    rcseq = reverse_complement(seq)
    for i in range(len(rcseq)-k+1):
        out.add(rcseq[i:i+k])
    return out


def main(argv=None):
    parser = argparse.ArgumentParser(description='Partition a k-mer matrix according to a set of unitigs')
    parser.add_argument('-m','--mat', dest='kmat', metavar='PATH', required=True, help='Input k-mer matrix')
    parser.add_argument('-f','--fasta', dest='fasta', metavar='PATH', required=True, help='Unitig file in FASTA format')
    parser.add_argument('-o','--out', dest='out', metavar='PATH', required=True, help='Output file')
    parser.add_argument('-k', dest='ksize', metavar='INT', type=int, default=31, help='k-mer size')
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

    seq_dict = {}
    kmer_dict = {}
    with open(args.fasta,'r') as infas:
        for header, sequence in SimpleFastaParser(infas):
            utgid = header.split()[0]
            seq_dict[utgid] = kmer_set(sequence,args.ksize)
            for kmer in seq_dict[utgid]:
                assert(kmer not in kmer_dict)
                kmer_dict[kmer] = utgid
    logger.info(f'{len(seq_dict)} sequences processed -> {sum(len(x) for x in seq_dict.values())} kmers')
    
    logger.info(f'Splitting k-mers from the matrix')
    n_samples = None
    n_kmers = defaultdict(int)
    utg_dict = {}
    with open(args.kmat,'r') as mat:
        for line in mat:
            kmer,col_str = line.strip().split(' ',1)
            if kmer in kmer_dict:
                utgid = kmer_dict[kmer]
                n_kmers[utgid]+=1
                cols = col_str.split()
                if n_samples is None:
                    n_samples = len(cols)
                if utgid not in utg_dict:
                    utg_dict[utgid] = [0]*n_samples
                utg_dict[utgid] = list(map(operator.add,utg_dict[utgid],(int(x) for x in cols)))
    
    logger.info(f'Writing utg matrix')
    with open(args.out,'w') as of:
        for utgid, utglist in utg_dict.items():
            of.write(f'{utgid}')
            utg_kmers = n_kmers[utgid]
            for s in utglist:
                of.write(f' {round(s/utg_kmers)}')
            of.write('\n')
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
