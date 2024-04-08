CC= gcc
CFLAGS= -Wall -Wno-unused-function -O3 -pedantic
INCLUDE_DIR = include
SRC_DIR = src
BIN_DIR = bin

OBJECTS= $(BIN_DIR)/km_basic_filter.o \
         $(BIN_DIR)/km_diff.o \
		 $(BIN_DIR)/km_fasta.o \
		 $(BIN_DIR)/km_merge.o \
		 $(BIN_DIR)/km_reverse.o \
		 $(BIN_DIR)/km_select.o \
		 $(BIN_DIR)/km_unitig.o \
		 $(BIN_DIR)/km_tools.o

.PHONY: all clean

all: $(BIN_DIR)/kmtools

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $< -c -o $@ -lz -lm

$(BIN_DIR)/kmtools: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) -lz -lm

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/km_basic_filter.o: $(SRC_DIR)/km_basic_filter.c $(INCLUDE_DIR)/common.h
$(BIN_DIR)/km_diff.o: $(SRC_DIR)/km_diff.c $(INCLUDE_DIR)/common.h
$(BIN_DIR)/km_fasta.o: $(SRC_DIR)/km_fasta.c $(INCLUDE_DIR)/common.h
$(BIN_DIR)/km_merge.o: $(SRC_DIR)/km_merge.c $(INCLUDE_DIR)/common.h
$(BIN_DIR)/km_reverse.o: $(SRC_DIR)/km_reverse.c $(INCLUDE_DIR)/common.h
$(BIN_DIR)/km_select.o: $(SRC_DIR)/km_select.c $(INCLUDE_DIR)/common.h
$(BIN_DIR)/km_unitig.o: $(SRC_DIR)/km_unitig.c $(INCLUDE_DIR)/common.h $(INCLUDE_DIR)/khash.h $(INCLUDE_DIR)/kseq.h

clean:
	rm -f $(OBJECTS) $(BIN_DIR)/kmtools
