#ifndef KM_COMMON_H
#define KM_COMMON_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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


static const int n2kt[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};


static const unsigned char kt2n[4] = { 'A', 'C', 'T', 'G' };


static int ktcmp(const char *k1, const char *k2) {

  unsigned char c1;
  unsigned char c2;
  do
  {
    c1 = (unsigned char) *k1++;
    c2 = (unsigned char) *k2++;
    if (c1 == '\0' || c2 == '\0') {
      return c1-c2;
    }
  } while (c1 == c2);

  return n2kt[c1] - n2kt[c2];
}

static int ktncmp(const char *k1, const char *k2, size_t n) {
  unsigned char c1 = '\0';
  unsigned char c2 = '\0';
  while (n > 0) {
    c1 = (unsigned char) *k1++;
    c2 = (unsigned char) *k2++;
    if (c1 == '\0' || c2 == '\0') {
      return c1-c2;
    } else if (c1 != c2) {
      return n2kt[c1] - n2kt[c2];
    }
    n--;
  }

  return n2kt[c1] - n2kt[c2];
}

// compare kmer with its reverse complement
// returns:
// a negative value if kmer is canonical
// 0 if kmer is equal to its reverse complement
// a positive value if kmer is not canonical
static int rccmp(char *kmer, int ksize) {
  char *left = kmer;
  char *right = kmer+ksize-1;
  unsigned char fc, rc;
  for(int i=0; i<ksize; ++i) {
    fc = (unsigned char) *left++;
    rc = (unsigned char) rctable[(int)*right--];
    if (fc != rc) {
      return fc-rc;
    }
  }
  return 0;
}

// same as revcmp but using the kmtricks nucleotide order
static int ktrccmp(char *kmer, int ksize) {
  unsigned char fc, rc;
  for(int i=0; i<ksize; ++i) {
    fc = n2kt[(int)kmer[i]];
    rc = n2kt[(int)rctable[(int)kmer[ksize-i-1]]];
    if (fc != rc) {
      return fc-rc;
    }
  }
  return 0;
}

static inline char * reverse_complement(char *kmer, int ksize) {
    char *out = (char *)calloc(ksize+1,1);
    for(int i=0; i<ksize; ++i) {
      out[i] = rctable[(int)kmer[ksize-i-1]];
    }
    return out;
}

static inline void reverse_complement_inplace(char *kmer, int ksize) {
  int m = 1 + ((ksize-1)>>1);
  for(int l=0; l<m; ++l) {
    int r = ksize-l-1;
    char temp = kmer[l];
    kmer[l] = rctable[(int)kmer[r]];
    kmer[r] = rctable[(int)temp];
  }
}

static char * second_column(char *line) {
  while(*line && *line != ' ' && *line != '\t') { ++line; }
  while(*line && (*line == ' ' || *line == '\t')) { ++line; }
  return line;
}

static size_t samples_number(const char *line) {
  size_t n_samples = 0;
  char *cpy = (char *)malloc(strlen(line)+1);
  strcpy(cpy,line);
  bool first = true;
  char *tok = strtok(cpy," \t\n");
  while(tok) {
    n_samples += (first ? 0 : 1);
    first = false;
    tok = strtok(NULL," \t\n"); 
  }
  free(cpy);
  return n_samples;
}


static char* next_kmer(char *kmer, int ksize, FILE *stream) {

  int c;

  // read first ksize characters in buf
  for(int i=0; i<ksize; i++) {
    c = getc(stream);
    if(!isalpha(c)) { return NULL; }
    kmer[i] = c;
  }

  // discard following characters until the end of line or EOF
  c = getc(stream);
  while(c != '\n' && c != EOF) { c = getc(stream); }

  return kmer;
}


static bool next_kmer_and_line(char *kmer, int ksize, char **line, size_t *line_size, FILE *stream) {
  
  ssize_t len = getline(line, line_size, stream);
  
  if(len < ksize) {
    return false;
  }

  // read first ksize characters in buf
  for(int i=0; i<ksize; i++) {
    if(!isnuc[(int)(*line)[i]]){
      fprintf(stderr, "[warning] input does not seem valid\n");
      return false; 
    }
    kmer[i] = (*line)[i];
  }

  // possibly remove trailing newline character
  if(len > 0 && (*line)[len-1]=='\n') { (*line)[len-1] = '\0'; }

  return true;
}


static int64_t encode_kmer(char *kmer, int ksize) {
  int64_t ret = 0;
  for(int i=0; i<ksize; ++i) {
    ret = (ret << 2) | ((kmer[i] >> 1) & 3);
  }
  return ret;
}

#endif