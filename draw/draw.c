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

list_f* _linear_interpolation(int16_t i0, int16_t d0, int16_t i1, int16_t d1) {

    list_f* values = init_list();

    if (i0 == i1) {
        append_el(values, (float_t)d0);
        return values;
    }

    float_t a = (float_t)(d1 - d0) / (float_t)(i1 - i0);
    float_t d = (float_t)d0;

    for (int16_t i = i0; i <= i1; i++) {
        append_el(values, d);
        d = d + a;
    }

    return values;

}

void _draw_line_high(int16_t x0, int16_t y0, int16_t x1, int16_t y1, pixel_data p_data, screen_buffer* buffer) {
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int8_t xi = 1;

    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int32_t D = (2 * dx) - dy;

    int16_t x = x0;

    for (int16_t y = y0; y <= y1; y++) {

        put_pixel(x, y, buffer, p_data);

        if (D > 0) {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else {
            D = D + 2 * dx;
        }

    }

}

void _draw_line_low(int16_t x0, int16_t y0, int16_t x1, int16_t y1, pixel_data p_data, screen_buffer* buffer) {
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int8_t yi = 1;

    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int32_t D = (2 * dy) - dx;

    int16_t y = y0;

    for (int16_t x = x0; x <= x1; x++) {

        put_pixel(x, y, buffer, p_data);

        if (D > 0) {
            y = y + yi;
            D = D + (2 * (dy - dx));
        }
        else {
            D = D + 2 * dy;
        }

    }
}

void _swap_points(vec2_i*a, vec2_i *b) {

    vec2_i temp = *a;
    *a = *b;
    *b = temp;

}

tr_queue* init_queue() {
    tr_queue* q = malloc(sizeof(tr_queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;

    return q;
}

void enqueue(tr_queue* q, triangle_d el) {
    node* new = malloc(sizeof(node));
    new->el = el;
    new->next = NULL;

    if (q->tail != NULL) {
        q->tail->next = new;
    }
    q->tail = new;

    if (q->head == NULL) {
        q->head = new;
    }
    q->size++;
}

void dequeue(tr_queue* q) {

    if (q->head == NULL) {
        perror("Head is empty!\n");
        return;
    }

    node* tmp = q->head;
    q->head = q->head->next;

    if (q->head == NULL) {
        q->tail = NULL;
    }
    free(tmp);
    q->size--;

}

void front(tr_queue* q, triangle_d* tr) {

    if (q->head == NULL) {
        perror("Head is empty!\n");
        return;
    }

    *tr = q->head->el;

}

void free_queue(tr_queue* q) {
    node* current = q->head;

    while(current != NULL) {
        node* next = current->next;
        free(current);
        current = next;
    }

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;

    free(q);

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
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) _draw_line_low(x1, y1, x0, y0, p_data, buffer);
        else _draw_line_low(x0, y0, x1, y1, p_data, buffer);
    }
    else {
        if (y0 > y1) _draw_line_high(x1, y1, x0, y0, p_data, buffer);
        else _draw_line_high(x0, y0, x1, y1, p_data, buffer);
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

    list_f* x01 = _linear_interpolation(p0n.y, p0n.x, p1n.y, p1n.x);
    list_f* x12 = _linear_interpolation(p1n.y, p1n.x, p2n.y, p2n.x);
    list_f* x02 = _linear_interpolation(p0n.y, p0n.x, p2n.y, p2n.x);

    pop_back(x01);

    list_f* x012 = init_list();

    for (int i = 0; i < x01->count; i++) {
        append_el(x012, x01->data[i]);
    }

    for (int i = 0; i < x12->count; i++) {
        append_el(x012, x12->data[i]);
    }

    uint16_t mid = x02->count / 2;

    if (x02->data[mid] < x012->data[mid]) {

        for (int16_t y = p0n.y; y < p2n.y; y++) {
            for (int16_t x = (int16_t)x02->data[y - p0n.y]; x < x012->data[y - p0n.y]; x++) {
                put_pixel(x, y, buffer, p_data);
            }
        }

    }
    else {

        for (int16_t y = p0n.y; y < p2n.y; y++) {
            for (int16_t x = (int16_t)x012->data[y - p0n.y]; x < x02->data[y - p0n.y]; x++) {
                put_pixel(x, y, buffer, p_data);
            }
        }
    }

    free_list(x01);
    free_list(x12);
    free_list(x02);
    free_list(x012);

}

uint8_t triangle_clip_plane(vec3_f plane_p, vec3_f plane_n, triangle_d in_tri, triangle_d* out_tri1, triangle_d* out_tri2) {
    plane_n = normalize(plane_n);

    vec3_f* inside_points[3] ; uint8_t inside_point_count = 0;
    vec3_f* outside_points[3]; uint8_t outside_point_count = 0;

    if (dist_point_plane(in_tri.p[0], plane_p, plane_n) >= 0.0f) {
        inside_points[inside_point_count++] = &in_tri.p[0];
    } else { outside_points[outside_point_count++] = &in_tri.p[0]; }
    if (dist_point_plane(in_tri.p[1], plane_p, plane_n) >= 0.0f) {
        inside_points[inside_point_count++] = &in_tri.p[1];
    } else { outside_points[outside_point_count++] = &in_tri.p[1]; }
    if (dist_point_plane(in_tri.p[2], plane_p, plane_n) >= 0.0f) {
        inside_points[inside_point_count++] = &in_tri.p[2];
    } else { outside_points[outside_point_count++] = &in_tri.p[2]; }

    if (inside_point_count == 0) {
        return 0;
    }

    if (inside_point_count == 3) {
        *out_tri1 = in_tri;
        return 1;
    }

    if (inside_point_count == 1 && outside_point_count == 2) {

        out_tri1->color = in_tri.color;

        out_tri1->p[0] = *inside_points[0];
        out_tri1->p[1] = vector_intersect_plane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1->p[2] = vector_intersect_plane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

        return 1;
    }

    if (inside_point_count == 2 && outside_point_count == 1) {

        out_tri1->color = in_tri.color;
        out_tri2->color = in_tri.color;

        out_tri1->p[0] = *inside_points[0];
        out_tri1->p[1] = *inside_points[1];
        out_tri1->p[2] = vector_intersect_plane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

        out_tri2->p[0] = *inside_points[0];
        out_tri2->p[1] = out_tri1->p[2];
        out_tri2->p[2] = vector_intersect_plane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

        return 2;
    }

    return 0;
}

void clear_buffer(pixel_data data, screen_buffer* buffer) {
    for (uint16_t y = 0; y < buffer->height; y++) {
        for (uint16_t x = 0; x < buffer->width; x++) {
            put_pixel(x, y, buffer, data);
        }
    }
}

void draw_cube(screen_buffer* buffer, float_t dt, vec3_f trans, mat4x4_f proj, vec3_f camera_pos, vec3_f* camera_front) {
    
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

    mat4x4_f view_mat = look_at(camera_pos, add_vectors(camera_pos, *camera_front), up);

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
        tr_queue* q = init_queue();
        enqueue(q, tri);

        int32_t new_tri = 1;

        for (uint8_t p = 0; p < 4; p++) {
            uint8_t tris_to_add = 0;
            if (tris_to_add != 0) {
                printf("To add : %d\n", tris_to_add);
            }

            while (new_tri > 0) {
                triangle_d test = {0.0f};
                front(q, &test);
                dequeue(q);

                new_tri--;

                switch(p) {
                    case 0: tris_to_add = triangle_clip_plane((vec3_f){0.0f, -1.0f, 0.0f}, (vec3_f){0.0f, 1.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 1: tris_to_add = triangle_clip_plane((vec3_f){0.0f, 1.0f, 0.0f}, (vec3_f){0.0f, -1.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 2: tris_to_add = triangle_clip_plane((vec3_f){-1.0f, 0.0f, 0.0f}, (vec3_f){1.0f, 0.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                    case 3: tris_to_add = triangle_clip_plane((vec3_f){1.0f, 0.0f, 0.0f}, (vec3_f){-1.0f, 0.0f, 0.0f}, test, &clipped[0], &clipped[1]); break;
                }

                for (uint16_t w = 0; w < tris_to_add; w++) {
                    enqueue(q, clipped[w]);
                }
            }
            new_tri = q->size;
        }

        for (node* curr = q->head; curr != NULL; curr = curr->next) {
            triangle_d t = curr->el;

            draw_triangle_fill((vec2_f){t.p[0].x, t.p[0].y}, 
                           (vec2_f){t.p[1].x, t.p[1].y},
                           (vec2_f){t.p[2].x, t.p[2].y}, t.color, buffer);
            draw_triangle_wireframe((vec2_f){t.p[0].x, t.p[0].y}, 
                           (vec2_f){t.p[1].x, t.p[1].y},
                           (vec2_f){t.p[2].x, t.p[2].y}, BLACK, buffer);
        }

        free_queue(q);

    }

    angle += 10.0f * dt;

}

void load_model(void) {
    const struct aiScene* scene = aiImportFile(
        "Pesho",
        aiProcess_Triangulate
    );

    if (!scene) {
        printf("Mesh could not be loaded!\n");
        return;
    }

}
