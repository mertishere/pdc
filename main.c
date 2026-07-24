#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>

#include "png.h"
#include "algorithms.h"
#include "utils.h"
#include "constants.h"
#include "compute.h"


int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Path required.\n");
        return 1;
    }

    char *path = argv[1];
    PNG png = parsePng(path);

    writePPM(&png);
    freePng(&png);

    return 0;
}
