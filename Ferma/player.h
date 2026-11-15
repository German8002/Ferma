#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_FARMER_NAME 32
#define MAX_ITEMS 10
#define MAX_NAME_LEN 32

typedef enum
{
    FARMER = 0,
    LUMBERJACK,
    MINER,
    SCOUT
} Profession;

typedef struct
{
    int bonus_hp;
    int bonus_stamina;
    int farmer_bonus;
    int lumberjack_bonus;
    int miner_bonus;
    int scout_bonus;
} Profession_desc;

typedef struct 
{
    char name[MAX_FARMER_NAME];
    int hp;
    int max_hp;
    int stamina;
    int max_stamina;
    int gold;
    int x, y; // Координаты на карте приключения
    int inventory[MAX_ITEMS];
    Profession_desc cls;
} Player;
