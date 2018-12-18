#include "mailbox.h"
#include "fb.h"

// This prevents the GPU and CPU from caching mailbox messages
#define GPU_NOCACHE 0x40000000

typedef struct {
    unsigned int width;       // width of the display
    unsigned int height;      // height of the display
    unsigned int virtual_width;  // width of the virtual framebuffer
    unsigned int virtual_height; // height of the virtual framebuffer
    unsigned int pitch;       // number of bytes per row
    unsigned int depth;       // number of bits per pixel
    unsigned int x_offset;    // x of the upper left corner of the virtual fb
    unsigned int y_offset;    // y of the upper left corner of the virtual fb
    unsigned int framebuffer; // pointer to the start of the framebuffer
    unsigned int size;        // number of bytes in the framebuffer
} fb_config_t;

// fb is volatile because the GPU will write to it
static volatile fb_config_t fb __attribute__ ((aligned(16)));

void fb_init(unsigned int width, unsigned int height, unsigned int depth, unsigned int mode) {
    // To support double-buffering, you will need to create space for two buffers by setting 
    // the virtual height of the buffer to be twice the physical height. The lower half 
    // corresponds to one buffer and the upper half is the other
    fb.height = height;
    fb.width = width;
    fb.virtual_width = width;
    if (mode == FB_DOUBLEBUFFER) {
        fb.virtual_height = 2*height;
    } else {
        fb.virtual_height = fb.height;
    }
    fb.depth = depth * 8; // convert number of bytes to number of bits
    fb.x_offset = 0;
    fb.y_offset = 0;

    // the manual requires we to set these value to 0
    // the GPU will return new values
    fb.pitch = 0;
    fb.framebuffer = 0;
    fb.size = 0;

    mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned)&fb + GPU_NOCACHE);
    (void) mailbox_read(MAILBOX_FRAMEBUFFER);
}

// inform the GPU of which part of the framebuffer to display. This involves 
// changing the Y offset from 0 to the physical height (or vice versa). This must 
// be done by writing the fb struct to the mailbox with the new x_offset and y_offset. 
void fb_swap_buffer(void) {
    if (fb.virtual_height == 2*fb.height) {
        if (fb.y_offset == 0) { //on 1st buffer, move to 2nd
            fb.y_offset = fb.height;
        } else { //on 2nd buffer, move to 1st
            fb.y_offset = 0;
        }
        mailbox_write(MAILBOX_FRAMEBUFFER, (unsigned)&fb + GPU_NOCACHE);
        (void) mailbox_read(MAILBOX_FRAMEBUFFER);
    }
   
}

/*
 * Get the start address of the portion of the framebuffer currently
 * being drawn into. This address is the start of an array of size
 * pitch*height bytes.  If in single buffering mode, the address
 * returned will not change as there is only one buffer in use.
 * In double buffering mode, the returned address differs based on 
 * which buffer is currently being used as the draw buffer.
 *
 * Returns the address of the current draw buffer
 */
unsigned char* fb_get_draw_buffer(void) {
    unsigned char*im = (unsigned char*)fb.framebuffer;
    if ((fb.virtual_height == 2*fb.height) && (fb.y_offset == 0)) { //double buffer
        im += fb.height*fb.pitch;
        return im;
    } else { //front buffer
        // address is the start of an array of size pitch*height bytes
        return im;
    }
}

unsigned int fb_get_width(void) {
    return fb.width;
}

unsigned int fb_get_height(void) {
    return fb.height;
}

unsigned int fb_get_depth(void) {
    //convert from bits to bytes 
    return fb.depth/8;
}

/*
 * Get the current pitch in bytes of a single row of pixels in the framebuffer.
 * The pitch is nominally the width (pixels per row) multiplied by the depth 
 * (in bytes per pixel). However, the pitch may be greater than that if the
 * GPU elects to add padding to the end of the row.
 */
unsigned int fb_get_pitch(void) {
    return fb.pitch;
}

