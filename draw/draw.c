#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "draw.h"

#define PI 3.1415926535

vec2_i _denormalize_point_coords(vec2_f p, uint16_t max_val_x, uint16_t max_val_y) {
    vec2_i ret_point = {0};

    ret_point.x = ((p.x + 1.0) / 2.0)  * max_val_x;
    ret_point.y = ((p.y - 1.0) / -2.0) * max_val_y;

    return ret_point;
}

list_f* _linear_interpolation(int16_t i0, float_t d0, int16_t i1, float_t d1) {

    list_f* values = init_list();

    if (i0 == i1) {
        append_el(values, d0);
        return values;
    }

    float_t a = (d1 - d0) / (float_t)(i1 - i0);
    float_t d = d0;

    for (int16_t i = i0; i <= i1; i++) {
        append_el(values, d);
        d = d + a;
    }

    return values;

}

void _swap_points(vec2_i* a, vec2_i *b) {

    vec2_i temp = *a;
    *a = *b;
    *b = temp;

}

void _swap_var_f(float_t* a, float_t* b) {
    float_t temp = *a;
    *a = *b;
    *b = temp;
}

void _swap_points_f(vec2_f* a, vec2_f* b) {
    vec2_f temp = *a;
    *a = *b;
    *b = temp;
}

bool put_pixel(int16_t x, int16_t y, screen_buffer* buffer, pixel_data data) {

    if (((x > buffer->width) || (x < 0)) || ((y > buffer->height) || (y < 0))) {
        return false;
    }

    buffer->mem[4 * (x + y * buffer->width) + 0] = data.b;
    buffer->mem[4 * (x + y * buffer->width) + 1] = data.g;
    buffer->mem[4 * (x + y * buffer->width) + 2] = data.r;
    buffer->mem[4 * (x + y * buffer->width) + 3] = 0;

    return true;

}

void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, pixel_data p_data, screen_buffer* buffer) {
    int16_t dx = 0;
    if (x1 > x0) {
        dx = x1 - x0;
    }
    else {
        dx = x0 - x1;
    }

    int16_t dy = 0;
    if (y1 > y0) {
        dy = y1 - y0;
    }
    else {
        dy = y0 - y1;
    }

    int8_t sx = x0 < x1 ? 1 : -1;
    int8_t sy = y0 < y1 ? 1 : -1;

    int16_t err = (dx > dy ? dx : -dy) / 2;
    int16_t e2 = 0;

    while (1) {
        put_pixel(x0, y0, buffer, p_data);
        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = err;

        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }

    }

}

void draw_line_p(vec2_f p0, vec2_f p1, pixel_data p_data, screen_buffer* buffer) {

    vec2_i p0n = _denormalize_point_coords(p0, buffer->width, buffer->height);
    vec2_i p1n = _denormalize_point_coords(p1, buffer->width, buffer->height);

    draw_line(p0n.x, p0n.y, p1n.x, p1n.y, p_data, buffer);
}

void draw_triangle_wireframe(vec2_f p0, vec2_f p1, vec2_f p2, pixel_data p_data, screen_buffer* buffer) {

    draw_line_p(p0, p1, p_data, buffer);
    draw_line_p(p1, p2, p_data, buffer);
    draw_line_p(p0, p2, p_data, buffer);

}

void draw_triangle_fill(vec2_f p0, vec2_f p1, vec2_f p2, pixel_data p_data, screen_buffer* buffer) {
    vec2_i p0n = _denormalize_point_coords(p0, buffer->width, buffer->height);
    vec2_i p1n = _denormalize_point_coords(p1, buffer->width, buffer->height);
    vec2_i p2n = _denormalize_point_coords(p2, buffer->width, buffer->height);

    if (p1n.y < p0n.y) { _swap_points(&p1n, &p0n); }
    if (p2n.y < p0n.y) { _swap_points(&p2n, &p0n); }
    if (p2n.y < p1n.y) { _swap_points(&p2n, &p1n); }

    int16_t dy1 = p1n.y - p0n.y;
    int16_t dx1 = p1n.x - p0n.x;

    int16_t dy2 = p2n.y - p0n.y;
    int16_t dx2 = p2n.x - p0n.x;

    float_t dax_step = 0.0f;
    float_t dbx_step = 0.0f;

    if (dy1) {
        dax_step = dx1 / (float_t)abs(dy1);
    }
    if (dy2) {
        dbx_step = dx2 / (float_t)abs(dy2);
    }

    if (dy1) {

        for (int16_t i = p0n.y; i <= p1n.y; i++) {

            int16_t ax = p0n.x + (float_t)(i - p0n.y) * dax_step;
            int16_t bx = p0n.x + (float_t)(i - p0n.y) * dbx_step;

            if (ax > bx) {
                int16_t x_temp = ax;
                ax = bx;
                bx = x_temp;
            }

            for (int16_t j = ax; j < bx; j++) {
                put_pixel(j, i, buffer, p_data);
            }

        }

    }

    dy1 = p2n.y - p1n.y;
    dx1 = p2n.x - p1n.x;


    if (dy1) {
        dax_step = dx1 / (float_t)abs(dy1);
    }
    if (dy2) {
        dbx_step = dx2 / (float_t)abs(dy2);
    }

    if (dy1) {

        for (int16_t i = p1n.y; i <= p2n.y; i++) {
            int16_t ax = p1n.x + (float_t)(i - p1n.y) * dax_step;
            int16_t bx = p0n.x + (float_t)(i - p0n.y) * dbx_step;

            if (ax > bx) {
                int16_t temp = ax;
                ax = bx;
                bx = temp;
            }

            for (int16_t j = ax; j < bx; j++) {
                put_pixel(j, i, buffer, p_data);
            }

        }

    }

}

