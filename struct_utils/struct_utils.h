#ifndef __STRUCT_UTILS_H__
#define __STRUCT_UTILS_H__

#include <stdint.h>
#include <math.h>
#include "h_math.h"

typedef struct {
    uint8_t* mem;
    float_t* z_buffer;
    uint16_t width;
    uint16_t height;
} screen_buffer;

screen_buffer* init_screen_buffer(uint16_t width, uint16_t height);
void free_screen_buffer(screen_buffer* buff);

typedef struct {
    float_t* data;
    uint16_t size;
    int16_t count;
} list_f;

list_f* init_list();
void append_el(list_f* list, float_t el);
void pop_back(list_f* list);
void free_list(list_f* list);

#endif