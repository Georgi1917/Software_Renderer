#include <X11/Xlib.h>
#include <X11/Xutil.h>

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

double_t get_time() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return (double_t)t.tv_sec + (double_t)t.tv_nsec * 1e-9;
}

int main(void)
{

    screen_buffer* back_buffer = init_screen_buffer(800, 800);
    Display *display = XOpenDisplay(NULL);

    if (display == NULL) {
        fprintf(stderr, "Could not open X display\n");
        return 1;
    }

    int screen = DefaultScreen(display);

    Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        100, 100,
        back_buffer->width, 
        back_buffer->height,
        0,
        BlackPixel(display, screen),
        BlackPixel(display, screen)
    );

    XStoreName(display, window, "X11 Framebuffer");

    XSelectInput(
        display,
        window,
        ExposureMask |
        KeyPressMask |
        StructureNotifyMask
    );

    XMapWindow(display, window);

    XImage *image = XCreateImage(
        display,
        DefaultVisual(display, screen),
        DefaultDepth(display, screen),
        ZPixmap,
        0,
        (char *)back_buffer->mem,
        back_buffer->width,
        back_buffer->height,
        32,
        back_buffer->width * sizeof(uint8_t) * 4
    );

    if (image == NULL) {
        fprintf(stderr, "XCreateImage failed\n");

        XDestroyWindow(display, window);
        XCloseDisplay(display);

        return 1;
    }

    GC gc = XCreateGC(display, window, 0, NULL);

    int running = 1;
    XEvent event;

    mat4x4_f proj = perspective(convert_to_radians(90.0f), 
                                back_buffer->width / back_buffer->height,
                                1.0f, 1000.0f);

    vec3_f camera = {0.0f, 0.0f, 0.0f};
    vec3_f camera_front = {0.0f, 0.0f, 1.0f};
    vec3_f direction = {0.0f};
    float_t f_yaw = 90.0f;
    float_t f_pitch = 0.0f;

    double_t prev = get_time();

    while (running) {

        double_t curr = get_time();
        double_t dt = curr - prev;
        prev = curr;

        clear_buffer(DARKGRAY, back_buffer);

        direction.x = cosf(convert_to_radians(f_yaw)) * cosf(convert_to_radians(f_pitch));
        direction.y = sinf(convert_to_radians(f_pitch));
        direction.z = sinf(convert_to_radians(f_yaw)) * cosf(convert_to_radians(f_pitch));
        camera_front = normalize(direction);

        draw_cube(back_buffer, dt, (vec3_f){1.5f, 1.5f, 5.0f}, proj, camera, &camera_front);
        draw_cube(back_buffer, dt, (vec3_f){-3.5f, -1.5f, 5.0f}, proj, camera, &camera_front);

        XPutImage(
            display,
            window,
            gc,
            image,
            0, 0,
            0, 0,
            back_buffer->width,
            back_buffer->height
        );

        if (!XPending(display)) {
            continue;
        }

        XNextEvent(display, &event);

        switch (event.type) {

        case Expose:

            XFlush(display);

            break;

        case KeyPress: {
            
            if (XLookupKeysym(&event.xkey, 0) == XK_q) {
                camera.y += 10.0f * dt;
            }
            if (XLookupKeysym(&event.xkey, 0) == XK_z) {
                camera.y -= 10.0f * dt;
            }
            if (XLookupKeysym(&event.xkey, 0) == XK_Left) {
                f_yaw += 60.0f * dt;
            }
            if (XLookupKeysym(&event.xkey, 0) == XK_Right) {
                f_yaw -= 60.0f * dt;
            }
            if (XLookupKeysym(&event.xkey, 0) == XK_w) {
                vec3_f forward = mul_vector(camera_front, 5.0f * dt);
                camera = add_vectors(camera, forward);
            }
            if (XLookupKeysym(&event.xkey, 0) == XK_s) {
                vec3_f forward = mul_vector(camera_front, 5.0f * dt);
                camera = sub_vectors(camera, forward);
            }
            if (XLookupKeysym(&event.xkey, 0) == XK_a) {
                vec3_f up = {0.0f, 1.0f, 0.0f};
                camera = sub_vectors(camera, mul_vector(normalize(calculate_cross_product(camera_front, up)), -10.0f * dt));
            }
            if (XLookupKeysym(&event.xkey, 0) == XK_d) {
                vec3_f up = {0.0f, 1.0f, 0.0f};
                camera = add_vectors(camera, mul_vector(normalize(calculate_cross_product(camera_front, up)), -10.0f * dt));
            }
            if (XLookupKeysym(&event.xkey, 0) == XK_Up) {
                f_pitch += 60.0f * dt;
                if (f_pitch < -89.0f) {
                    f_pitch = -89.0f;
                }
            }
            if (XLookupKeysym(&event.xkey, 0) == XK_Down) {
                f_pitch -= 60.0f * dt;
                if (f_pitch > 89.0f) {
                    f_pitch = 89.0f;
                }
            }
            break;
        }

        case DestroyNotify:

            running = 0;
            break;
        }
    }

    XFreeGC(display, gc);

    image->data = NULL;
    XDestroyImage(image);

    XDestroyWindow(display, window);

    XCloseDisplay(display);

    free_screen_buffer(back_buffer);

    return 0;
}

