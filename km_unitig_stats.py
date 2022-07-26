#!/usr/bin/env python3
import sys, os, argparse, logging
from collections import defaultdict
from statistics import median

logger = logging.getLogger()

def init_logging():
    global logger
    log_formatter = logging.Formatter('[{asctime}] {levelname}: {message}', datefmt='%Y-%m-%d %H:%M:%S', style='{')
    stream_handler = logging.StreamHandler(stream=sys.stderr)
    stream_handler.setFormatter(log_formatter)
    logger.setLevel(logging.INFO)
    logger.addHandler(stream_handler)


def main(argv=None):
    parser = argparse.ArgumentParser(description='Partition a k-mer matrix according to a set of unitigs')
    parser.add_argument('-m','--matrix', dest='kmat', metavar='PATH', required=True, help='Input k-mer matrix')
    parser.add_argument('-s','--sample-ids', dest='samples', metavar='PATH', required=True, help='File containing sample IDs of matrix columns')
    args = parser.parse_args()

    init_logging()

    is_input_valid=True
    if not os.path.isfile(args.kmat):
        is_input_valid=False
        logger.error(f'-m/--mat file "{args.kmat}" does not exist.')
    if not is_input_valid:
        return 1

    sample_id = []
    with open(args.samples,'r') as sf:
        for line in sf:
            line = line.strip()
            if line == '':
                continue
            sample_id.append(line)
    n_samples = len(sample_id)
    logger.info(f'samples: {n_samples}')
   
    utg_mat = defaultdict(list)
    with open(args.kmat,'r') as kmat:
        for line in kmat:
            line = line.strip()
            if line == '':
                continue
            cols = line.split()
            utgid,values = cols[0],list(map(int,cols[2:]))
            assert(len(values)==n_samples)
            utg_mat[utgid].append(values)
    logger.info(f'unitigs: {len(utg_mat)}')

    print('UTG_ID\tSAMPLE_ID\tMEAN_COUNT\tMEDIAN_COUNT\tKMER_FRAC')
    for utgid, umat in utg_mat.items():
        n_kmers = len(umat)
        tot_count = [0] * n_samples
        pa_count = [0] * n_samples
        for row in umat:
            for i in range(n_samples):
                tot_count[i] += row[i]
                pa_count[i] += 1 if row[i] > 0 else 0
        medians = [median([row[i] for row in umat]) for i in range(n_samples)]
        for i in range(n_samples):
            print(f'{utgid}\t{sample_id[i]}\t{tot_count[i]/n_kmers:.2f}\t{medians[i]:.1f}\t{pa_count[i]/n_kmers:.2f}')
    return 0


if __name__ == "__main__":
    sys.exit(main())