void draw_triangle_depth(vec3_f p0, vec3_f p1, vec3_f p2, pixel_data p_data, screen_buffer* buffer) {

    vec2_i p0n = _denormalize_point_coords((vec2_f){p0.x, p0.y}, buffer->width, buffer->height);
    vec2_i p1n = _denormalize_point_coords((vec2_f){p1.x, p1.y}, buffer->width, buffer->height);
    vec2_i p2n = _denormalize_point_coords((vec2_f){p2.x, p2.y}, buffer->width, buffer->height);

    if (p1n.y < p0n.y) { 
        _swap_points(&p1n, &p0n);
        float_t tmp = p1.z; p1.z = p0.z; p0.z = tmp; 
    }
    if (p2n.y < p0n.y) { 
        _swap_points(&p2n, &p0n);
        float_t tmp = p2.z; p2.z = p0.z; p0.z = tmp; 
    }
    if (p2n.y < p1n.y) { 
        _swap_points(&p2n, &p1n);
        float_t tmp = p2.z; p2.z = p1.z; p1.z = tmp; 
    }

    float_t z = 0.0f;

    int16_t dy1 = p1n.y - p0n.y;
    int16_t dx1 = p1n.x - p0n.x;
    float_t dz1 = p1.z  - p0.z;

    int16_t dy2 = p2n.y - p0n.y;
    int16_t dx2 = p2n.x - p0n.x;
    float_t dz2 = p2.z  - p0.z;

    float_t dax_step = 0.0f;
    float_t dbx_step = 0.0f;
    float_t dz1_step = 0.0f;
    float_t dz2_step = 0.0f;

    if (dy1) {
        dax_step = dx1 / (float_t)abs(dy1);
        dz1_step = dz1 / (float_t)abs(dy1);
    }
    if (dy2) {
        dbx_step = dx2 / (float_t)abs(dy2);
        dz2_step = dz2 / (float_t)abs(dy2);
    }

    if (dy1) {

        for (int16_t i = p0n.y; i <= p1n.y; i++) {

            int16_t ax = p0n.x + (float_t)(i - p0n.y) * dax_step;
            int16_t bx = p0n.x + (float_t)(i - p0n.y) * dbx_step;

            float_t start_z = p0.z + (float_t)(i - p0n.y) * dz1_step;
            float_t end_z = p0.z + (float_t)(i - p0n.y) * dz2_step;

            if (ax > bx) {
                int16_t x_temp = ax;
                float_t z_temp = start_z;
                ax = bx;
                bx = x_temp;

                start_z = end_z;
                end_z = z_temp;
            }

            z = start_z;

            float_t t_step = 1.0f / ((float_t)(bx - ax));
            float_t t = 0.0f;

            for (int16_t j = ax; j < bx; j++) {

                z = (1.0f - t) * start_z + t * end_z;

                if (z < buffer->z_buffer[j + i * buffer->width]) {
                    buffer->z_buffer[j + i * buffer->width] = z;
                    put_pixel(j, i, buffer, p_data);
                }

                t += t_step;
            }

        }

    }

    dy1 = p2n.y - p1n.y;
    dx1 = p2n.x - p1n.x;
    dz1 = p2.z  - p1.z;

    dz1_step = 0.0f;

    if (dy1) {
        dax_step = dx1 / (float_t)abs(dy1);
        dz1_step = dz1 / (float_t)abs(dy1);
    }
    if (dy2) {
        dbx_step = dx2 / (float_t)abs(dy2);
    }

    if (dy1) {

        for (int16_t i = p1n.y; i <= p2n.y; i++) {
            int16_t ax = p1n.x + (float_t)(i - p1n.y) * dax_step;
            int16_t bx = p0n.x + (float_t)(i - p0n.y) * dbx_step;

            float_t start_z = p1.z + (float_t)(i - p1n.y) * dz1_step;
            float_t end_z = p0.z + (float_t)(i - p0n.y) * dz2_step;

            if (ax > bx) {
                int16_t temp = ax;
                float_t z_temp = start_z;
                ax = bx;
                bx = temp;
                
                start_z = end_z;
                end_z = z_temp;
            }

            z = start_z;

            float_t t_step = 1.0f / ((float_t)(bx - ax));
            float_t t = 0.0f;

            for (int16_t j = ax; j < bx; j++) {

                z = (1.0f - t) * start_z + t * end_z;

                if (z < buffer->z_buffer[j + i * buffer->width]) {
                    buffer->z_buffer[j + i * buffer->width] = z;
                    put_pixel(j, i, buffer, p_data);
                }

                t += t_step;
            }

        }

    }

}

