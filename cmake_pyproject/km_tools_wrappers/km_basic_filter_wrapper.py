import argparse
import km_basic_filter  

def parse_arguments():
    parser = argparse.ArgumentParser(description="Filter a matrix by selecting k-mers that are potentially differential.")
    
    parser.add_argument("in_mat", metavar="<in.mat>", type=str, help="Input matrix file")
    parser.add_argument("-a", type=int, default=10, help="Minimum abundance to define a k-mer as present in a sample [10]")
    parser.add_argument("-n", type=int, default=10, help="Minimum number of samples for which a k-mer should be absent [10]")
    parser.add_argument("-f", type=float, help="Fraction of samples for which a k-mer should be absent (overrides -n)")
    parser.add_argument("-N", type=int, default=10, help="Minimum number of samples for which a k-mer should be present [10]")
    parser.add_argument("-F", type=float, help="Fraction of samples for which a k-mer should be present (overrides -N)")
    parser.add_argument("-o", type=str, help="Output filtered matrix to FILE [stdout]")
    
    return parser

def km_basic_filter_main():
    args_km_basic_filter=["filter"]
    try:
        parser=parse_arguments()
        args=parser.parse_args()
        for name, value in vars(args).items():
            if value!=None and name!="in_mat":
                args_km_basic_filter=args_km_basic_filter+["-"+name,str(value)]
        args_km_basic_filter=args_km_basic_filter+[args.in_mat]
        km_basic_filter.py_main_basic_filter(args_km_basic_filter)
        
        return 0

    except Exception as e:
        print(f"Error: {str(e)}")
        return 1

if __name__ == "__main__":
    km_basic_filter_main()
