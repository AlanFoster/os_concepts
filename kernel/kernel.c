#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#include <stdint.h>

enum screen_color {
	SCREEN_COLOR_BLACK = 0,
	SCREEN_COLOR_BLUE = 1,
	SCREEN_COLOR_GREEN = 2,
	SCREEN_COLOR_CYAN = 3,
	SCREEN_COLOR_RED = 4,
	SCREEN_COLOR_MAGENTA = 5,
	SCREEN_COLOR_BROWN = 6,
	SCREEN_COLOR_LIGHT_GREY = 7,
	SCREEN_COLOR_DARK_GREY = 8,
	SCREEN_COLOR_LIGHT_BLUE = 9,
	SCREEN_COLOR_LIGHT_GREEN = 10,
	SCREEN_COLOR_LIGHT_CYAN = 11,
	SCREEN_COLOR_LIGHT_RED = 12,
	SCREEN_COLOR_LIGHT_MAGENTA = 13,
	SCREEN_COLOR_LIGHT_BROWN = 14,
	SCREEN_COLOR_WHITE = 15,
};

int terminal_row;
int terminal_column;
unsigned char* terminal_buffer;
enum screen_color terminal_foreground_color;
enum screen_color terminal_background_color;

int get_terminal_offset(int row, int column) {
    return (row * (MAX_COLS * 2)) + (column * 2);
}

void initialize_screen() {
    terminal_row = 0;
    terminal_column = 0;

    terminal_background_color = SCREEN_COLOR_BLACK;
    terminal_foreground_color = SCREEN_COLOR_WHITE;

    terminal_buffer = (unsigned char *) VIDEO_ADDRESS;

    for (int column = 0; column < MAX_COLS; column++) {
        for (int row = 0; row < MAX_ROWS; row++) {
            int offset = get_terminal_offset(row, column);
            terminal_buffer[offset] = 0;
            terminal_buffer[offset + 1] = 0;
        }
    }
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
        return;
    }

    print_char_at(c, terminal_row, terminal_column);
    terminal_column++;
    if (terminal_column > MAX_COLS) {
        terminal_row++;
        terminal_column = 0;
    }
}

void print_string(char *s) {
    for (int i = 0; s[i] != 0; i++) {
        print_char(s[i]);
    }
}

void main() {
    initialize_screen();

    char* video_memory = (char*) 0xb8000;
    char* first_line = "Hello world\n\0";
    char* second_line = "Testing\n\0";
    char* third_line = "1234\n\0";

    print_string(first_line);
    print_string(second_line);
    print_string(third_line);
}