void draw_triangle_tex(vec3_f p0, vec3_f p1, vec3_f p2,
                       vec2_f uv0, vec2_f uv1, vec2_f uv2, 
                       screen_buffer* buffer, texture_t* tex)
{
    vec2_i p0n = _denormalize_point_coords((vec2_f){p0.x, p0.y}, buffer->width, buffer->height);
    vec2_i p1n = _denormalize_point_coords((vec2_f){p1.x, p1.y}, buffer->width, buffer->height);
    vec2_i p2n = _denormalize_point_coords((vec2_f){p2.x, p2.y}, buffer->width, buffer->height);

    if (p1n.y < p0n.y) { 
        _swap_points(&p1n, &p0n);
        _swap_points_f(&uv1, &uv0);
        float_t tmp = p1.z; p1.z = p0.z; p0.z = tmp; 
    }
    if (p2n.y < p0n.y) { 
        _swap_points(&p2n, &p0n);
        _swap_points_f(&uv2, &uv0);
        float_t tmp = p2.z; p2.z = p0.z; p0.z = tmp; 
    }
    if (p2n.y < p1n.y) { 
        _swap_points(&p2n, &p1n);
        _swap_points_f(&uv2, &uv1);
        float_t tmp = p2.z; p2.z = p1.z; p1.z = tmp; 
    }

    float_t z = 0.0f;

    int16_t dy1 = p1n.y - p0n.y;
    int16_t dx1 = p1n.x - p0n.x;
    float_t dz1 = p1.z  - p0.z;
    float_t du1 = uv1.x - uv0.x;
    float_t dv1 = uv1.y - uv0.y;

    int16_t dy2 = p2n.y - p0n.y;
    int16_t dx2 = p2n.x - p0n.x;
    float_t dz2 = p2.z  - p0.z;
    float_t du2 = uv2.x - uv0.x;
    float_t dv2 = uv2.y - uv0.y;

    float_t tex_u = 0.0f;
    float_t tex_v = 0.0f;

    float_t dax_step = 0.0f, dbx_step = 0.0f;
    float_t dz1_step = 0.0f, dz2_step = 0.0f;
    float_t du1_step = 0.0f, dv1_step = 0.0f;
    float_t du2_step = 0.0f, dv2_step = 0.0f;

    if (dy1) {
        dax_step = dx1 / (float_t)abs(dy1);
        dz1_step = dz1 / (float_t)abs(dy1);
        du1_step = du1 / (float_t)abs(dy1);
        dv1_step = dv1 / (float_t)abs(dy1);
    }
    if (dy2) {
        dbx_step = dx2 / (float_t)abs(dy2);
        dz2_step = dz2 / (float_t)abs(dy2);
        du2_step = du2 / (float_t)abs(dy2);
        dv2_step = dv2 / (float_t)abs(dy2);
    }

    if (dy1) {

        for (int16_t i = p0n.y; i <= p1n.y; i++) {

            int16_t ax = p0n.x + (float_t)(i - p0n.y) * dax_step;
            int16_t bx = p0n.x + (float_t)(i - p0n.y) * dbx_step;

            float_t start_z = p0.z + (float_t)(i - p0n.y) * dz1_step;
            float_t tex_su = uv0.x + (float_t)(i - p0n.y) * du1_step;
            float_t tex_sv = uv0.y + (float_t)(i - p0n.y) * dv1_step;

            float_t end_z = p0.z + (float_t)(i - p0n.y) * dz2_step;
            float_t tex_eu = uv0.x + (float_t)(i - p0n.y) * du2_step;
            float_t tex_ev = uv0.y + (float_t)(i - p0n.y) * dv2_step;

            if (ax > bx) {
                int16_t x_temp = ax;
                float_t z_temp = start_z;
                ax = bx;
                bx = x_temp;

                start_z = end_z;
                end_z = z_temp;

                _swap_var_f(&tex_su, &tex_eu);
                _swap_var_f(&tex_sv, &tex_ev);
            }

            z = start_z;
            tex_u = tex_su;
            tex_v = tex_sv;

            float_t t_step = 1.0f / ((float_t)(bx - ax));
            float_t t = 0.0f;

            for (int16_t j = ax; j < bx; j++) {

                z = (1.0f - t) * start_z + t * end_z;
                tex_u = (1.0f - t) * tex_su + t * tex_eu;
                tex_v = (1.0f - t) * tex_sv + t * tex_ev;

                if (z < buffer->z_buffer[j + i * buffer->width]) {
                    buffer->z_buffer[j + i * buffer->width] = z;
                    put_pixel(j, i, buffer, sample_color(tex, tex_u, tex_v));
                }

                t += t_step;
            }

        }

    }

    dy1 = p2n.y - p1n.y;
    dx1 = p2n.x - p1n.x;
    dz1 = p2.z  - p1.z;
    du1 = uv2.x - uv1.x;
    dv1 = uv2.y - uv1.y;

    dz1_step = 0.0f;
    du1_step = 0.0f;
    dv1_step = 0.0f;

    if (dy1) {
        dax_step = dx1 / (float_t)abs(dy1);
        dz1_step = dz1 / (float_t)abs(dy1);
        du1_step = du1 / (float_t)abs(dy1);
        dv1_step = dv1 / (float_t)abs(dy1);
    }
    if (dy2) {
        dbx_step = dx2 / (float_t)abs(dy2);
    }

    if (dy1) {

        for (int16_t i = p1n.y; i <= p2n.y; i++) {
            int16_t ax = p1n.x + (float_t)(i - p1n.y) * dax_step;
            int16_t bx = p0n.x + (float_t)(i - p0n.y) * dbx_step;

            float_t start_z = p1.z + (float_t)(i - p1n.y) * dz1_step;
            float_t tex_su = uv1.x + (float_t)(i - p1n.y) * du1_step;
            float_t tex_sv = uv1.y + (float_t)(i - p1n.y) * dv1_step;

            float_t end_z = p0.z + (float_t)(i - p0n.y) * dz2_step;
            float_t tex_eu = uv0.x + (float_t)(i - p0n.y) * du2_step;
            float_t tex_ev = uv0.y + (float_t)(i - p0n.y) * dv2_step;

            if (ax > bx) {
                int16_t temp = ax;
                float_t z_temp = start_z;
                ax = bx;
                bx = temp;
                
                start_z = end_z;
                end_z = z_temp;

                _swap_var_f(&tex_su, &tex_eu);
                _swap_var_f(&tex_sv, &tex_ev);
            }

            z = start_z;
            tex_u = tex_su;
            tex_v = tex_sv;

            float_t t_step = 1.0f / ((float_t)(bx - ax));
            float_t t = 0.0f;

            for (int16_t j = ax; j < bx; j++) {

                z = (1.0f - t) * start_z + t * end_z;
                tex_u = (1.0f - t) * tex_su + t * tex_eu;
                tex_v = (1.0f - t) * tex_sv + t * tex_ev;

                if (z < buffer->z_buffer[j + i * buffer->width]) {
                    buffer->z_buffer[j + i * buffer->width] = z;
                    put_pixel(j, i, buffer, sample_color(tex, tex_u, tex_v));
                }

                t += t_step;
            }

        }

    }

}

