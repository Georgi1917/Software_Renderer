#include "h_math.h"

float_t convert_to_radians(float_t degrees) {
    return degrees * PI / 180.0f;
}

vec3_f calculate_normal(vec3_f a, vec3_f b, vec3_f c) {
    vec3_f normal;
    vec3_f line1, line2;

    line1 = sub_vectors(b, a);
    line2 = sub_vectors(c, a);

    normal = calculate_cross_product(line1, line2);
    normal = normalize(normal);

    return normal;
}

float_t calculate_dot_product(vec3_f a, vec3_f b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

vec3_f calculate_cross_product(vec3_f a, vec3_f b) {
    vec3_f ret = {0};

    ret.x = a.y * b.z - a.z * b.y;
    ret.y = a.z * b.x - a.x * b.z;
    ret.z = a.x * b.y - a.y * b.x;

    return ret;
}

vec3_f add_vectors(vec3_f a, vec3_f b) {
    vec3_f ret = {0};

    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    ret.z = a.z + b.z;

    return ret;
}

vec3_f sub_vectors(vec3_f a, vec3_f b) {
    vec3_f ret = {0};

    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    ret.z = a.z - b.z;

    return ret;
}

vec3_f mul_vector(vec3_f a, float_t b) {
    vec3_f ret = {0};

    ret.x = a.x * b;
    ret.y = a.y * b;
    ret.z = a.z * b;

    return ret;
}

vec3_f vector_intersect_plane(vec3_f plane_p, vec3_f plane_n, vec3_f line_start, vec3_f line_end, float_t* t) {
    plane_n = normalize(plane_n);
    
    float_t plane_d = -calculate_dot_product(plane_n, plane_p);
    float_t ad = calculate_dot_product(line_start, plane_n);
    float_t bd = calculate_dot_product(line_end, plane_n);
    *t = (-plane_d - ad) / (bd - ad);

    vec3_f line_start_to_end = sub_vectors(line_end, line_start);
    vec3_f line_to_intersect = mul_vector(line_start_to_end, *t);

    return add_vectors(line_start, line_to_intersect);
}

float_t dist_point_plane(vec3_f p, vec3_f plane_p, vec3_f plane_n) {

    plane_n = normalize(plane_n);
    return calculate_dot_product(
        sub_vectors(p, plane_p),
        plane_n
    );

}

vec2_f project_point(vec3_f p) {
    vec2_f ret = {0};

    ret.x = p.x / p.z;
    ret.y = p.y / p.z;

    return ret;
}

vec3_f normalize(vec3_f vec) {
    float_t len = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    vec.x /= len;
    vec.y /= len;
    vec.z /= len;

    return vec;
}

mat4x4_f perspective(float_t fov_rad, float_t aspect, float_t z_near, float_t z_far) {

    mat4x4_f ret = {0.0f};

    float_t _fov = 1.0f / tanf(fov_rad * 0.5f);

    ret.m[0][0] = aspect * _fov;
    ret.m[0][1] = 0.0f; 
    ret.m[0][2] = 0.0f; 
    ret.m[0][3] = 0.0f;

    ret.m[1][0] = 0.0f;
    ret.m[1][1] = _fov;
    ret.m[1][2] = 0.0f;
    ret.m[1][3] = 0.0f;

    ret.m[2][0] = 0.0f;
    ret.m[2][1] = 0.0f;
    ret.m[2][2] = z_far / (z_far - z_near);
    ret.m[2][3] = 1.0f;

    ret.m[3][0] = 0.0f;
    ret.m[3][1] = 0.0f;
    ret.m[3][2] = (-z_far * z_near) / (z_far - z_near);
    ret.m[3][3] = 0.0f;

    return ret;

}

mat4x4_f matrix_quick_invert(mat4x4_f m) {
    mat4x4_f matrix = {0.0f};

    matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;

    return matrix;
}

mat4x4_f look_at(vec3_f pos, vec3_f target, vec3_f up) {
    vec3_f forward_vec = normalize(sub_vectors(target, pos));

    vec3_f a = mul_vector(forward_vec, calculate_dot_product(up, forward_vec));
    vec3_f new_up = normalize(sub_vectors(up, a));

    vec3_f new_right = calculate_cross_product(new_up, forward_vec);

    mat4x4_f mat = {0.0f};

    mat.m[0][0] = new_right.x;   mat.m[0][1] = new_right.y;     mat.m[0][2] = new_right.z;   mat.m[0][3] = 0.0f;
    mat.m[1][0] = new_up.x;      mat.m[1][1] = new_up.y;        mat.m[1][2] = new_up.z;      mat.m[1][3] = 0.0f;
    mat.m[2][0] = forward_vec.x; mat.m[2][1] = forward_vec.y;   mat.m[2][2] = forward_vec.z; mat.m[2][3] = 0.0f;
    mat.m[3][0] = pos.x;         mat.m[3][1] = pos.y;           mat.m[3][2] = pos.z;         mat.m[3][3] = 1.0f;

    mat4x4_f look_mat = matrix_quick_invert(mat);

    return look_mat;

}

mat4x4_f matrix_rotation_x(float_t angle_rad) {
    mat4x4_f mat = {0.0f};

    mat.m[0][0] = 1.0f;
    mat.m[1][1] = cosf(angle_rad);
    mat.m[1][2] = sinf(angle_rad);
    mat.m[2][1] = -sinf(angle_rad);
    mat.m[2][2] = cosf(angle_rad);
    mat.m[3][3] = 1.0f;

    return mat;

}

mat4x4_f matrix_rotation_y(float_t angle_rad) {
    mat4x4_f mat = {0.0f};

    mat.m[0][0] = cosf(angle_rad);
    mat.m[0][2] = sinf(angle_rad);
    mat.m[2][0] = -sinf(angle_rad);
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = cosf(angle_rad);
    mat.m[3][3] = 1.0f;

    return mat;
}

mat4x4_f matrix_rotation_z(float_t angle_rad) {
    mat4x4_f mat = {0.0f};

    mat.m[0][0] = cosf(angle_rad);
    mat.m[0][1] = sinf(angle_rad);
    mat.m[1][0] = -sinf(angle_rad);
    mat.m[1][1] = cosf(angle_rad);
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;

    return mat;
}

mat4x4_f matrix_translation(float_t x, float_t y, float_t z) {
    mat4x4_f mat = {0.0f};

    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;
    mat.m[3][0] = x;
    mat.m[3][1] = y;
    mat.m[3][2] = z;

    return mat;
}

mat4x4_f multiply_mat4_mat4(mat4x4_f mat1, mat4x4_f mat2) {
    mat4x4_f mat = {0.0f};

    for (uint8_t c = 0; c < 4; c++) {
        for (uint8_t r = 0; r < 4; r++) {
            mat.m[r][c] = mat1.m[r][0] * mat2.m[0][c] + mat1.m[r][1] * mat2.m[1][c] + mat1.m[r][2] * mat2.m[2][c] + mat1.m[r][3] * mat2.m[3][c];
        }
    }

    return mat;
}

vec4_f multiply_vec4_mat4(vec4_f vec, mat4x4_f mat) {
    vec4_f ret = {0};

    ret.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + vec.w * mat.m[3][0];
    ret.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + vec.w * mat.m[3][1];
    ret.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + vec.w * mat.m[3][2];
    ret.w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + vec.w * mat.m[3][3];

    return ret;
}

vec3_f to_cartesian_coords(vec4_f vec) {
    if (vec.w != 0.0f) {
        vec.x /= vec.w;
        vec.y /= vec.w;
        vec.z /= vec.w;
    }
    return (vec3_f) { vec.x, vec.y, vec.z };
}