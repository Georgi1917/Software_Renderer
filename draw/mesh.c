#include <stdlib.h>
#include <stdio.h>
#include "mesh.h"

mesh_t* load_mesh(const char* filepath) {

    mesh_t* mesh_t = calloc(1, sizeof(mesh_t));

    const struct aiScene* scene = aiImportFile(
        filepath,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipWindingOrder
    );

    if (!scene) {
        printf("Mesh could not be loaded!\n");
        return NULL;
    }

    printf("Mesh number : %d\n", scene->mNumMeshes);
    uint32_t num_vertices = 0;
    uint32_t num_faces = 0;

    for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
        const struct aiMesh* mesh = scene->mMeshes[i];
        num_vertices += mesh->mNumVertices;
        
        for (uint32_t j = 0; j < mesh->mNumFaces; j++) {
            const struct aiFace face = mesh->mFaces[j];

            if (face.mNumIndices != 3) {
                continue;
            }

            num_faces += 3;

        }

    }

    printf("Total vertices : %d\n", num_vertices);
    printf("Total Faces : %d\n", num_faces);

    mesh_t->vertex_count = num_vertices;
    mesh_t->index_count = num_faces;

    mesh_t->vertices = malloc(sizeof(vertex_t) * num_vertices);
    mesh_t->indices  = malloc(sizeof(uint32_t) * num_faces);

    uint32_t ver_idx = 0;
    uint32_t ind_idx = 0;

    for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
        const struct aiMesh* mesh = scene->mMeshes[i];

        uint32_t ver_idx_base = ver_idx;

        for (uint32_t j = 0; j < mesh->mNumVertices; j++) {
            vertex_t vec = {0};
            vec.pos = (vec3_f){mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};

            if (mesh->mNormals) {
                vec.nor = (vec3_f){mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};
            }
            else {
                vec.nor = (vec3_f){0.0f, 0.0f, 0.0f};
            }

            mesh_t->vertices[ver_idx++] = vec;

        }

        for (uint32_t j = 0; j < mesh->mNumFaces; j++) {
            const struct aiFace face = mesh->mFaces[j];

            if (face.mNumIndices != 3) {
                continue;
            }

            for (uint32_t f = 0; f < 3; f++) {
                mesh_t->indices[ind_idx++] = ver_idx_base + face.mIndices[f];
            } 

        }

    }

    return mesh_t;

}

void free_mesh(mesh_t* mesh) {
    free(mesh->vertices);
    free(mesh->indices);
    free(mesh);
}