CC=gcc
CFLAGS=-I include
LDFLAGS=-lm

src = src/decompress.c src/png.c src/lzss.c src/huffman.c src/filter.c
utils = src/utils/constants.c src/utils/conversions.c src/utils/strings.c src/utils/hex.c src/utils/buffer.c src/utils/ints.c src/utils/file.c

pulse: main.c $(src) $(utils)
	mkdir -p dist
	$(CC) $(CFLAGS) -o dist/pdc main.c $(src) $(utils) $(LDFLAGS)