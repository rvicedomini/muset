CC= g++
CFLAGS= -Wall -Wno-unused-function -O3 -pedantic
INC= -Iinclude/essentials/include -Iinclude/pthash -Iinclude

OBJECTS= km_basic_filter.o \
         km_diff.o \
		 km_fasta.o \
		 km_merge.o \
		 km_reverse.o \
		 km_select.o \
		 km_unitig.o \
		 km_tools.o


all: kmtools


%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@ -lz -lm

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) $< -c -o $@ -lz -lm -pthread

kmtools: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lz -lm


km_basic_filter.o: km_basic_filter.c common.h
km_diff.o: km_diff.c common.h
km_fasta.o: km_fasta.c common.h
km_merge.o: km_merge.c common.h
km_reverse.o: km_reverse.c common.h
km_select.o: km_select.c common.h
km_unitig.o: km_unitig.cpp common.h


clean:
	rm -f $(OBJECTS) kmtools
