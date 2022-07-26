#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>


int main(int argc, char **argv) {

  int ksize = 31;
  char *out_fname = NULL;

  int c;
  while ((c = getopt(argc, argv, "k:o:")) != -1) {
    switch (c) {
      case 'k':
        ksize = strtol(optarg, NULL, 10);
        break;
      case 'o':
        out_fname = optarg;
        break;
      case '?':
        return 1;
      default:
        abort();
    }
  }

  if(ksize <= 0) { 
    fprintf(stderr, "Invalid value of k: %d\n",ksize);
    return 1;
  }

  if(argc-optind != 1) {
    fprintf(stderr, "Usage: %s [-k INT] [-o PATH] matrix_file\n", argv[0]);
    return 1;
  }

  FILE *fp = fopen(argv[optind],"r");
  if(fp == NULL) { fprintf(stderr,"Cannot open file \"%s\"\n",argv[optind]); return 1; }

  FILE *outfile = out_fname ? fopen(out_fname,"w") : stdout;
  if(outfile != stdout && outfile == NULL) { fclose(fp); fprintf(stderr,"[error] cannot open output file \"%s\"\n",out_fname); return 1; }

  char *line = NULL;
  size_t line_size=0, line_num=0, count=0;
  
  ssize_t ch_read = getline(&line, &line_size, fp);
  while(ch_read >= 0) {
    ++line_num;
    
    if(ch_read == 0 || (ch_read == 1 && line[0]=='\n')) { 
      continue; 
    } else if (ch_read < ksize) {
      fprintf(stderr,"[error] cannot read a k-mer of size %d at line %zu\n", ksize, line_num);
      free(line);
      fclose(fp);
      if(outfile != stdout){fclose(outfile);}
      return 2;
    }

    line[ksize] = '\0';
    fprintf(outfile, ">%zu\n%s\n", ++count, line);
    ch_read = getline(&line, &line_size, fp);
  }

  free(line);
  fclose(fp);
  if(outfile != stdout){ fclose(outfile); }

  return 0;
}
