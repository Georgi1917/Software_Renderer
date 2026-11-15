#ifndef __TEXTURE_T__
#define __TEXTURE_T__
#include <STB/stb_image.h>
#include <stdint.h>
#include "h_math.h"
#include "colors.h"

typedef struct {
    uint8_t* data;
    uint16_t width;
    uint16_t height;
    uint8_t channels;
} texture_t;

texture_t* load_texture(const char* filepath);
pixel_data sample_color(texture_t* tex, float_t x, float_t y);
void free_texture(texture_t* tex);

#endif // __TEXTURE_T__
