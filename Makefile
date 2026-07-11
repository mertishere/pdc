CC=gcc
CFLAGS=-Iinclude
FLAGS=-Wall -Werror # -Wextra # -Wvla
LDFLAGS=-lm

src = src/decompress.c src/png.c src/lzss.c src/huffman.c src/filter.c
utils = src/utils/constants.c src/utils/conversions.c src/utils/strings.c src/utils/hex.c src/utils/buffer.c src/utils/ints.c src/utils/file.c src/utils/filter.c

pulse: main.c $(src) $(utils)
	mkdir -p dist
	mkdir -p out
	$(CC) $(CFLAGS) $(FLAGS) -o dist/pdc main.c $(src) $(utils) $(LDFLAGS)
