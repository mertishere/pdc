CC=gcc
CFLAGS=-Iinclude
FLAGS=-Wall -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -Wextra # -Wvla
LDFLAGS=-lm

utils = src/utils/constants.c src/utils/conversions.c src/utils/hex.c src/utils/buffer.c src/utils/file.c src/utils/vectors.c src/utils/ppm.c
algorithms = src/algorithms/decompress.c src/algorithms/lzss.c src/algorithms/huffman.c src/algorithms/filter.c src/algorithms/ancillary.c
compute = src/compute/fill.c src/compute/gaussian.c src/compute/neighbours.c src/compute/outline.c src/compute/sad.c

src = src/png.c $(algorithms) $(utils) $(compute)

pulse: main.c $(src)
	mkdir -p dist
	mkdir -p out
	$(CC) $(CFLAGS) $(FLAGS) -o dist/pdc main.c $(src) $(LDFLAGS)
