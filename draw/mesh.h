#ifndef __MESH__
#define __MESH__
#include <stdint.h>
#include <stdbool.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "h_math.h"

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

mesh_t* load_mesh(const char* filepath);
void free_mesh(mesh_t* mesh);

#endif // __MESH__