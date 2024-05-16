#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"


int main_reverse(int argc, char **argv) {

  int ksize = 31;
  char *out_fname = NULL;
  bool help_opt = false;

  int c;
  while ((c = getopt(argc, argv, "k:o:h")) != -1) {
    switch (c) {
      case 'k':
        ksize = strtol(optarg, NULL, 10);
        break;
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

  if(ksize <= 0) { 
    fprintf(stderr, "[error] invalid value of k: %d\n", ksize);
    return 1;
  }

  if(argc-optind != 1 || help_opt) {
    fprintf(stdout, "Usage: kmtools reverse [options] <in.mat>\n\n");
    fprintf(stdout, "Reverse-complement all k-mers in a k-mer matrix file.\n\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -k INT   k-mer size [31]\n");
    fprintf(stdout, "  -o FILE  output reverse-complement matrix to FILE [stdout]\n");
    fprintf(stdout, "  -h       print this help message\n");
    return 0;
  }

  FILE *infile = strcmp(argv[optind],"-") ? fopen(argv[optind],"r") : stdin;
  if(infile == NULL) {
    fprintf(stderr,"[error] cannot open file \"%s\"\n",argv[optind]);
    return 1;
  }

  FILE *outfile = out_fname ? fopen(out_fname,"w") : stdout;
  if(outfile != stdout && outfile == NULL) {
    if(infile != stdin){ fclose(infile); }
    fprintf(stderr,"[error] cannot open output file \"%s\"\n",out_fname);
    return 1;
  }

  char *kmer = (char *)calloc(ksize+1,1);
  char *line = NULL;
  size_t line_size=0, line_num=0;

  ssize_t ch_read = getline(&line, &line_size, infile);
  while(ch_read >= 0) {
    ++line_num;
    
    // empty line
    if(ch_read == 0 || line[0]=='\n') { 
      continue; 
    }
    
    if(ch_read < ksize) {
      fprintf(stderr,"[error] cannot read a k-mer of size %d at line %zu\n", ksize, line_num);
      free(kmer); free(line);
      if(infile != stdin){ fclose(infile); }
      if(outfile != stdout){ fclose(outfile); }
      return 2;
    }

    for(int i=0; i<=ksize/2; ++i) {
      kmer[i] = rctable[(int)line[ksize-i-1]];
      kmer[ksize-i-1] = rctable[(int)line[i]];
      if(!isnuc[(int)line[ksize-i-1]] || !isnuc[(int)line[i]]) {
        fprintf(stderr,"[error] invalid k-mer at line %zu: %s\n", line_num, line);
        free(kmer); free(line);
        if(infile != stdin){ fclose(infile); }
        if(outfile != stdout){ fclose(outfile); }
        return 2;
      }
    }

    for(int i=0; i<ksize; ++i) {
      line[i] = kmer[i];
    }

    fputs(line, outfile);
    //fputc('\n', outfile);
    ch_read = getline(&line, &line_size, infile);
  }

  fprintf(stderr,"[info] %zu lines processed successfully\n", line_num);
  free(kmer); free(line);
  if(infile != stdin){ fclose(infile); }
  if(outfile != stdout){ fclose(outfile); }

  return 0;
}
