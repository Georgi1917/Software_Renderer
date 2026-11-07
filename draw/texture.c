#include "texture.h"

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

pixel_data sample_color(texture_t* tex, uint16_t x, uint16_t y) {
    pixel_data col = {0};

    if (tex->channels == 3) {
        col.r = tex->data[3 * (x + y * tex->width) + 0];
        col.g = tex->data[3 * (x + y * tex->width) + 1];
        col.b = tex->data[3 * (x + y * tex->width) + 2];
    }
    else {
        printf("Invalid number of channels!\n");
    }

    return col;

}
