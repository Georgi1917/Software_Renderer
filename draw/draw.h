#ifndef __DRAW_H__
#define __DRAW_H__

#include <stdint.h>
#include <stdbool.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "struct_utils.h"
#include "colors.h"
#include "h_math.h"

typedef struct {
    vec3_f p[3];
    pixel_data color;
} triangle_d;

typedef struct {
    vec3_f pos;
    vec3_f nor;
} vertex_t;

typedef struct {
    vertex_t* vertices;
    uint32_t* indices;

    uint32_t vertex_count;
    uint32_t index_count;
} mesh_t;

typedef struct node {
    triangle_d el;
    struct node* next;
} node;

typedef struct {
    node* head;
    node* tail;
    uint32_t size;
} tr_queue;

tr_queue* init_queue();
void enqueue(tr_queue* q, triangle_d el);
void front(tr_queue* q, triangle_d* tr);
void dequeue(tr_queue* q);
void free_queue(tr_queue* q);

bool put_pixel(int16_t x, int16_t y, screen_buffer* buffer, pixel_data data);
void clear_buffer(pixel_data data, screen_buffer* buffer);

void draw_line_p(vec2_f p0, vec2_f p1, pixel_data p_data, screen_buffer* buffer);
void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, pixel_data p_data, screen_buffer* buffer);
void draw_triangle_wireframe(vec2_f p0, vec2_f p1, vec2_f p2, pixel_data p_data, screen_buffer* buffer);
void draw_triangle_fill(vec2_f p0, vec2_f p1, vec2_f p2, pixel_data p_data, screen_buffer* buffer);

uint8_t triangle_clip_plane(vec3_f plane_p, vec3_f plane_n, triangle_d in_tri, triangle_d* out_tri1, triangle_d* out_tri2);

void draw_cube(screen_buffer* buffer, float_t dt, vec3_f trans, mat4x4_f proj, vec3_f camera_pos, vec3_f* look_dir);
void draw_mesh(screen_buffer* buffer, mesh_t* mesh, float_t dt, vec3_f trans, mat4x4_f proj, vec3_f camera_pos, vec3_f camera_front);

void load_mesh(mesh_t* mesh);
void free_mesh(mesh_t* mesh);


#endif //__DRAW_H__