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

void clear_screen();
void set_terminal_foreground(enum screen_color color);
void set_terminal_background(enum screen_color color);
void print_char(char c);
void print_string(char *format, ...);
