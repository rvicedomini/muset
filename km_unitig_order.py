#!/usr/bin/env python3
import sys, os, argparse, logging

from Bio.SeqIO.FastaIO import SimpleFastaParser

logger = logging.getLogger()

def init_logging():
    global logger
    log_formatter = logging.Formatter('[{asctime}] {levelname}: {message}', datefmt='%Y-%m-%d %H:%M:%S', style='{')
    stream_handler = logging.StreamHandler(stream=sys.stderr)
    stream_handler.setFormatter(log_formatter)
    logger.setLevel(logging.INFO)
    logger.addHandler(stream_handler)

RC_TABLE = str.maketrans("ACTGNactgn", "TGACNtgacn")
def reverse_complement(seq):
    return seq.translate(RC_TABLE)[::-1]

def kmer_list(seq,k):
    return [seq[i:i+k] for i in range(len(seq)-k+1)]


def main(argv=None):
    parser = argparse.ArgumentParser(description='Sort k-mers in a (small!) matrix according to their position within a given unitig')
    parser.add_argument('-m','--mat', dest='kmat', metavar='PATH', required=True, help='Input k-mer matrix (it will be loaded in memory)')
    parser.add_argument('-f','--fasta', dest='fasta', metavar='PATH', required=True, help='Unitig in FASTA format')
    parser.add_argument('-o','--output', dest='out', metavar='PATH', required=True, help='Output file name')
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
    elif os.path.isfile(args.out) and os.stat(args.out).st_size > 0:
        is_input_valid=False
        logger.error(f'-o/--output file "{args.out}" already exists.')
    if not is_input_valid:
        return 1

    # load matrix in a dictionary
    krow_dict = {}
    with open(f'{args.kmat}','r') as mf:
        for line in mf:
            line = line.strip()
            if line != "":
                kmer = line[:line.find(' ')]
                assert(kmer not in krow_dict)
                krow_dict[kmer] = line
    logger.info(f'kmers loaded: {len(krow_dict)}')

    k = args.ksize
    with open(args.fasta,'r') as infas, open(args.out,'w') as out:
        for _,seq in SimpleFastaParser(infas):
            fwd = kmer_list(seq,args.ksize)
            rev = kmer_list(reverse_complement(seq),args.ksize)[::-1]
            for i in range(len(fwd)):
                assert(fwd[i] in krow_dict or rev[i] in krow_dict)
                assert(fwd[i] not in krow_dict or rev[i] not in krow_dict)
                out.write(krow_dict[fwd[i]] if fwd[i] in krow_dict else krow_dict[rev[i]])
                out.write('\n')

    return 0

if __name__ == "__main__":
    sys.exit(main())
