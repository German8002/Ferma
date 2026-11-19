#pragma once
#include "player.h"
#include "entity.h"
#define BASE_GOLD_REWARD 10

typedef struct
{
	char** cells;
	int width;
	int height;
	int level;

	Entity* entities; //Массив сущностей
	int entity_count; //Сколько всего сущностей на этаже
} Map;

void carve_path(Map* m, char** map, int y1, int x1, int y2, int x2);
void mirror_map(Map* m, int vertical);
void print_map(const Map* m, int player_y, int player_x, int vertical, int show_mirror);
void free_map(Map* m);
void generate_map(Map* m, int* out_y, int* out_x, int* out_vertical, int* out_sy, int* out_sx);
void clear_buffer(void);
void rebuild_right_half(Map* m, int vertical, int player_y, int player_x);
void start_adventure(Player* p, int* day, int* hour);