import argparse
import km_fasta

def parse_arguments():
    parser = argparse.ArgumentParser(description="Outputs k-mers of a k-mer matrix in a FASTA file.")

    parser.add_argument("in_mat", metavar="<in.mat>", help="Input file containing k-mer matrix")
    parser.add_argument("-o",metavar="FILE", help="Output FASTA file of k-mers to FILE (default: stdout)")
    return parser

def km_fasta_main():
    args_km_fasta=["fasta"]
    try:
        parser=parse_arguments()
        args=parser.parse_args()
        for name, value in vars(args).items():
            if value!=None and name!="in_mat":
                args_km_fasta=args_km_fasta+["-"+name,str(value)]
        args_km_fasta=args_km_fasta+[args.in_mat]
        km_fasta.py_main_fasta(args_km_fasta)
        
        return 0

    except Exception as e:
        print(f"Error: {str(e)}")
        return 1

if __name__ == "__main__":
    km_fasta_main()
