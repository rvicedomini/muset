import argparse
import km_unitig  

def parse_arguments():
    parser = argparse.ArgumentParser(description="Creates a unitig matrix.")

    parser.add_argument("unitigs_file", metavar="<unitigs.fasta>", help="Input file containing unitigs in FASTA format")
    parser.add_argument("kmer_matrix", metavar="<kmer_matrix>", help="Input file containing kmer matrix")
    parser.add_argument("-k", type=int, default=31, help="Size of k-mers of input matrices (default: 31)")
    parser.add_argument("-l",  type=int, default=0, help="Minimum length of unitigs to consider (default: 0)")
    parser.add_argument("-o", metavar="FILE", help="Write unitig matrix to FILE (default: stdout)")
    
    return parser

def km_unitig_main():
    args_km_unitig=["filter"]
    try:
        parser=parse_arguments()
        args=parser.parse_args()
        for name, value in vars(args).items():
            if value!=None and name!="unitigs_file" and name!="kmer_matrix":
                args_km_unitig=args_km_unitig+["-"+name,str(value)]
        args_km_unitig=args_km_unitig+[args.unitigs_file,args.kmer_matrix]
        km_unitig.py_main_unitig(args_km_unitig)
        
        return 0

    except Exception as e:
        print(f"Error: {str(e)}")
        return 1

if __name__ == "__main__":
    km_unitig_main()
