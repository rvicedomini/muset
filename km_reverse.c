#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

static const unsigned char rctable[256] = {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
     16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
     32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
     48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
     64, 'T',  66, 'G',  68,  69,  70, 'C',  72,  73,  74,  75,  76,  77, 'N',  79,
     80,  81,  82,  83, 'A',  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
     96, 't',  98, 'g', 100, 101, 102, 'c', 104, 105, 106, 107, 108, 109, 'n', 111,
    112, 113, 114, 115, 'a', 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

static const int isnuc[256] = {
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   1,   0,   1,   0,   0,   0,   1,   0,   0,   0,   0,   0,   0,   1,   0,
      0,   0,   0,   0,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   1,   0,   1,   0,   0,   0,   1,   0,   0,   0,   0,   0,   0,   1,   0,
      0,   0,   0,   0,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};


int main(int argc, char **argv) {

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
    fprintf(stdout, "Usage: km_reverse [options] <in.mat>\n\n");
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
      if(outfile != stdout){fclose(outfile);}
      return 2;
    }

    for(int i=0; i<=ksize/2; ++i) {
      kmer[i] = rctable[line[ksize-i-1]];
      kmer[ksize-i-1] = rctable[line[i]];
      if(!isnuc[line[ksize-i-1]] || !isnuc[line[i]]) {
        fprintf(stderr,"[error] invalid k-mer at line %zu: %s\n", line_num, line);
        free(kmer); free(line);
        if(infile != stdin){ fclose(infile); }
        if(outfile != stdout){fclose(outfile);}
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
