#include "gl.h"
#include "fb.h"

// Initializing the graphics library is mostly a matter of making the the 
// appropriate call to initialize the underlying framebuffer. Assume the graphics 
// library always is configured for 32-bit depth. Each pixel stores a 4-byte BGRA color.
void gl_init(unsigned int width, unsigned int height, unsigned int mode) {
    fb_init(width, height, 4, mode);
}

/*
 * Swap the front and back buffers. The draw buffer is moved to the 
 * front (displayed) and the front buffer is moved to the back
 * (becomes the draw buffer).
 *
 * If not in double-buffer mode, this function has no effect.
 */
void gl_swap_buffer(void) {
    fb_swap_buffer();
}

unsigned int gl_get_width(void) {
    return fb_get_width;
}

unsigned int gl_get_height(void) {
    return fb_get_height;
}

/*
 * Returns a color composed of the specified red, green, and
 * blue components. The alpha component of the color will be
 * set to 0xff (fully opaque).
 *
 * Returns the color as a single value of type color_t
 */
color_t gl_color(unsigned char r, unsigned char g, unsigned char b) {
    color_t currColor = 0xff;
    currColor = (currColor*256) + r;
    currColor = (currColor*256) + g;
    currColor = (currColor*256) + b;
    return currColor;
}

/* Clear all the pixels in the framebuffer to the given color */
void gl_clear(color_t c) {
    int myPitch = fb_get_pitch()/fb_get_depth();
    color_t (*base)[myPitch] = (color_t (*)[myPitch]) fb_get_draw_buffer();
    for (int i = 0; i < myPitch; i++) {
        for (int j = 0; j < fb_get_height(); j++) {
            base[j][i] = c;
        }
    }
}

//cast fb base to 2d array type color_t pointer to pointer
void gl_draw_pixel(int x, int y, color_t c) {
    int myPitch = fb_get_pitch()/fb_get_depth();
    color_t (*base)[myPitch] = (color_t (*)[myPitch]) fb_get_draw_buffer();
    base[y][x] = c;
}

color_t gl_read_pixel(int x, int y) {
    int myPitch = fb_get_pitch()/fb_get_depth();
    color_t (*base)[myPitch] = (color_t (*)[myPitch]) fb_get_draw_buffer();
    return base[y][x];
}

/*
 * Draw a filled rectangle at location x,y with size w,h filled with color c.
 * All pixels in the rectangle that lie within the bounds of the 
 * framebuffer are drawn. Any pixel that lies outside is clipped (i.e. not drawn).
 */
void gl_draw_rect(int x, int y, int w, int h, color_t c) {
    for (int i = x; i < x+w; i++) {
        for (int j = y; j < y+h; j++) {
            gl_draw_pixel(i, j, c);
        }
    }
}

/*
 * Draw a single character at location x,y in color c.
 * Only those pixels of the character that lie within the bounds
 * of the framebuffer are drawn. Any pixel that lies outside is
 * clipped (i.e. not drawn).
 */
void gl_draw_char(int x, int y, int ch, color_t c) {
    unsigned char buf[font_get_size()];
    if (font_get_char(ch, buf, font_get_size())) {
        unsigned char (*buf2D)[font_get_width()] = (unsigned char (*)[font_get_width()]) buf;
        for (int i = 0; i < gl_get_char_height(); i ++) {
            for (int j = 0; j < gl_get_char_width(); j++) {
                // printf("%d", buf2D[i][j]);
                if (buf2D[i][j] == 0xFF) {
                    gl_draw_pixel(x+j, y+i, c);
                }   
            }
            // printf("\n");
        }
    }
}

/*
 * Draw a string at location x,y in color c. The characters are drawn
 * left to right in a single line. Only the pixels of the characters
 * that lie within the bounds of the framebuffer are drawn. Any pixel
 * that lies outside is clipped (i.e. not drawn).
 */
void gl_draw_string(int x, int y, char* str, color_t c) {
    char* original = str;
    for (int i = 0; i < strlen(original); i++) {
        gl_draw_char(x+(font_get_width()*i), y, (int) *str, c);
        str++;
    }
}

unsigned int gl_get_char_height(void) {
    return font_get_height();
}

unsigned int gl_get_char_width(void) {
    return font_get_width();
}

