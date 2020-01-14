#include "../drivers/screen.h"

void main() {
    clear_screen();

    char* first_line = "Hello world\n\0";
    char* second_line = "Testing\n\0";
    char* third_line = "1234\n\0";

    set_terminal_foreground(SCREEN_COLOR_LIGHT_BLUE);
    print_string(first_line);

    set_terminal_foreground(SCREEN_COLOR_GREEN);
    print_string(second_line);

    set_terminal_foreground(SCREEN_COLOR_RED);
    print_string(third_line);
}
