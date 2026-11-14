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
#include "mesh.h"
#include "queue_t.h"

typedef struct {
    vec3_f p[3];
    pixel_data color;
} triangle_d;

bool put_pixel(int16_t x, int16_t y, screen_buffer* buffer, pixel_data data);
void clear_buffer(pixel_data data, screen_buffer* buffer);

void draw_line_p(vec2_f p0, vec2_f p1, pixel_data p_data, screen_buffer* buffer);
void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, pixel_data p_data, screen_buffer* buffer);
void draw_triangle_wireframe(vec2_f p0, vec2_f p1, vec2_f p2, pixel_data p_data, screen_buffer* buffer);
void draw_triangle_fill(vec2_f p0, vec2_f p1, vec2_f p2, pixel_data p_data, screen_buffer* buffer);
void draw_triangle_depth(vec3_f p0, vec3_f p1, vec3_f p2, pixel_data p_data, screen_buffer* buffer);

uint8_t triangle_clip_plane(vec3_f plane_p, vec3_f plane_n, triangle_d in_tri, triangle_d* out_tri1, triangle_d* out_tri2);

void draw_cube(screen_buffer* buffer, float_t dt, vec3_f trans, mat4x4_f proj, vec3_f camera_pos, vec3_f camera_front);
void draw_mesh(screen_buffer* buffer, mesh_t* mesh, float_t dt, vec3_f trans, mat4x4_f proj, vec3_f camera_pos, vec3_f camera_front, pixel_data color);

#endif //__DRAW_H__