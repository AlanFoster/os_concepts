#include "keyboard.h"
#include "../cpu/isr.h"
#include "../drivers/ports.h"
#include "../drivers/screen.h"

#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60

// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
const char *scancode_name[] = {
    "ERROR",
    "Escape",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "-",
    "=",
    "Backspace",
    "Tab",
    "Q",
    "W",
    "E",
    "R",
    "T",
    "Y",
    "U",
    "I",
    "O",
    "P",
    "[",
    "]",
    "Enter",
    "LeftControl",
    "A",
    "S",
    "D",
    "F",
    "G",
    "H",
    "J",
    "K",
    "L",
    ";",
    "'",
    "`",
    "LeftShift",
    "\\",
    "Z",
    "X",
    "C",
    "V",
    "B",
    "N",
    "M",
    ",",
    ".",
    "/",
    "RightShift",
    "Keypad *",
    "LeftAlt",
    "SpaceBar"
};

const char scancode_ascii[] = {
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
    '[',
    ']',
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
    ';',
    '\'',
    '`',
    '?',
    '\\',
    'Z',
    'X',
    'C',
    'V',
    'B',
    'N',
    'M',
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
    if (is_key_release(scancode)) {
        return;
    }

    print_char(scancode_ascii[scancode]);
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}
