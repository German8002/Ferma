#pragma once

typedef enum 
{
	ENT_TREE,
	ENT_ROCK,
	ENT_HEAL,
	ENT_MONSTER
} EntityType;
typedef struct 
{
	enum EntityType type; // РЕСУРС, МОНСТР, АПТЕЧКА
	char symbol; // 'T', 'R', 'M', '+'
	int x, y; // Координаты на карте
	int hp, max_hp; // Прочность ресурса или здоровье зверя
	int attack; // Урон (0 для мирных ресурсов)
	int defense;
	int drop_item_id; // Какой ID предмета из items.txt упадёт в рюкзак
	int is_alive; // 1 -- объект на карте, 0 -- уничтожен/собран
} Entity ;

