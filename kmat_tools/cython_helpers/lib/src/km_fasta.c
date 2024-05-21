#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"

int main_fasta(int argc, char **argv) {

  char *out_fname = NULL;
  bool help_opt = false;

  int c;
  while ((c = getopt(argc, argv, "o:h")) != -1) {
    switch (c) {
      case 'o':
        out_fname = optarg;
        break;
      case 'h':
        help_opt = true;
        break;
      case '?':
        return 1;
      default:
        abort();
    }
  }

  if(argc-optind != 1 || help_opt) {
    fprintf(stdout, "Usage: km_fasta [options] <in.mat>\n\n");

    fprintf(stdout, "Outputs k-mers of a k-mer matrix in a FASTA file.\n");
    fprintf(stdout, "k-mer size is inferred from the first non-empty line.\n\n");
    
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -o FILE  output FASTA file of k-mers to FILE [stdout]\n");
    fprintf(stdout, "  -h       print this help message\n");
    return 0;
  }

  FILE *fp = strcmp(argv[optind],"-") ? fopen(argv[optind],"r") : stdin;
  if(fp == NULL) {
    fprintf(stderr,"[error] cannot open file \"%s\"\n",argv[optind]);
    return 1;
  }

  FILE *outfile = out_fname ? fopen(out_fname,"w") : stdout;
  if(outfile != stdout && outfile == NULL) {
    if(fp != stdin){ fclose(fp); }
    fprintf(stderr,"[error] cannot open output file \"%s\"\n",out_fname);
    return 1;
  }

  char *line = NULL;
  size_t line_size=0, line_num=0, kmer_count=0;
  
  ssize_t ch_read = getline(&line, &line_size, fp);
  while(ch_read >= 0) {
    ++line_num;

    if(ch_read == 0 || line[0]=='\n') { // skip empty lines
      continue;
    }

    bool valid_kmer = true;    
    char *kmer = strtok(line," \t\n");
    if(kmer == NULL){ 
      valid_kmer = false;
    }
    else 
    {
      while(*kmer && isnuc[(int)*kmer]) { ++kmer; }
      valid_kmer = *kmer == '\0';
    }

    if(valid_kmer) {
      fprintf(outfile, ">%zu\n%s\n", ++kmer_count, line);
    } else {
      fprintf(stderr,"[warning] invalid k-mer at line %zu: %s\n", line_num, line);
    }

    ch_read = getline(&line, &line_size, fp);
  }

  fprintf(stderr, "[info] %zu k-mers outputted.\n", kmer_count);
  free(line);
  if(fp != stdin) { fclose(fp); }
  if(outfile != stdout){ fclose(outfile); }

  return 0;
}