uint8_t triangle_clip_plane(vec3_f plane_p, vec3_f plane_n, triangle_d in_tri, triangle_d* out_tri1, triangle_d* out_tri2) {
    plane_n = normalize(plane_n);

    vec3_f* inside_points[3] ; uint8_t inside_point_count = 0;
    vec2_f* inside_points_tex[3] ; uint8_t inside_point_count_tex = 0;
    vec3_f* outside_points[3] ; uint8_t outside_point_count = 0;
    vec2_f* outside_points_tex[3] ; uint8_t outside_point_count_tex = 0;

    if (dist_point_plane(in_tri.p[0], plane_p, plane_n) >= 0.0f) {
        inside_points[inside_point_count++] = &in_tri.p[0];
        inside_points_tex[inside_point_count_tex++] = &in_tri.t[0];
    } else { 
        outside_points[outside_point_count++] = &in_tri.p[0];
        outside_points_tex[outside_point_count_tex++] = &in_tri.t[0];
    }
    if (dist_point_plane(in_tri.p[1], plane_p, plane_n) >= 0.0f) {
        inside_points[inside_point_count++] = &in_tri.p[1];
        inside_points_tex[inside_point_count_tex++] = &in_tri.t[1];
    } else { 
        outside_points[outside_point_count++] = &in_tri.p[1];
        outside_points_tex[outside_point_count_tex++] = &in_tri.t[1];
    }
    if (dist_point_plane(in_tri.p[2], plane_p, plane_n) >= 0.0f) {
        inside_points[inside_point_count++] = &in_tri.p[2];
        inside_points_tex[inside_point_count_tex++] = &in_tri.t[2];
    } else { 
        outside_points[outside_point_count++] = &in_tri.p[2];
        outside_points_tex[outside_point_count_tex++] = &in_tri.t[2];
    }

    if (inside_point_count == 0) {
        return 0;
    }

    if (inside_point_count == 3) {
        *out_tri1 = in_tri;
        return 1;
    }

    float_t t = 0.0f;

    if (inside_point_count == 1 && outside_point_count == 2) {

        out_tri1->color = in_tri.color;

        out_tri1->p[0] = *inside_points[0];
        out_tri1->t[0] = *inside_points_tex[0];

        out_tri1->p[1]   = vector_intersect_plane(plane_p, plane_n, *inside_points[0], *outside_points[0], &t);
        out_tri1->t[1].x = inside_points_tex[0]->x + t * (outside_points_tex[0]->x - inside_points_tex[0]->x);
        out_tri1->t[1].y = inside_points_tex[0]->y + t * (outside_points_tex[0]->y - inside_points_tex[0]->y);

        out_tri1->p[2] = vector_intersect_plane(plane_p, plane_n, *inside_points[0], *outside_points[1], &t);
        out_tri1->t[2].x = inside_points_tex[0]->x + t * (outside_points_tex[1]->x - inside_points_tex[0]->x);
        out_tri1->t[2].y = inside_points_tex[0]->y + t * (outside_points_tex[1]->y - inside_points_tex[0]->y);

        return 1;
    }

    if (inside_point_count == 2 && outside_point_count == 1) {

        out_tri1->color = in_tri.color;
        out_tri2->color = in_tri.color;

        out_tri1->p[0] = *inside_points[0];
        out_tri1->p[1] = *inside_points[1];
        out_tri1->t[0] = *inside_points_tex[0];
        out_tri1->t[1] = *inside_points_tex[1];
        out_tri1->p[2] = vector_intersect_plane(plane_p, plane_n, *inside_points[1], *outside_points[0], &t);
        out_tri1->t[2].x = inside_points_tex[1]->x + t * (outside_points_tex[0]->x - inside_points_tex[1]->x);
        out_tri1->t[2].y = inside_points_tex[1]->y + t * (outside_points_tex[0]->y - inside_points_tex[1]->y);

        out_tri2->p[0] = *inside_points[0];
        out_tri2->t[0] = *inside_points_tex[0];
        out_tri2->p[1] = out_tri1->p[2];
        out_tri2->t[1] = out_tri1->t[2];
        out_tri2->p[2] = vector_intersect_plane(plane_p, plane_n, *inside_points[0], *outside_points[0], &t);
        out_tri1->t[2].x = inside_points_tex[0]->x + t * (outside_points_tex[0]->x - inside_points_tex[0]->x);
        out_tri1->t[2].y = inside_points_tex[0]->y + t * (outside_points_tex[0]->y - inside_points_tex[0]->y);

        return 2;
    }

    return 0;
}

void clear_buffer(pixel_data data, screen_buffer* buffer) {
    for (uint16_t y = 0; y < buffer->height; y++) {
        for (uint16_t x = 0; x < buffer->width; x++) {
            put_pixel(x, y, buffer, data);
            buffer->z_buffer[x + y * buffer->width] = 1000.0f;
        }
    }

}

// ########################################################################################################################## //

