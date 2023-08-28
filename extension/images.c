#include "images.h"
#define PI 3.1415926

void write_image(FILE *fp, voxel **voxels, vector start, tilemap map, view init_view) {
    vector cur_pos = start;
    view view = init_view;

    pixel *pixels = render(voxels, view, cur_pos, map, false);

    int32_t width = view.width;
    int32_t height = view.height;
    uint16_t bitcount = 24; 
    int width_bytes = ((width * bitcount + 31) / 32) * 4;
    uint32_t size_image= width_bytes * height;
    const uint32_t info_header_size = 40;
    const uint32_t num_offset = 54;
    uint32_t size_file = size_image + 54;
    const uint16_t num_planes = 1;

    unsigned char header[54]={ 0 };
    memcpy(header, "BM", 2);
    memcpy(header + 2 , &size_file, 4);
    memcpy(header + 10, &num_offset, 4);
    memcpy(header + 14, &info_header_size, 4);
    memcpy(header + 18, &width, 4);
    memcpy(header + 22, &height, 4);
    memcpy(header + 26, &num_planes, 2);
    memcpy(header + 28, &bitcount, 2);
    memcpy(header + 34, &size_image, 4);

    unsigned char* bitmap_pixels = malloc(size_image);
    for (int vz = 0; vz < view.height; vz ++) {
	for (int vx = 0; vx < view.width; vx ++) {
            pixel contact = pixels[vz * view.width + vx];
	    bitmap_pixels[(view.height-vz-1) * width_bytes + vx * 3 + 0] = contact.b;
	    bitmap_pixels[(view.height-vz-1) * width_bytes + vx * 3 + 1] = contact.g;
	    bitmap_pixels[(view.height-vz-1) * width_bytes + vx * 3 + 2] = contact.r;

	}    
    }
    fwrite(header, 1 , 54, fp);
    fwrite((char*)bitmap_pixels, 1, size_image, fp);
    free(bitmap_pixels);	    
   



}

void main(void){

    char tiles[] = {
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "YYYYYYYYY"
        "CCCCCCCCC"
        "W       C"
        "W       C"
        "W   G   C"
        "W       C"
        "W       C"
        "W       C"
        "W       C"
        "CCCCMCCCC"
        "CCCCCCCCC"
        "W       C"
        "W       C"
        "W   G   C"
        "W       C"
        "W       C"
        "W       C"
        "W       C"
        "CCCCMCCCC"
        "         "
        "         "
        "         "
        "         "
        "         "
        "         "
        "         "
        "         "
        "         "
    };


    int max_x = 9;
    int max_y = 9;
    int max_z = 4;

    tilemap map = {
        .tiles = tiles,
        .max_x = max_x,
        .max_y = max_y,
        .max_z = max_z
    };

    vector start = { .x = 3.5, .y = 6, .z = 3 };

    view viewport = {
        .width = 400,
        .height = 400,
        .x_rotation = 0.3,
        .z_rotation = 0.6
    };


    set_camera_position(0.9 * PI, 0.222 * PI, &viewport);

    
    voxel **voxels = init_voxels();
    FILE *fp = fopen("scene.bmp","wb");
    write_image(fp, voxels, start, map, viewport);
    fclose(fp);


}
