#pragma once
#define BASE_GOLD_REWARD 10

void carve_path(char** map, int y1, int x1, int y2, int x2);
void mirror_map(char** map, int height, int width, int vertical);
void print_map(char** map, int height, int width, int player_y, int player_x, int vertical, int show_mirror);
void free_map(char** map, int height);
void generate_map(char*** map_ptr, int* out_height, int* out_width, int* out_y, int* out_x, int* out_vertical, int* out_sy, int* out_sx);
void clear_buffer(void);
void rebuild_right_half(char** map, int height, int width, int vertical,
	int player_y, int player_x);
void start_adventure(void);