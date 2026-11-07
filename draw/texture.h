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
pixel_data sample_color(texture_t* tex, uint16_t x, uint16_t y);
