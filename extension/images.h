#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "graphics.h"
#include "vectors.h"
#include "rays.h"


// generate and write an image from a 2d array of pixels
extern void write_image(FILE *fp, voxel **voxels, vector start, tilemap map, view init_view);