void draw_cube(screen_buffer* buffer, float_t dt, vec3_f trans, mat4x4_f proj, vec3_f camera_pos, vec3_f camera_front) {
    
    static float_t angle = 0.0;
    triangle_d tri[12];

    // Front
    tri[0].p[0] = (vec3_f){-1.0,  1.0,  1.0};
    tri[0].p[1] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[0].p[2] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[0].color = BLUE;

    tri[1].p[0] = (vec3_f){-1.0,  1.0,  1.0};
    tri[1].p[1] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[1].p[2] = (vec3_f){-1.0, -1.0,  1.0};
    tri[1].color = BLUE;

    /* Back (-Z) */
    tri[2].p[0] = (vec3_f){ 1.0,  1.0, -1.0};
    tri[2].p[1] = (vec3_f){-1.0,  1.0, -1.0};
    tri[2].p[2] = (vec3_f){-1.0, -1.0, -1.0};
    tri[2].color = RED;

    tri[3].p[0] = (vec3_f){ 1.0,  1.0, -1.0};
    tri[3].p[1] = (vec3_f){-1.0, -1.0, -1.0};
    tri[3].p[2] = (vec3_f){ 1.0, -1.0, -1.0};
    tri[3].color = RED;

    /* Top (+Y) */
    tri[4].p[0] = (vec3_f){-1.0,  1.0, -1.0};
    tri[4].p[1] = (vec3_f){ 1.0,  1.0, -1.0};
    tri[4].p[2] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[4].color = PURPLE;

    tri[5].p[0] = (vec3_f){-1.0,  1.0, -1.0};
    tri[5].p[1] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[5].p[2] = (vec3_f){-1.0,  1.0,  1.0};
    tri[5].color = PURPLE;

    /* Bottom (-Y) */
    tri[6].p[0] = (vec3_f){-1.0, -1.0, -1.0};
    tri[6].p[1] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[6].p[2] = (vec3_f){ 1.0, -1.0, -1.0};
    tri[6].color = WHITE;

    tri[7].p[0] = (vec3_f){-1.0, -1.0, -1.0};
    tri[7].p[1] = (vec3_f){-1.0, -1.0,  1.0};
    tri[7].p[2] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[7].color = WHITE;

    /* Right (+X) */
    tri[8].p[0] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[8].p[1] = (vec3_f){ 1.0,  1.0, -1.0};
    tri[8].p[2] = (vec3_f){ 1.0, -1.0, -1.0};
    tri[8].color = GOLD;

    tri[9].p[0] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[9].p[1] = (vec3_f){ 1.0, -1.0, -1.0};
    tri[9].p[2] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[9].color = GOLD;

    /* Left (-X) */
    tri[10].p[0] = (vec3_f){-1.0,  1.0, -1.0};
    tri[10].p[1] = (vec3_f){-1.0,  1.0,  1.0};
    tri[10].p[2] = (vec3_f){-1.0, -1.0,  1.0};
    tri[10].color = GREEN;

    tri[11].p[0] = (vec3_f){-1.0,  1.0, -1.0};
    tri[11].p[1] = (vec3_f){-1.0, -1.0,  1.0};
    tri[11].p[2] = (vec3_f){-1.0, -1.0, -1.0};
    tri[11].color = GREEN;

    vec3_f up = {0.0f, 1.0f, 0.0f};

    mat4x4_f view_mat = look_at(camera_pos, add_vectors(camera_pos, camera_front), up);

    mat4x4_f trans_mat = matrix_translation(trans.x, trans.y, trans.z);
    mat4x4_f rot_mat_y = matrix_rotation_y(convert_to_radians(angle));
    mat4x4_f world_mat = multiply_mat4_mat4(rot_mat_y, trans_mat);

    triangle_d tri_to_raster[512] = {0.0f};
    uint16_t tri_count = 0;

    for (int i = 0; i < 12; i++) {
        vec3_f a = tri[i].p[0];
        vec3_f b = tri[i].p[1];
        vec3_f c = tri[i].p[2];

        vec3_f trans_point_a = to_cartesian_coords(multiply_vec4_mat4
                                        ((vec4_f){a.x, a.y, a.z, 1.0f}, world_mat));

        vec3_f trans_point_b = to_cartesian_coords(multiply_vec4_mat4
                                        ((vec4_f){b.x, b.y, b.z, 1.0f}, world_mat));
                                        
        vec3_f trans_point_c = to_cartesian_coords(multiply_vec4_mat4
                                        ((vec4_f){c.x, c.y, c.z, 1.0f}, world_mat));

        vec3_f normal = calculate_normal(trans_point_a, trans_point_b, trans_point_c);

        float_t cam_dot = calculate_dot_product(normalize(sub_vectors(trans_point_a, camera_pos)), normal);

        if (cam_dot > 0.0f) {

            vec3_f light_pos = {0.0f, 4.0f, -1.0f};
            vec3_f light_vec = sub_vectors(trans_point_a, light_pos);
            light_vec = normalize(light_vec);
            float_t dot = calculate_dot_product(light_vec, normal);

            if (dot > 1.0f) dot = 1.0f;
            if (dot < 0.0f) dot = 0.0f;

            vec3_f view_point_a = to_cartesian_coords(multiply_vec4_mat4
                                            ((vec4_f){trans_point_a.x, trans_point_a.y, trans_point_a.z, 1.0f}, view_mat));

            vec3_f view_point_b = to_cartesian_coords(multiply_vec4_mat4
                                            ((vec4_f){trans_point_b.x, trans_point_b.y, trans_point_b.z, 1.0f}, view_mat));
                                            
            vec3_f view_point_c = to_cartesian_coords(multiply_vec4_mat4
                                            ((vec4_f){trans_point_c.x, trans_point_c.y, trans_point_c.z, 1.0f}, view_mat));

            triangle_d to_clip = {0.0f};
            to_clip.p[0] = view_point_a;
            to_clip.p[1] = view_point_b;
            to_clip.p[2] = view_point_c;
            to_clip.color = tri[i].color;

            triangle_d clipped[2];
            uint8_t num_clipped = 0;

            num_clipped = triangle_clip_plane((vec3_f){0.0f, 0.0f, 1.0f}, (vec3_f){0.0f, 0.0f, 1.0f}, to_clip, &clipped[0], &clipped[1]);

            for (uint8_t i = 0; i < num_clipped; i++) {

                vec3_f proj_point_a = to_cartesian_coords(multiply_vec4_mat4
                                                ((vec4_f){clipped[i].p[0].x, clipped[i].p[0].y, clipped[i].p[0].z, 1.0f}, proj));
                vec3_f proj_point_b = to_cartesian_coords(multiply_vec4_mat4
                                                ((vec4_f){clipped[i].p[1].x, clipped[i].p[1].y, clipped[i].p[1].z, 1.0f}, proj));
                vec3_f proj_point_c = to_cartesian_coords(multiply_vec4_mat4
                                                ((vec4_f){clipped[i].p[2].x, clipped[i].p[2].y, clipped[i].p[2].z, 1.0f}, proj));

                triangle_d new = {0.0f};
                new.p[0] = proj_point_a;
                new.p[1] = proj_point_b;
                new.p[2] = proj_point_c;
                new.color = to_clip.color;

                tri_to_raster[tri_count++] = new;
            }
        }

    }

    for (uint16_t i = 0; i < tri_count; i++) {
        triangle_d tri = tri_to_raster[i];

        triangle_d clipped[2];
        queue_t* q = queue_init(sizeof(triangle_d));
        enqueue(q, &tri);

        int32_t new_tri = 1;

        for (uint8_t p = 0; p < 4; p++) {
            uint8_t tris_to_add = 0;
            if (tris_to_add != 0) {
                printf("To add : %d\n", tris_to_add);
            }

            while (new_tri > 0) {
                triangle_d test = {0.0f};
                queue_front(q, &test);
                dequeue(q);

                new_tri--;

                switch(p) {
                    case 0: tris_to_add = triangle_clip_plane((vec3_f){0.0f, -1.0f, 0.0f}, (vec3_f){0.0f, 1.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 1: tris_to_add = triangle_clip_plane((vec3_f){0.0f, 1.0f, 0.0f}, (vec3_f){0.0f, -1.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 2: tris_to_add = triangle_clip_plane((vec3_f){-1.0f, 0.0f, 0.0f}, (vec3_f){1.0f, 0.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 3: tris_to_add = triangle_clip_plane((vec3_f){1.0f, 0.0f, 0.0f}, (vec3_f){-1.0f, 0.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                }

                for (uint16_t w = 0; w < tris_to_add; w++) {
                    enqueue(q, &clipped[w]);
                }
            }
            new_tri = q->size;
        }

        for (node_t* curr = q->head; curr != NULL; curr = curr->next) {
            triangle_d t = *(triangle_d*)curr->data;
            draw_triangle_depth(t.p[0], t.p[1], t.p[2], t.color, buffer);
            // draw_triangle_fill((vec2_f){t.p[0].x, t.p[0].y}, 
            //                    (vec2_f){t.p[1].x, t.p[1].y}, 
            //                    (vec2_f){t.p[2].x, t.p[2].y}, t.color, buffer);
        }

        queue_free(q);

    }

    //angle += 10.0f * dt;

}

void draw_cube_tex(screen_buffer* buffer, texture_t* tex, vec3_f trans, mat4x4_f proj, vec3_f camera_pos, vec3_f camera_front) {
    static float_t angle = 0.0;
    triangle_d tri[12];

    // Front
    tri[0].p[0] = (vec3_f){-1.0,  1.0,  1.0};
    tri[0].p[1] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[0].p[2] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[0].color = BLUE;

    tri[1].p[0] = (vec3_f){-1.0,  1.0,  1.0};
    tri[1].p[1] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[1].p[2] = (vec3_f){-1.0, -1.0,  1.0};
    tri[1].color = BLUE;

    tri[0].t[0] = (vec2_f){0.0, 1.0};
    tri[0].t[1] = (vec2_f){1.0, 1.0};
    tri[0].t[2] = (vec2_f){1.0, 0.0};

    tri[1].t[0] = (vec2_f){0.0, 1.0};
    tri[1].t[1] = (vec2_f){1.0, 0.0};
    tri[1].t[2] = (vec2_f){0.0, 0.0};

    /* Back (-Z) */
    tri[2].p[0] = (vec3_f){ 1.0,  1.0, -1.0};
    tri[2].p[1] = (vec3_f){-1.0,  1.0, -1.0};
    tri[2].p[2] = (vec3_f){-1.0, -1.0, -1.0};
    tri[2].color = RED;

    tri[3].p[0] = (vec3_f){ 1.0,  1.0, -1.0};
    tri[3].p[1] = (vec3_f){-1.0, -1.0, -1.0};
    tri[3].p[2] = (vec3_f){ 1.0, -1.0, -1.0};
    tri[3].color = RED;

    tri[2].t[0] = (vec2_f){1.0, 1.0};
    tri[2].t[1] = (vec2_f){0.0, 1.0};
    tri[2].t[2] = (vec2_f){0.0, 0.0};

    tri[3].t[0] = (vec2_f){1.0, 1.0};
    tri[3].t[1] = (vec2_f){0.0, 0.0};
    tri[3].t[2] = (vec2_f){1.0, 0.0};

    /* Top (+Y) */
    tri[4].p[0] = (vec3_f){-1.0,  1.0, -1.0};
    tri[4].p[1] = (vec3_f){ 1.0,  1.0, -1.0};
    tri[4].p[2] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[4].color = PURPLE;

    tri[5].p[0] = (vec3_f){-1.0,  1.0, -1.0};
    tri[5].p[1] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[5].p[2] = (vec3_f){-1.0,  1.0,  1.0};
    tri[5].color = PURPLE;

    tri[4].t[0] = (vec2_f){0.0, 0.0};
    tri[4].t[1] = (vec2_f){1.0, 0.0};
    tri[4].t[2] = (vec2_f){1.0, 1.0};

    tri[5].t[0] = (vec2_f){0.0, 0.0};
    tri[5].t[1] = (vec2_f){1.0, 1.0};
    tri[5].t[2] = (vec2_f){0.0, 1.0};

    /* Bottom (-Y) */
    tri[6].p[0] = (vec3_f){-1.0, -1.0, -1.0};
    tri[6].p[1] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[6].p[2] = (vec3_f){ 1.0, -1.0, -1.0};
    tri[6].color = WHITE;

    tri[7].p[0] = (vec3_f){-1.0, -1.0, -1.0};
    tri[7].p[1] = (vec3_f){-1.0, -1.0,  1.0};
    tri[7].p[2] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[7].color = WHITE;

    tri[6].t[0] = (vec2_f){0.0, 0.0};
    tri[6].t[1] = (vec2_f){1.0, 1.0};
    tri[6].t[2] = (vec2_f){1.0, 0.0};

    tri[7].t[0] = (vec2_f){0.0, 0.0};
    tri[7].t[1] = (vec2_f){0.0, 1.0};
    tri[7].t[2] = (vec2_f){1.0, 1.0};

    /* Right (+X) */
    tri[8].p[0] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[8].p[1] = (vec3_f){ 1.0,  1.0, -1.0};
    tri[8].p[2] = (vec3_f){ 1.0, -1.0, -1.0};
    tri[8].color = GOLD;

    tri[9].p[0] = (vec3_f){ 1.0,  1.0,  1.0};
    tri[9].p[1] = (vec3_f){ 1.0, -1.0, -1.0};
    tri[9].p[2] = (vec3_f){ 1.0, -1.0,  1.0};
    tri[9].color = GOLD;

    tri[8].t[0] = (vec2_f){0.0, 1.0};
    tri[8].t[1] = (vec2_f){1.0, 1.0};
    tri[8].t[2] = (vec2_f){1.0, 0.0};

    tri[9].t[0] = (vec2_f){0.0, 1.0};
    tri[9].t[1] = (vec2_f){1.0, 0.0};
    tri[9].t[2] = (vec2_f){0.0, 0.0};

    /* Left (-X) */
    tri[10].p[0] = (vec3_f){-1.0,  1.0, -1.0};
    tri[10].p[1] = (vec3_f){-1.0,  1.0,  1.0};
    tri[10].p[2] = (vec3_f){-1.0, -1.0,  1.0};
    tri[10].color = GREEN;

    tri[11].p[0] = (vec3_f){-1.0,  1.0, -1.0};
    tri[11].p[1] = (vec3_f){-1.0, -1.0,  1.0};
    tri[11].p[2] = (vec3_f){-1.0, -1.0, -1.0};
    tri[11].color = GREEN;

    tri[10].t[0] = (vec2_f){1.0, 1.0};
    tri[10].t[1] = (vec2_f){0.0, 1.0};
    tri[10].t[2] = (vec2_f){0.0, 0.0};

    tri[11].t[0] = (vec2_f){1.0, 1.0};
    tri[11].t[1] = (vec2_f){0.0, 0.0};
    tri[11].t[2] = (vec2_f){1.0, 0.0};

    vec3_f up = {0.0f, 1.0f, 0.0f};

    mat4x4_f view_mat = look_at(camera_pos, add_vectors(camera_pos, camera_front), up);

    mat4x4_f trans_mat = matrix_translation(trans.x, trans.y, trans.z);
    mat4x4_f rot_mat_y = matrix_rotation_y(convert_to_radians(angle));
    mat4x4_f world_mat = multiply_mat4_mat4(rot_mat_y, trans_mat);

    triangle_d tri_to_raster[512] = {0.0f};
    uint16_t tri_count = 0;

    for (int i = 0; i < 12; i++) {
        vec3_f a = tri[i].p[0];
        vec3_f b = tri[i].p[1];
        vec3_f c = tri[i].p[2];

        vec3_f trans_point_a = to_cartesian_coords(multiply_vec4_mat4
                                        ((vec4_f){a.x, a.y, a.z, 1.0f}, world_mat));

        vec3_f trans_point_b = to_cartesian_coords(multiply_vec4_mat4
                                        ((vec4_f){b.x, b.y, b.z, 1.0f}, world_mat));
                                        
        vec3_f trans_point_c = to_cartesian_coords(multiply_vec4_mat4
                                        ((vec4_f){c.x, c.y, c.z, 1.0f}, world_mat));

        vec3_f normal = calculate_normal(trans_point_a, trans_point_b, trans_point_c);

        float_t cam_dot = calculate_dot_product(normalize(sub_vectors(trans_point_a, camera_pos)), normal);

        if (cam_dot > 0.0f) {

            vec3_f light_pos = {0.0f, 4.0f, -1.0f};
            vec3_f light_vec = sub_vectors(trans_point_a, light_pos);
            light_vec = normalize(light_vec);
            float_t dot = calculate_dot_product(light_vec, normal);

            if (dot > 1.0f) dot = 1.0f;
            if (dot < 0.0f) dot = 0.0f;

            vec3_f view_point_a = to_cartesian_coords(multiply_vec4_mat4
                                            ((vec4_f){trans_point_a.x, trans_point_a.y, trans_point_a.z, 1.0f}, view_mat));

            vec3_f view_point_b = to_cartesian_coords(multiply_vec4_mat4
                                            ((vec4_f){trans_point_b.x, trans_point_b.y, trans_point_b.z, 1.0f}, view_mat));
                                            
            vec3_f view_point_c = to_cartesian_coords(multiply_vec4_mat4
                                            ((vec4_f){trans_point_c.x, trans_point_c.y, trans_point_c.z, 1.0f}, view_mat));

            triangle_d to_clip = {0.0f};
            to_clip.p[0] = view_point_a;
            to_clip.p[1] = view_point_b;
            to_clip.p[2] = view_point_c;
            to_clip.t[0] = tri[i].t[0];
            to_clip.t[1] = tri[i].t[1];
            to_clip.t[2] = tri[i].t[2];
            to_clip.color = tri[i].color;

            triangle_d clipped[2];
            uint8_t num_clipped = 0;

            num_clipped = triangle_clip_plane((vec3_f){0.0f, 0.0f, 1.0f}, (vec3_f){0.0f, 0.0f, 1.0f}, to_clip, &clipped[0], &clipped[1]);

            for (uint8_t i = 0; i < num_clipped; i++) {

                vec3_f proj_point_a = to_cartesian_coords(multiply_vec4_mat4
                                                ((vec4_f){clipped[i].p[0].x, clipped[i].p[0].y, clipped[i].p[0].z, 1.0f}, proj));
                vec3_f proj_point_b = to_cartesian_coords(multiply_vec4_mat4
                                                ((vec4_f){clipped[i].p[1].x, clipped[i].p[1].y, clipped[i].p[1].z, 1.0f}, proj));
                vec3_f proj_point_c = to_cartesian_coords(multiply_vec4_mat4
                                                ((vec4_f){clipped[i].p[2].x, clipped[i].p[2].y, clipped[i].p[2].z, 1.0f}, proj));

                triangle_d new = {0.0f};
                new.p[0] = proj_point_a;
                new.p[1] = proj_point_b;
                new.p[2] = proj_point_c;
                new.t[0] = clipped[i].t[0];
                new.t[1] = clipped[i].t[1];
                new.t[2] = clipped[i].t[2];
                new.color = to_clip.color;

                tri_to_raster[tri_count++] = new;
            }
        }

    }

    for (uint16_t i = 0; i < tri_count; i++) {
        triangle_d tri = tri_to_raster[i];

        triangle_d clipped[2];
        queue_t* q = queue_init(sizeof(triangle_d));
        enqueue(q, &tri);

        int32_t new_tri = 1;

        for (uint8_t p = 0; p < 4; p++) {
            uint8_t tris_to_add = 0;
            if (tris_to_add != 0) {
                printf("To add : %d\n", tris_to_add);
            }

            while (new_tri > 0) {
                triangle_d test = {0.0f};
                queue_front(q, &test);
                dequeue(q);

                new_tri--;

                switch(p) {
                    case 0: tris_to_add = triangle_clip_plane((vec3_f){0.0f, -1.0f, 0.0f}, (vec3_f){0.0f, 1.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 1: tris_to_add = triangle_clip_plane((vec3_f){0.0f, 1.0f, 0.0f}, (vec3_f){0.0f, -1.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 2: tris_to_add = triangle_clip_plane((vec3_f){-1.0f, 0.0f, 0.0f}, (vec3_f){1.0f, 0.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 3: tris_to_add = triangle_clip_plane((vec3_f){1.0f, 0.0f, 0.0f}, (vec3_f){-1.0f, 0.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                }

                for (uint16_t w = 0; w < tris_to_add; w++) {
                    enqueue(q, &clipped[w]);
                }
            }
            new_tri = q->size;
        }

        for (node_t* curr = q->head; curr != NULL; curr = curr->next) {
            triangle_d t = *(triangle_d*)curr->data;
            //draw_triangle_depth(t.p[0], t.p[1], t.p[2], t.color, buffer);
            // draw_triangle_fill((vec2_f){t.p[0].x, t.p[0].y}, 
            //                    (vec2_f){t.p[1].x, t.p[1].y}, 
            //                    (vec2_f){t.p[2].x, t.p[2].y}, t.color, buffer);
            draw_triangle_tex(t.p[0], t.p[1], t.p[2], 
                              t.t[0], t.t[1], t.t[2], buffer, tex);
        }

        queue_free(q);

    }

    //angle += 10.0f * dt;
}

void draw_mesh(screen_buffer* buffer, mesh_t* mesh, float_t dt, vec3_f trans, mat4x4_f proj, vec3_f camera_pos, vec3_f camera_front, pixel_data color) {

    vec3_f up = {0.0f, 1.0f, 0.0f};

    mat4x4_f view_mat = look_at(camera_pos, add_vectors(camera_pos, camera_front), up);

    mat4x4_f trans_mat = matrix_translation(trans.x, trans.y, trans.z);

    uint32_t initial_capacity = 4096;
    uint32_t triangle_count = 0;
    triangle_d* triangles_to_raster = malloc(sizeof(triangle_d) * initial_capacity);

    for (uint32_t i = 0; i + 2 < mesh->index_count; i += 3) {
        vertex_t a = mesh->vertices[mesh->indices[i + 0]];
        vertex_t b = mesh->vertices[mesh->indices[i + 1]];
        vertex_t c = mesh->vertices[mesh->indices[i + 2]];

        vec3_f point_a = a.pos;
        vec3_f point_b = b.pos;
        vec3_f point_c = c.pos;

        vec3_f trans_point_a = to_cartesian_coords(
            multiply_vec4_mat4((vec4_f){point_a.x, point_a.y, point_a.z, 1.0f}, trans_mat)
        );
        vec3_f trans_point_b = to_cartesian_coords(
            multiply_vec4_mat4((vec4_f){point_b.x, point_b.y, point_b.z, 1.0f}, trans_mat)
        );
        vec3_f trans_point_c = to_cartesian_coords(
            multiply_vec4_mat4((vec4_f){point_c.x, point_c.y, point_c.z, 1.0f}, trans_mat)
        );

        vec3_f normal = a.nor;

        float_t cam_dot = calculate_dot_product(
            normalize(sub_vectors(trans_point_a, camera_pos)), normal);

        if (cam_dot < 0.0f) {

            vec3_f view_point_a = to_cartesian_coords(multiply_vec4_mat4
                                            ((vec4_f){trans_point_a.x, trans_point_a.y, trans_point_a.z, 1.0f}, view_mat));

            vec3_f view_point_b = to_cartesian_coords(multiply_vec4_mat4
                                            ((vec4_f){trans_point_b.x, trans_point_b.y, trans_point_b.z, 1.0f}, view_mat));
                                            
            vec3_f view_point_c = to_cartesian_coords(multiply_vec4_mat4
                                            ((vec4_f){trans_point_c.x, trans_point_c.y, trans_point_c.z, 1.0f}, view_mat));

            triangle_d to_clip = {0};
            to_clip.p[0] = view_point_a;
            to_clip.p[1] = view_point_b;
            to_clip.p[2] = view_point_c;

            uint8_t num_clipped = 0;
            triangle_d clipped[2];

            num_clipped = triangle_clip_plane((vec3_f){0.0f, 0.0f, 1.0f}, (vec3_f){0.0f, 0.0f, 1.0f}, to_clip, &clipped[0], &clipped[1]);

            for (uint8_t j = 0; j < num_clipped; j++) {

                vec3_f proj_point_a = to_cartesian_coords(multiply_vec4_mat4
                                                ((vec4_f){clipped[j].p[0].x, clipped[j].p[0].y, clipped[j].p[0].z, 1.0f}, proj));
                vec3_f proj_point_b = to_cartesian_coords(multiply_vec4_mat4
                                                ((vec4_f){clipped[j].p[1].x, clipped[j].p[1].y, clipped[j].p[1].z, 1.0f}, proj));
                vec3_f proj_point_c = to_cartesian_coords(multiply_vec4_mat4
                                                ((vec4_f){clipped[j].p[2].x, clipped[j].p[2].y, clipped[j].p[2].z, 1.0f}, proj));

                triangle_d new = {0.0f};
                new.p[0] = proj_point_a;
                new.p[1] = proj_point_b;
                new.p[2] = proj_point_c;
                new.color = color;

                if (triangle_count >= initial_capacity) {
                    initial_capacity *= 2;
                    triangle_d* tmp = realloc(triangles_to_raster, sizeof(triangle_d) * initial_capacity);
                    if (!tmp) {
                        perror("Error while reallocating triangle array!\n");
                        return;
                    }
                    triangles_to_raster = tmp;
                }

                triangles_to_raster[triangle_count++] = new;

            }

        }

    }

    for (uint32_t i = 0; i < triangle_count; i++) {
        triangle_d tri = triangles_to_raster[i];

        triangle_d clipped[2];
        queue_t* q = queue_init(sizeof(triangle_d));
        enqueue(q, &tri);

        int32_t new_tri = 1;

        for (uint8_t p = 0; p < 4; p++) {
            uint8_t tris_to_add = 0;
            if (tris_to_add != 0) {
                printf("To add : %d\n", tris_to_add);
            }

            while (new_tri > 0) {
                triangle_d test = {0.0f};
                queue_front(q, &test);
                dequeue(q);

                new_tri--;

                switch(p) {
                    case 0: tris_to_add = triangle_clip_plane((vec3_f){0.0f, -1.0f, 0.0f}, (vec3_f){0.0f, 1.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 1: tris_to_add = triangle_clip_plane((vec3_f){0.0f, 1.0f, 0.0f}, (vec3_f){0.0f, -1.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 2: tris_to_add = triangle_clip_plane((vec3_f){-1.0f, 0.0f, 0.0f}, (vec3_f){1.0f, 0.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 3: tris_to_add = triangle_clip_plane((vec3_f){1.0f, 0.0f, 0.0f}, (vec3_f){-1.0f, 0.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                }

                for (uint16_t w = 0; w < tris_to_add; w++) {
                    enqueue(q, &clipped[w]);
                }
            }
            new_tri = q->size;
        }

        for (node_t* curr = q->head; curr != NULL; curr = curr->next) {
            triangle_d t = *(triangle_d*)curr->data;

            draw_triangle_depth(t.p[0], t.p[1], t.p[2], t.color, buffer);

            // for debugging only
            // draw_triangle_wireframe((vec2_f){t.p[0].x, t.p[0].y}, 
            //                (vec2_f){t.p[1].x, t.p[1].y},
            //                (vec2_f){t.p[2].x, t.p[2].y}, BLACK, buffer);
        }

        queue_free(q);

    }

    free(triangles_to_raster);

}
