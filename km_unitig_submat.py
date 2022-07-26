#!/usr/bin/env python3
import sys, os, argparse, logging
from contextlib import ExitStack
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
    parser.add_argument('-o','--out-dir', dest='outdir', metavar='PATH', required=True, help='Output directory')
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

    if os.path.isdir(f'{args.outdir}') and len(os.listdir(f'{args.outdir}')) != 0:
        logger.error(f'-o/--out-dir {args.outdir} directory must not exist or be empty')
        return 1

    seq_dict = {}
    kmer_dict = {}
    with open(args.fasta,'r') as infas:
        for header, sequence in SimpleFastaParser(infas):
            seqid = header.split()[0]
            seq_dict[seqid] = kmer_set(sequence,args.ksize)
            for kmer in seq_dict[seqid]:
                assert(kmer not in kmer_dict)
                kmer_dict[kmer] = seqid
    logger.info(f'{len(seq_dict)} sequences processed -> {sum(len(x) for x in seq_dict.values())} kmers')
    
    logger.info(f'Splitting k-mers from the matrix')
    os.makedirs(f'{args.outdir}',exist_ok=True)
    with ExitStack() as stack:
        out_files = dict()
        for seqid in seq_dict.keys():
            out_files[seqid] = stack.enter_context(open(f'{args.outdir}/{seqid}.rows.mat','w'))
        count = 0
        with open(args.kmat,'r') as mat:
            for line in mat:
                count += 1
                kmer,cols = line.strip().split(' ',1)
                if count % 16000000 == 0:
                    logger.info(f'{count} million k-mers processed')
                if kmer in kmer_dict:
                    seqid = kmer_dict[kmer]
                    out_files[seqid].write(f'{kmer} {cols}\n')

    return 0

if __name__ == "__main__":
    sys.exit(main())
