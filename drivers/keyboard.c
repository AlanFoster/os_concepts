#include "keyboard.h"
#include "../cpu/isr.h"
#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "../kernel/kernel.h"
#include "../kernel/util.h"

#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60
#define LEFT_SHIFT_DOWN 0x2a
#define LEFT_SHIFT_UP 0xaa
#define RIGHT_SHIFT_UP 0x36
#define RIGHT_SHIFT_DOWN 0xb6
#define CAPS_LOCK 0x3a
#define BACKSPACE 0x0e
#define ENTER 0x1c
#define INPUT_BUFFER_SIZE 256

enum KeyboardState {
    None = 0,
    ShiftDown = 1 << 0,
    CapsDown = 1 << 1
};

static char input_buffer[INPUT_BUFFER_SIZE];
static int8_t pointer;

enum KeyboardState state = None;

const char upper_scancode_ascii[] = {
    '?',
    '?',
    '!',
    '@',
    // £ sign
    '\xa3',
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '_',
    '+',
    '?',
    '?',
    'Q',
    'W',
    'E',
    'R',
    'T',
    'Y',
    'U',
    'I',
    'O',
    'P',
    '{',
    '}',
    '?',
    '?',
    'A',
    'S',
    'D',
    'F',
    'G',
    'H',
    'J',
    'K',
    'L',
    ':',
    '\'',
    '`',
    '?',
    '|',
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
    '<',
    '>',
    '?',
    '?',
    '?',
    '?',
    ' '
};

const char lower_scancode_ascii[] = {
    '?',
    '?',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    '?',
    '?',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '?',
    '?',
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    '?',
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    '?',
    '?',
    '?',
    ' '
};

int has_buffer_available(uint8_t status) {
    return status & 0x01;
}

int is_key_release(uint8_t scancode) {
    // If the 8th bit is set, then the key has been released
    return scancode & 0x80;
}

static void keyboard_callback(__attribute__((unused)) ISR_event e) {
    uint8_t status = port_byte_in(KEYBOARD_STATUS_PORT);
    uint8_t scancode;
    if (!has_buffer_available(status)) {
        return;
    }

    scancode = port_byte_in(KEYBOARD_DATA_PORT);

    if (scancode == CAPS_LOCK) {
        state ^= CapsDown;
        return;
    }

    if (scancode == LEFT_SHIFT_DOWN ||
        scancode == LEFT_SHIFT_UP ||
        scancode == RIGHT_SHIFT_DOWN ||
        scancode == RIGHT_SHIFT_UP) {
        if (is_key_release(scancode)) {
            state &= ~ShiftDown;
        } else {
            state |= ShiftDown;
        }
        return;
    }

    if (is_key_release(scancode)) {
        return;
    }

    if (
        scancode == BACKSPACE
    ) {
        if (input_buffer[0] != '\0') {
            back_char();
            input_buffer[--pointer] = '\0';
        }

        return;
    }

    if (
        scancode == ENTER
    ) {
        print_char('\n');
        on_user_input(input_buffer);
        memory_set(input_buffer, 0, INPUT_BUFFER_SIZE);
        pointer = 0;
        return;
    }

    // TODO: There still needs to be more granularity here.
    // If caps is down, and shift is down, its lower case letters.
    // But it's only upper "case" symbols when shift is down.
    if (
        state == None ||
        ((state & (CapsDown | ShiftDown)) == (CapsDown | ShiftDown))
    ) {
        print_char(lower_scancode_ascii[scancode]);
        input_buffer[pointer++] = lower_scancode_ascii[scancode];
    } else {
        print_char(upper_scancode_ascii[scancode]);
        input_buffer[pointer++] = upper_scancode_ascii[scancode];
    }
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
    input_buffer[0] = '\0';
    pointer = 0;
}
