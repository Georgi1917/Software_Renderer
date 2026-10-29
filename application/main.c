#include <SDL3/SDL.h>

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "draw.h"
#include "colors.h"
#include "struct_utils.h"

float_t get_time() {
    return (float_t)SDL_GetTicks() / 1000.0f + (float_t)SDL_GetTicksNS() * 1e-9;
}

void visualize_mesh_struct(mesh_t mesh) {
    printf("Vertices : \n");
    for(uint32_t i = 0; i < mesh.vertex_count; i++) {
        printf("Pos : %f %f %f | Nor : %f %f %f\n", 
            mesh.vertices[i].pos.x, mesh.vertices[i].pos.y, mesh.vertices[i].pos.z,
            mesh.vertices[i].nor.x, mesh.vertices[i].nor.y, mesh.vertices[i].nor.z
        );
    }
}

int main(void)
{
    screen_buffer *back_buffer = init_screen_buffer(800, 800);

    if (back_buffer == NULL) {
        fprintf(stderr, "Could not create screen buffer\n");
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        free_screen_buffer(back_buffer);
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Software Renderer",
        back_buffer->width,
        back_buffer->height,
        SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        free_screen_buffer(back_buffer);
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, "software");
    printf("Using rendering backend : %s\n", SDL_GetRendererName(renderer));

    if (renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        free_screen_buffer(back_buffer);
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_BGRA32,
        SDL_TEXTUREACCESS_STREAMING,
        back_buffer->width,
        back_buffer->height
    );

    if (texture == NULL) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        free_screen_buffer(back_buffer);
        return 1;
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);

    bool running = true;
    SDL_Event event;

    mat4x4_f proj = perspective(convert_to_radians(90.0f), (float)back_buffer->width / (float)back_buffer->height, 1.0f, 1000.0f);

    vec3_f camera = {0.0f, 0.0f, 0.0f};
    vec3_f camera_front = {0.0f, 0.0f, 1.0f};
    vec3_f direction = {0.0f};

    float_t f_yaw = 90.0f;
    float_t f_pitch = 0.0f;

    float_t prev = get_time();
    mesh_t mesh = {0};
    load_mesh(&mesh);

    while (running) {

        float_t curr = get_time();
        float_t dt = curr - prev;
        prev = curr;

        while (SDL_PollEvent(&event)) {

            switch (event.type) {

            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                }
                break;

            default:
                break;
            }
        }

        if (!running) {
            break;
        }

        const bool *keys = SDL_GetKeyboardState(NULL);

        direction.x = cosf(convert_to_radians(f_yaw)) * cosf(convert_to_radians(f_pitch));

        direction.y = sinf(convert_to_radians(f_pitch));

        direction.z = sinf(convert_to_radians(f_yaw)) * cosf(convert_to_radians(f_pitch));

        camera_front = normalize(direction);

        if (keys[SDL_SCANCODE_Q]) {
            camera.y += 10.0f * dt;
        }

        if (keys[SDL_SCANCODE_Z]) {
            camera.y -= 10.0f * dt;
        }

        if (keys[SDL_SCANCODE_W]) {
            camera = add_vectors(
                camera,
                mul_vector(camera_front, 5.0f * dt)
            );
        }

        if (keys[SDL_SCANCODE_S]) {
            camera = sub_vectors(
                camera,
                mul_vector(camera_front, 5.0f * dt)
            );
        }

        if (keys[SDL_SCANCODE_A]) {
            vec3_f up = {0.0f, 1.0f, 0.0f};
            camera = sub_vectors(
                camera, 
                mul_vector(normalize(
                    calculate_cross_product(camera_front, up)), -10.0f * dt)
            );
        }

        if (keys[SDL_SCANCODE_D]) {
            vec3_f up = {0.0f, 1.0f, 0.0f};
            camera = add_vectors(
                camera, 
                mul_vector(normalize(
                    calculate_cross_product(camera_front, up)), -10.0f * dt)
            );
        }

        if (keys[SDL_SCANCODE_LEFT]) {
            f_yaw += 60.0f * dt;
        }

        if (keys[SDL_SCANCODE_RIGHT]) {
            f_yaw -= 60.0f * dt;
        }

        if (keys[SDL_SCANCODE_UP]) {
            f_pitch += 60.0f * dt;

            if (f_pitch > 89.0f) {
                f_pitch = 89.0f;
            }
        }

        if (keys[SDL_SCANCODE_DOWN]) {
            f_pitch -= 60.0f * dt;

            if (f_pitch < -89.0f) {
                f_pitch = -89.0f;
            }
        }

        clear_buffer(DARKGRAY, back_buffer);

        // draw_cube(back_buffer, dt, (vec3_f){1.5f, 1.5f, 5.0f}, proj, camera, &camera_front);
        // draw_cube(back_buffer, dt, (vec3_f){-3.5f, -1.5f, 5.0f}, proj, camera, &camera_front);

        draw_mesh(back_buffer, &mesh, dt, (vec3_f){0.0f, 0.0f, 5.0f}, proj, camera, camera_front);

        if (!SDL_UpdateTexture(
                texture,
                NULL,
                back_buffer->mem,
                back_buffer->width * 4)) {

            fprintf(stderr, "SDL_UpdateTexture failed: %s\n",
                    SDL_GetError());
            running = false;
            break;
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    free_mesh(&mesh);
    free_screen_buffer(back_buffer);

    return 0;
}