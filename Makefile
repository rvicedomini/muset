CC= gcc
CFLAGS= -Wall -Wno-unused-function -O3
OBJECTS= km_basic_filter km_diff km_fasta km_merge km_reverse km_select km_unitig

all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

$(OBJECTS): %: %.c
	$(CC) $(CFLAGS) $< -o $@ -lz -lm
