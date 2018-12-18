#include "console.h"
#include "font.h"
#include "gl.h"
#include "fb.h"
#include "printf.h"
#include "malloc.h"

typedef struct {
    unsigned int X;
    unsigned int Y;
    unsigned char* data;
    unsigned int lineLen;
    unsigned int nrows;
    unsigned int ncols;
} cursor_t;

// fb is volatile because the GPU will write to it
static cursor_t cursor;

// void draw_screen() {
// 	gl_draw_char
// 	gl_swap_buffer();
// }

/*
 * Initialize the console. The console text begins empty and
 * the cursor is in the home position (upper left corner).
 *
 * @param nrows the requested number of rows in characters
 * @param ncols the requested number of columns in characters
 */
void console_init(unsigned int nrows, unsigned int ncols) {
    gl_init(ncols*font_get_width(), nrows*font_get_height(), GL_SINGLEBUFFER);
    cursor.X = 0;
    cursor.nrows = nrows;
    cursor.ncols = ncols;
    cursor.Y = 0;
    cursor.data = malloc(ncols*nrows);
    cursor.lineLen = ncols*font_get_width();
}

/*
 * Clear all console text and move the cursor to the home
 * position.
 */
void console_clear(void) {
    gl_clear(0x0);
    // gl_clear(gl_color(0xFF, 0, 0xFF));
    gl_swap_buffer();
    // draw_screen();
}

/*
 * Print the formatted string to the console starting at current cursor
 * position. The arguments to this function are the same as `printf`.
 * When processing characters, interpret the following special characters:
 *
 * '\n' :  newline (move cursor down to the beginning of next line)
 * '\b' :  backspace (move cursor backwards one position)
 * '\f' :  form feed (clear contents and move cursor to home position)
 *
 * @return the number of characters written to the console
 */
int console_printf(const char *format, ...) {
	int totalLen = strlen(format);
	int numChars = 0;
	va_list ap;
    va_start(ap, format);
    char *lineData = malloc(strlen(format));//malloc(strlen(cursor.ncols*cursor.nrows));
    int final = vsnprintf(lineData, cursor.lineLen, format, ap);
    printf("%s\n", lineData);
    gl_swap_buffer();
    for (int i = 0; i < totalLen; i++){
    	if (cursor.Y > fb_get_height()) {
    		console_clear();
    		cursor.Y-=font_get_height();
    	}
    	while (*lineData != '\n' && *lineData != '\f' && *lineData != '\b') {
    		if (*lineData == '~') {
    			break;
		    }
		    if (cursor.X >= fb_get_width()) {
		    	cursor.Y += font_get_height();
		    	cursor.X = 0;
		    }
	    	numChars++;
	    	gl_draw_char(cursor.X, cursor.Y, *lineData, 0xFFFFFFFF);
	    	// gl_swap_buffer();
	    	cursor.X += font_get_width();
	    	lineData++;
	    }
	    if (*lineData == '\0') {
	    	printf("finished\n");
	    	break;
	    } else if (*lineData == '\n') {
	    	cursor.Y += font_get_height();
	    	cursor.X = 0;
	    	lineData++;
	    } else if (*lineData == '\f') {
	    	console_clear();
	    	console_init(cursor.nrows, cursor.ncols);
	    	lineData++; 
	    }else if (*lineData == '\b') {
            printf("%s\n", lineData);
	    	printf("backspace\n");
	    	cursor.X -= font_get_width();
	    	gl_draw_rect(cursor.X, cursor.Y, font_get_width(), font_get_height(), 0x0);	
	    	// gl_swap_buffer();    	
	    	lineData++; 
	    }
    }
    gl_swap_buffer();
    cursor.data = lineData;
    va_end(ap);
    return numChars;
}