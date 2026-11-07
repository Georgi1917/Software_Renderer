#ifndef __COLORS_H__
#define __COLORS_H__

#define LIGHTGRAY  (pixel_data){ 200, 200, 200 }
#define GRAY       (pixel_data){ 130, 130, 130 }        // Gray
#define DARKGRAY   (pixel_data){ 80, 80, 80 }      // Dark Gray
#define YELLOW     (pixel_data){ 253, 249, 0 }     // Yellow
#define GOLD       (pixel_data){ 255, 203, 0 }     // Gold
#define ORANGE     (pixel_data){ 255, 161, 0 }     // Orange
#define PINK       (pixel_data){ 255, 109, 194 }   // Pink
#define RED        (pixel_data){ 230, 41, 55 }     // Red
#define MAROON     (pixel_data){ 190, 33, 55 }     // Maroon
#define GREEN      (pixel_data){ 0, 228, 48 }      // Green
#define LIME       (pixel_data){ 0, 158, 47 }      // Lime
#define DARKGREEN  (pixel_data){ 0, 117, 44 }      // Dark Green
#define SKYBLUE    (pixel_data){ 102, 191, 255 }   // Sky Blue
#define BLUE       (pixel_data){ 0, 121, 241 }     // Blue
#define DARKBLUE   (pixel_data){ 0, 82, 172 }      // Dark Blue
#define PURPLE     (pixel_data){ 200, 122, 255 }   // Purple
#define VIOLET     (pixel_data){ 135, 60, 190 }    // Violet
#define DARKPURPLE (pixel_data){ 112, 31, 126 }    // Dark Purple
#define BEIGE      (pixel_data){ 211, 176, 131 }   // Beige
#define BROWN      (pixel_data){ 127, 106, 79 }    // Brown
#define DARKBROWN  (pixel_data){ 76, 63, 47 }      // Dark Brown

#define WHITE      (pixel_data){ 255, 255, 255 }   // White
#define BLACK      (pixel_data){ 0, 0, 0 }         // Black
#define MAGENTA    (pixel_data){ 255, 0, 255 }

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_data;

#endif //__COLORS_H__
