#include "texture_t.h"

texture_t* load_texture(const char* filepath) {
    int num_channels = 0;
    int width = 0, height = 0;
    texture_t* tex = malloc(sizeof(texture_t));

    uint8_t* p_data = stbi_load(filepath, &width, &height, &num_channels, 3);

    if (p_data) {
        printf("Texture Loaded Successfully!\n");
        printf("Width : %d | Height : %d | Channels : %d\n", width, height, num_channels);
        tex->data = p_data;
        tex->width = width;
        tex->height = height;
        tex->channels = num_channels;
    }
    else {
        printf("Could not load texture!\n");
        tex = NULL;
    }

    return tex;

}

pixel_data sample_color(texture_t* tex, float_t x, float_t y) {
    pixel_data col = {0};

    uint16_t conv_x = x * (float_t)tex->width;
    uint16_t conv_y = y * (float_t)tex->height;

    if (conv_x == tex->width) {
        conv_x--;
    }
    if (conv_y == tex->height) {
        conv_y--;
    }

    if (tex->channels == 3) {
        col.r = tex->data[3 * (conv_x + conv_y * tex->width) + 0];
        col.g = tex->data[3 * (conv_x + conv_y * tex->width) + 1];
        col.b = tex->data[3 * (conv_x + conv_y * tex->width) + 2];
    }
    else {
        printf("Invalid number of channels!\n");
    }

    return col;

}

void free_texture(texture_t* tex) {
    free(tex->data);
    free(tex);
}
