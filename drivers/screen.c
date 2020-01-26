#include <stdint.h>
#include "screen.h"
#include "ports.h"
#include "../kernel/util.h"
#include <stdarg.h>.

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define VGA_PORT_CTRL 0x3d4
#define VGA_PORT_DATA 0x3d5
#define VGA_PORT_LOW_CURSOR_BITS 15
#define VGA_PORT_HIGH_CURSOR_BITS 14

int terminal_row;
int terminal_column;
unsigned char* terminal_buffer;
enum screen_color terminal_foreground_color;
enum screen_color terminal_background_color;

int get_terminal_offset(int row, int column) {
    return 2 * (row * MAX_COLS + column);
}

void set_cursor_offset(int offset) {
    // Set the cursor offset via VGA ports
    offset /= 2;
    port_byte_out(VGA_PORT_CTRL, VGA_PORT_HIGH_CURSOR_BITS);
    port_byte_out(VGA_PORT_DATA, (unsigned char)(offset >> 8));
    port_byte_out(VGA_PORT_CTRL, VGA_PORT_LOW_CURSOR_BITS);
    port_byte_out(VGA_PORT_DATA, (unsigned char)(offset & 0xff));
}

void scroll_up() {
    memory_copy(
        ((char *) VIDEO_ADDRESS),
        ((char *) VIDEO_ADDRESS) + get_terminal_offset(1, 0),
        get_terminal_offset(MAX_ROWS, MAX_COLS)
    );
}

void clear_screen() {
    terminal_row = 0;
    terminal_column = 0;

    terminal_background_color = SCREEN_COLOR_BLACK;
    terminal_foreground_color = SCREEN_COLOR_WHITE;

    terminal_buffer = (unsigned char *) VIDEO_ADDRESS;

    for (int column = 0; column < MAX_COLS; column++) {
        for (int row = 0; row < MAX_ROWS; row++) {
            int offset = get_terminal_offset(row, column);
            terminal_buffer[offset] = ' ';
            terminal_buffer[offset + 1] = SCREEN_COLOR_WHITE;
        }
    }

    set_cursor_offset(0);
}

void set_terminal_foreground(enum screen_color color) {
    terminal_foreground_color = color;
}

void set_terminal_background(enum screen_color color) {
    terminal_background_color = color;
}

void print_char_at(char c, int row, int col) {
    int offset = get_terminal_offset(row, col);
    uint8_t color = (terminal_background_color << 4) | terminal_foreground_color;

    terminal_buffer[offset] = c;
    terminal_buffer[offset + 1] = color;
}

void print_char(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else {
        print_char_at(c, terminal_row, terminal_column);
        terminal_column++;
        if (terminal_column >= MAX_COLS) {
            terminal_row++;
            terminal_column = 0;
        }
    }

    int offset = get_terminal_offset(terminal_row, terminal_column);

    if (terminal_row >= MAX_ROWS) {
        scroll_up();
        terminal_row--;
    }

    set_cursor_offset(offset);
}

void print_string(char *format, ...) {
    va_list args;
    va_start(args, format);

    char str_buffer[32];

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] == 'd') {
            i++; // Skip the percentage character
            int number = va_arg(args, int);
            itoa(number, str_buffer);
            print_string(str_buffer);
        } else {
            print_char(format[i]);
        }
    }

    va_end(args);
}
