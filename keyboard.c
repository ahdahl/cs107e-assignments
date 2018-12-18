#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"

const unsigned int CLK  = GPIO_PIN23;
const unsigned int DATA = GPIO_PIN24; 

void wait_for_falling_clock_edge() {
    while (gpio_read(CLK) == 0) {}
    while (gpio_read(CLK) == 1) {}
}

void keyboard_init(void) 
{
    gpio_set_input(CLK); 
    gpio_set_pullup(CLK); 
 
    gpio_set_input(DATA); 
    gpio_set_pullup(DATA); 
}

/*
 * `keyboard_read_scancode`: Bottom level keyboard interface.
 *
 * Read (blocking) a single scan code byte from the PS/2 keyboard.
 * Bits are read on the falling edge of the clock.
 *
 * Tries to read 11 bits: 1 start bit, 8 data bits, 1 parity bit, and 1 stop bit
 *
 * Restarts the scan code if:
 *   (lab5) The start bit is incorrect
 *   (assign5) or if parity or stop bit is incorrect
 *
 * Returns the 8 data bits of a well-formed PS/2 scan code. 
 * Will not return until it reads a valid scan code.
 */
unsigned char keyboard_read_scancode(void) {
	while (true) {
		wait_for_falling_clock_edge();
		unsigned int startBit = gpio_read(DATA);
	    if (startBit == 1) {
	    	break; //retry reading a new scancode from the beginning
	    }
	    unsigned int numOnes = 0;
	    unsigned char c = 0;
	    for (int i = 0; i < 8; i++) {
	        wait_for_falling_clock_edge();
	        unsigned int digit = gpio_read(DATA);
	        c = c | (digit << i);
	        if (gpio_read(DATA) == 1) {
	            numOnes++;
	        }
	    }
	    //parity
	    wait_for_falling_clock_edge();
	    unsigned int parity = gpio_read(DATA);
	    numOnes += parity;
	    if (numOnes % 2 == 0) {
	    	break;
	    }
	    //end bit
	    wait_for_falling_clock_edge();
	    unsigned int endBit = gpio_read(DATA);
	    if (endBit == 0) {
	        break;
	    }
	    return c;
	}
}

/*
 * `keyboard_read_sequence`: Low level keyboard interface.
 *
 * Reads the sequence of scan codes corresponding to next key action.
 * A key action is either a key down or key up. A sequence can
 * 1, 2, or 3 bytes, as follows:
 *    1 byte:  simple key down
 *    2 bytes: simple key up or extended key down
 *    3 bytes: extended key up
 * The function stores the scan code bytes into the `seq` array and
 * returns the number of bytes written to the array. This function
 * calls `keyboard_read_scancode` to read each scan code.
 */
int keyboard_read_sequence(unsigned char seq[]) {
	int numBytes = 0;
	unsigned char curr = keyboard_read_scancode();
	if (curr == 0xe0) {
		seq[numBytes] = curr; //[e0]
		numBytes++;
		curr = keyboard_read_scancode();
		seq[numBytes] = curr; //[e0, f0] or [e0, 1a]
		numBytes++;
		if (curr == 0xf0) {
			curr = keyboard_read_scancode();
			seq[numBytes] = curr; //[e0, f0, 1a]
			numBytes++;
		}
		return numBytes;
	} else if (curr == 0xf0) {
		seq[numBytes] = curr; //[f0]
		numBytes++;
		curr = keyboard_read_scancode();
		seq[numBytes] = curr; //[f0, 1a]
		numBytes++;
		return numBytes;
	} else {
		seq[numBytes] = curr; //[1a]
		numBytes++;
		return numBytes;
	}
    return numBytes;
}

/*
 * `keyboard_read_event`: Mid level keyboard interface.
 *
 * The function reads (blocking) the next key action from the keyboard.
 * Returns a `key_event_t` struct that represents the key event.  The
 * struct includes the sequence of raw scan codes, the action (up or down),
 * and the state of the modifier flags in effect. If this event is a
 * a key down or up for a modifier key (CTRL, ALT, SHIFT, etc.), the
 * modifiers field in the event contains the state of the modifiers
 * after having incorporated this key action.
 * This function calls `keyboard_read_sequence`.
 */
key_event_t keyboard_read_event(void) {
	key_event_t event;
	unsigned char seq[3];
    // unsigned char seq[3];     // sequence of raw scan code bytes
    // int seq_len;              // number of bytes in sequence
    // ps2_key_t key;            // entry taken from ps2_keys table (see ps2.h)
    // keyboard_action_t action; // either KEYBOARD_ACTION_UP or KEYBOARD_ACTION_DOWN
    // keyboard_modifiers_t modifiers;   // modifiers in effect, composed of above bit flags
	event.seq_len = keyboard_read_sequence(seq);
	for (int i = 0; i< 3; i++){
		event.seq[i] = seq[i];
	}
	event.action = KEYBOARD_ACTION_DOWN;
	if (seq[0] == PS2_CODE_RELEASE || seq[1] == PS2_CODE_RELEASE) {
		event.action = KEYBOARD_ACTION_UP;
	}
    return event;
}

/*
 * `keyboard_read_next`: Top level keyboard interface.
 *
 * This function reads (blocking) the next key typed on the keyboard.
 * The character returned reflects the current keyboard modifier settings
 * for shift and caps lock.
 *
 * Characters returned that have value <= 0x7f '~' are printable ascii
 * characters. Character values >= 0x90 are returned for those keys that are
 * are not associated with an ascii character (e.g. arrow and function keys).
 * See the ps2_codes defined in ps2.h for constants used for those keys.
 * This function calls `keyboard_read_event`.
 */
unsigned char keyboard_read_next(void) 
{
    // TODO: Your code here
    return '!';
}
