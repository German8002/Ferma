#pragma once
#include "player.h"

extern int current_day;
extern int current_hour;
extern char item_names[MAX_ITEMS][MAX_NAME_LEN];

void clear_buffer(void);
int fool_check(int scanf_result);
void handle_time_menu(int* day, int* hour, Player* p);
int can_go_adventure(Player* p, int current_hour);
void read_file(void);
void diary_writer(Player* p);
void map_unpacker(void);
void compare_items(Player* p);