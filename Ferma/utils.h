#pragma once
#include "player.h"

extern int current_day;
extern int current_hour;
extern char item_names[MAX_ITEMS][MAX_NAME_LEN];
typedef enum
{
    MENU_EXIT = 0,
    MENU_TIME = 1,
    MENU_CREATE = 2,
    MENU_CARD = 3,
    MENU_ITEM_PLACE = 4,
    MENU_ITEM_DROP = 5,
    MENU_UNIQUE = 6,
    MENU_COMPARE = 7,
    MENU_DIARY = 8,
    MENU_UNPACK = 9,
    MENU_ADVENTURE = 10
} MenuOption;

void clear_buffer(void);
int fool_check(int scanf_result);
void handle_time_menu(int* day, int* hour, Player* p);
int can_go_adventure(Player* p, int current_hour);
void read_file(void);
void diary_writer(Player* p);
void map_unpacker(void);
void compare_items(Player* p);