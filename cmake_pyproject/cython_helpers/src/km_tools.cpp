#include <stdio.h>
#include <string.h>

#define KMTOOLS_VERSION "v0.2"


int main_diff(int argc, char *argv[]);
int main_fasta(int argc, char *argv[]);
int main_fafmt(int argc, char *argv[]);
int main_basic_filter(int argc, char *argv[]);
int main_merge(int argc, char *argv[]);
int main_reverse(int argc, char *argv[]);
int main_select(int argc, char *argv[]);
int main_unitig(int argc, char *argv[]);


static int usage()
{
	fprintf(stderr, "kmtools %s\n\n", KMTOOLS_VERSION);

	fprintf(stderr, "DESCRIPTION\n");
	fprintf(stderr, "  kmtools - a collection of tools to process text-based k-mer matrices\n\n");
	
	fprintf(stderr, "USAGE\n");
	fprintf(stderr, "  kmtools <command> <arguments>\n\n");
	
	fprintf(stderr, "COMMANDS\n");
	fprintf(stderr, "  diff    - difference between two sorted k-mer matrices\n");
    fprintf(stderr, "  fasta   - output a k-mer matrix in FASTA format\n");
	fprintf(stderr, "  fafmt   - filter a FASTA file by length and write sequences in single lines\n");
	fprintf(stderr, "  filter  - filter a k-mer matrix by selecting k-mers that are potentially differential\n");
    fprintf(stderr, "  merge   - merge two input sorted k-mer matrices\n");
    fprintf(stderr, "  reverse - reverse complement k-mers in a matrix\n");
    fprintf(stderr, "  select  - select only a subset of k-mers\n");
    fprintf(stderr, "  unitig  - build a unitig matrix\n");
	fprintf(stderr, "  version - print version\n");
	fprintf(stderr, "\n");
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) { 
		return usage(); 
	}
	
	if (strcmp(argv[1], "diff") == 0) { return main_diff(argc-1, argv+1); }
    else if (strcmp(argv[1], "fasta") == 0) { return main_fasta(argc-1, argv+1); }
	else if (strcmp(argv[1], "fafmt") == 0) { return main_fafmt(argc-1, argv+1); }
	else if (strcmp(argv[1], "filter") == 0) { return main_basic_filter(argc-1, argv+1); }
	else if (strcmp(argv[1], "merge") == 0) { return main_merge(argc-1, argv+1); }
	else if (strcmp(argv[1], "reverse") == 0) { return main_reverse(argc-1, argv+1); }
	else if (strcmp(argv[1], "select") == 0) { return main_select(argc-1, argv+1); }
	else if (strcmp(argv[1], "unitig") == 0) { return main_unitig(argc-1, argv+1); }
	
	if (strcmp(argv[1], "version") == 0 || strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
		fprintf(stderr, "kmtools %s\n", KMTOOLS_VERSION);
		return 0;
	}
	
    fprintf(stderr, "[error] unrecognized command \"%s\"\n", argv[1]);
	return 1;
}