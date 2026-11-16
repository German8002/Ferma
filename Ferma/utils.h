#pragma once

#include "player.h"

void clear_buffer(void);
void handle_time_menu(int* day, int* hour, Player* p);
int can_go_adventure(Player* p, int current_hour);