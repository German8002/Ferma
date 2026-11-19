#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "map.h"
#include "player.h"
#include "utils.h"

void carve_path(char** map, int y1, int x1, int y2, int x2)//Отдельная вспомогательная функция для прокладывания дороги
{
	int cy = y1, cx = x1;
	while (cx != x2)
	{
		map[cy][cx] = '.';
		cx += (x2 > cx) ? 1 : -1;
	}
	while (cy != y2)
	{
		map[cy][cx] = '.';
		cy += (y2 > cy) ? 1 : -1;
	}
	map[cy][cx] = '.'; //последняя клетка тоже
}

void mirror_map(Map* m, int vertical)
{
	char** map = m->cells;
	int height = m->height;
	int width = m->width;

	if (vertical) //Стена вертикальная
	{
		int mid = width / 2;
		for (int i = 1; i < height - 1; i++)
		{
			for (int j = mid + 1; j < width - 1; j++)
			{
				map[i][j] = map[i][width - 1 - j]; //Зеркалим по горизонтали
			}
		}
	}
	else //Стена горизонтальная
	{
		int mid = height / 2;
		for (int i = mid + 1; i < height - 1; i++)
		{
			for (int j = 1; j < width - 1; j++)
			{
				map[i][j] = map[height - 1 - i][j]; //Зеркалим по вертикали
			}
		}
	}
}

void print_map(const Map* m, int player_y, int player_x, int vertical, int show_mirror)
{
	if (m == NULL || m->cells == NULL) return;
	system("cls");

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	const WORD COLOR_NORMAL = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // обычный серый (7)
	const WORD COLOR_DIM = FOREGROUND_INTENSITY;                                   // тёмно-серый (8)

	int height = m->height;
	int width = m->width;

	int mid = vertical ? width / 2 : height / 2;
	int mirror_y = vertical ? player_y : height - 1 - player_y;
	int mirror_x = vertical ? width - 1 - player_x : player_x;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int in_mirror_half = show_mirror && (vertical ? j > mid : i > mid);

			SetConsoleTextAttribute(h, in_mirror_half ? COLOR_DIM : COLOR_NORMAL);

			if ((i == player_y && j == player_x) ||
				(show_mirror && i == mirror_y && j == mirror_x))
			{
				printf("@");
				continue;
			}

			char cell_to_draw = m->cells[i][j];

			int check_x = j;
			int check_y = i;
			if (in_mirror_half) {
				if (vertical) check_x = width - 1 - j;
				else check_y = height - 1 - i;
			}

			for (int k = 0; k < m->entity_count; k++)
			{
				if (m->entities[k].is_alive && m->entities[k].x == check_x && m->entities[k].y == check_y)
				{
					cell_to_draw = m->entities[k].symbol;
					break;
				}
			}
			printf("%c", cell_to_draw);
		}
		SetConsoleTextAttribute(h, COLOR_NORMAL);
		printf("\n");
	}
	SetConsoleTextAttribute(h, COLOR_NORMAL);
}

void free_map(Map* m)
{
	if (m->cells == NULL) return;

	for (int i = 0; i < m->height; i++)
	{
		free(m->cells[i]);
	}
	free(m->cells);
	m->cells = NULL;

	if (m->entities != NULL)
	{
		free(m->entities);
		m->entities = NULL;
	}
	m->entity_count = 0;

	m->width = 0;
	m->height = 0;
}

// Создаёт новую локацию внутри m. Счётчик m->level не меняется - им управляет вызывающий код.
void generate_map(Map* m, int* out_y, int* out_x, int* out_vertical, int* out_sy, int* out_sx)
{
	int vertical = rand() % 2;
	int height = rand() % 26 + 5;
	int width = rand() % 26 + 5;
	if (vertical) width = (rand() % 12) * 2 + 7;
	else height = (rand() % 12) * 2 + 7;
	int mid = vertical ? width / 2 : height / 2;
	m->height = height;
	m->width = width;
	*out_vertical = vertical;

	char** map = (char**)malloc(height * sizeof(char*));
	for (int i = 0; i < height; i++)
	{
		map[i] = (char*)malloc(width * sizeof(char));
	}
	m->cells = map;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int on_middle = vertical ? (j == mid) : (i == mid);
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1 || on_middle)
			{
				map[i][j] = '#';
			}
			else
			{
				map[i][j] = '.';
			}
		}
	}

	int max_y = vertical ? height - 2 : mid - 1;
	int max_x = vertical ? mid - 1 : width - 2;

	int y = rand() % max_y + 1;
	int x = rand() % max_x + 1;
	int u_y, u_x, s_y, s_x;
	do
	{
		u_y = rand() % max_y + 1;
		u_x = rand() % max_x + 1;
	} while (u_y == y && u_x == x);
	do
	{
		s_y = rand() % max_y + 1;
		s_x = rand() % max_x + 1;
	} while ((s_y == y && s_x == x) || (s_y == u_y && s_x == u_x));

	carve_path(map, y, x, u_y, u_x);
	carve_path(map, y, x, s_y, s_x);

	map[y][x] = 'D';
	map[s_y][s_x] = 'S';

	int placed = 0;
	int guard = 0;
	while (!placed && guard < 10000)
	{
		int ry, rx;
		if (vertical)
		{
			if (rand() % 2 == 0) { ry = rand() % (height - 2) + 1; rx = rand() % (mid - 1) + 1; }
			else { ry = rand() % (height - 2) + 1; rx = rand() % (width - mid - 2) + mid + 1; }
		}
		else
		{
			if (rand() % 2 == 0) { ry = rand() % (mid - 1) + 1; rx = rand() % (width - 2) + 1; }
			else { ry = rand() % (height - mid - 2) + mid + 1; rx = rand() % (width - 2) + 1; }
		}
		if (map[ry][rx] == '.' && !(ry == y && rx == x) && !(ry == s_y && rx == s_x))
		{
			map[ry][rx] = 'U';
			placed = 1;
		}
		guard++;
	}
	if (!placed)
	{
		for (int i = 1; i < height - 1 && !placed; i++)
			for (int j = 1; j < width - 1 && !placed; j++)
				if (map[i][j] == '.' && !(i == y && j == x) && !(i == s_y && j == s_x))
				{
					map[i][j] = 'U';
					placed = 1;
				}
	}

	int half_area = vertical ? (height * mid) : (mid * width);
	int target_entities = half_area / 7;
	int max_entities = height * width; // Выделяем память с огромным запасом на всю карту
	m->entities = (Entity*)malloc(max_entities * sizeof(Entity));
	m->entity_count = 0;

	int spawned = 0;
	int attempts = 0;
	while (spawned < target_entities && attempts < max_entities * 20)
	{
		attempts++;
		int ey = rand() % (height - 2) + 1;
		int ex = rand() % (width - 2) + 1;

		if (vertical) {
			if (ex >= mid) continue;
		}
		else {
			if (ey >= mid) continue;
		}

		if (map[ey][ex] != '.') continue;

		int occupied = 0;
		for (int k = 0; k < m->entity_count; k++) {
			if (m->entities[k].is_alive && m->entities[k].x == ex && m->entities[k].y == ey) {
				occupied = 1;
				break;
			}
		}
		if (occupied) continue;

		int too_close = 0;
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0) continue;
				int check_x = ex + dx;
				int check_y = ey + dy;
				for (int k = 0; k < m->entity_count; k++) {
					if (m->entities[k].is_alive && m->entities[k].x == check_x && m->entities[k].y == check_y) {
						too_close = 1;
						break;
					}
				}
				if (too_close) break;
			}
			if (too_close) break;
		}
		if (too_close) continue;

		Entity new_ent;
		new_ent.x = ex;
		new_ent.y = ey;
		new_ent.is_alive = 1;

		float mult = 1.0f + (m->level - 1) * 0.3f;

		int type_rand = rand() % 100;
		if (type_rand < 40) {
			new_ent.type = ENT_TREE;
			new_ent.symbol = 'T';
			new_ent.hp = new_ent.max_hp = (int)(10 * mult);
			new_ent.attack = 0;
			new_ent.drop_item_id = 1;
		}
		else if (type_rand < 80) {
			new_ent.type = ENT_ROCK;
			new_ent.symbol = 'R';
			new_ent.hp = new_ent.max_hp = (int)(15 * mult);
			new_ent.attack = 0;
			new_ent.drop_item_id = 2;
		}
		else if (type_rand < 90) {
			new_ent.type = ENT_HEAL;
			new_ent.symbol = '+';
			new_ent.hp = new_ent.max_hp = 1;
			new_ent.attack = 0;
			new_ent.drop_item_id = 0;
		}
		else {
			new_ent.type = ENT_MONSTER;
			new_ent.symbol = 'M';
			new_ent.hp = new_ent.max_hp = (int)(20 * mult);
			new_ent.attack = (int)(5 * mult);
			new_ent.drop_item_id = 9;
		}

		m->entities[m->entity_count++] = new_ent;
		spawned++;
	}

	mirror_map(m, vertical);

	*out_y = y;
	*out_x = x;
	*out_sy = s_y;
	*out_sx = s_x;
}

void rebuild_right_half(Map* m, int vertical, int player_y, int player_x)
{
	char** map = m->cells;
	int height = m->height;
	int width = m->width;
	int mid = vertical ? width / 2 : height / 2;

	int u_y = -1, u_x = -1;
	if (vertical)
	{
		for (int i = 1; i < height - 1; i++)
			for (int j = mid + 1; j < width - 1; j++)
				if (map[i][j] == 'U') { u_y = i; u_x = j; }
	}
	else
	{
		for (int i = mid + 1; i < height - 1; i++)
			for (int j = 1; j < width - 1; j++)
				if (map[i][j] == 'U') { u_y = i; u_x = j; }
	}

	if (vertical)
		for (int i = 1; i < height - 1; i++) map[i][mid] = '.';
	else
		for (int j = 1; j < width - 1; j++) map[mid][j] = '.';

	if (vertical)
	{
		for (int i = 1; i < height - 1; i++)
			for (int j = mid + 1; j < width - 1; j++)
			{
				map[i][j] = '.';
			}
	}
	else
	{
		for (int i = mid + 1; i < height - 1; i++)
			for (int j = 1; j < width - 1; j++)
			{
				map[i][j] = '.';
			}
	}

	if (u_y >= 0)
	{
		map[u_y][u_x] = 'U';
	}
	else
	{
		int placed = 0, guard = 0;
		while (!placed && guard < 10000)
		{
			int ry, rx;
			if (vertical)
			{
				ry = rand() % (height - 2) + 1;
				rx = rand() % (width - mid - 2) + mid + 1;
			}
			else
			{
				ry = rand() % (height - mid - 2) + mid + 1;
				rx = rand() % (width - 2) + 1;
			}
			if (map[ry][rx] == '.')
			{
				map[ry][rx] = 'U';
				u_y = ry; u_x = rx;
				placed = 1;
			}
			guard++;
		}
		if (!placed)
		{
			for (int i = 1; i < height - 1 && !placed; i++)
				for (int j = 1; j < width - 1 && !placed; j++)
					if (map[i][j] == '.')
					{
						map[i][j] = 'U';
						u_y = i; u_x = j;
						placed = 1;
					}
		}
	}

	if (u_y >= 0)
	{
		carve_path(map, player_y, player_x, u_y, u_x);
		map[u_y][u_x] = 'U';

		int d_y = -1, d_x = -1;
		for (int i = 1; i < height - 1; i++)
			for (int j = 1; j < width - 1; j++)
				if (map[i][j] == 'D') { d_y = i; d_x = j; }
		if (d_y >= 0)
		{
			carve_path(map, player_y, player_x, d_y, d_x);
			map[d_y][d_x] = 'D';
		}
	}

	for (int k = 0; k < m->entity_count; k++)
	{
		if (m->entities[k].is_alive)
		{
			int in_rebuilt_half = vertical ? (m->entities[k].x > mid) : (m->entities[k].y > mid);
			if (in_rebuilt_half) m->entities[k].is_alive = 0;
		}
	}

	int half_area = vertical ? (height * (width - mid)) : ((height - mid) * width);
	int target_entities = half_area / 7;

	int spawned = 0;
	int attempts = 0;

	while (spawned < target_entities && attempts < target_entities * 20)
	{
		attempts++;
		int ey = rand() % (height - 2) + 1;
		int ex = rand() % (width - 2) + 1;

		if (vertical) {
			if (ex <= mid) continue;
		}
		else {
			if (ey <= mid) continue;
		}

		if (map[ey][ex] != '.') continue;

		int occupied = 0;
		for (int k = 0; k < m->entity_count; k++) {
			if (m->entities[k].is_alive && m->entities[k].x == ex && m->entities[k].y == ey) {
				occupied = 1;
				break;
			}
		}
		if (occupied) continue;

		int too_close = 0;
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx == 0 && dy == 0) continue;
				int check_x = ex + dx;
				int check_y = ey + dy;
				for (int k = 0; k < m->entity_count; k++) {
					if (m->entities[k].is_alive && m->entities[k].x == check_x && m->entities[k].y == check_y) {
						too_close = 1;
						break;
					}
				}
				if (too_close) break;
			}
			if (too_close) break;
		}
		if (too_close) continue;

		Entity new_ent;
		new_ent.x = ex;
		new_ent.y = ey;
		new_ent.is_alive = 1;

		float mult = 1.0f + (m->level - 1) * 0.3f;

		int type_rand = rand() % 100;
		if (type_rand < 40) {
			new_ent.type = ENT_TREE;
			new_ent.symbol = 'T';
			new_ent.hp = new_ent.max_hp = (int)(10 * mult);
			new_ent.attack = 0;
			new_ent.drop_item_id = 1;
		}
		else if (type_rand < 80) {
			new_ent.type = ENT_ROCK;
			new_ent.symbol = 'R';
			new_ent.hp = new_ent.max_hp = (int)(15 * mult);
			new_ent.attack = 0;
			new_ent.drop_item_id = 2;
		}
		else if (type_rand < 90) {
			new_ent.type = ENT_HEAL;
			new_ent.symbol = '+';
			new_ent.hp = new_ent.max_hp = 1;
			new_ent.attack = 0;
			new_ent.drop_item_id = 0;
		}
		else {
			new_ent.type = ENT_MONSTER;
			new_ent.symbol = 'M';
			new_ent.hp = new_ent.max_hp = (int)(20 * mult);
			new_ent.attack = (int)(5 * mult);
			new_ent.drop_item_id = 9;
		}

		m->entities[m->entity_count++] = new_ent;
		spawned++;
	}
}

void start_adventure(Player* p, int* day, int* hour)
{
	if (p == NULL || day == NULL || hour == NULL)
	{
		printf("Сначала создайте персонажа!\n");
		return;
	}

	Map* map = (Map*)malloc(sizeof(Map));
	if (map == NULL) return;
	map->cells = NULL;
	map->width = 0;
	map->height = 0;
	map->level = 1;
	map->entities = NULL;
	map->entity_count = 0;

	p->y = 0;
	p->x = 0;
	int vertical = 0;
	int s_y = 0, s_x = 0;
	int mirror_active = 1;

	generate_map(map, &p->y, &p->x, &vertical, &s_y, &s_x);

	int event_msg = 0;
	char message[256] = "";

	while (1)
	{
		print_map(map, p->y, p->x, vertical, mirror_active);
		printf("=== Этаж: %d ===\n", map->level);
		printf("HP: %d/%d | Выносливость: %d/%d | Золото: %d | Атака: %d | Защита: %d\n",
			p->hp, p->max_hp, p->stamina, p->max_stamina, p->gold, p->attack, p->defense);

		if (event_msg) {
			printf("\n[СОБЫТИЕ]: %s\n", message);
			event_msg = 0;
		}
		printf("\nУправление: W-вверх, A-влево, S-вниз, D-вправо, Q-вернуться домой (только на лестнице D)\n");

		printf("Ваш ход: ");
		char choice;
		if (scanf(" %c", &choice) != 1)
		{
			printf("Память структуры по адресу %p освобождена\n", (void*)map);
			free_map(map);
			free(map);
			break;
		}
		clear_buffer();

		if (choice == 'q' || choice == 'Q')
		{
			if (map->cells[p->y][p->x] == 'D')
			{
				printf("Память структуры по адресу %p освобождена\n", (void*)map);
				free_map(map);
				free(map);
				break;
			}
			printf("\nВыход домой возможен только на лестнице вниз (D)!\n");
			system("pause");
			continue;
		}

		int next_y = p->y;
		int next_x = p->x;
		if (choice == 'w' || choice == 'W') next_y--;
		else if (choice == 's' || choice == 'S') next_y++;
		else if (choice == 'a' || choice == 'A') next_x--;
		else if (choice == 'd' || choice == 'D') next_x++;

		if (next_y < 0 || next_y >= map->height || next_x < 0 || next_x >= map->width)
		{
			printf("\nВы не можете выйти за пределы карты!\n");
			system("pause");
			continue;
		}

		char target = map->cells[next_y][next_x];

		int ent_index = -1;
		for (int k = 0; k < map->entity_count; k++)
		{
			if (map->entities[k].is_alive)
			{
				int check_x = next_x;
				int check_y = next_y;
				int in_mirror_half = mirror_active && (vertical ? next_x > (map->width / 2) : next_y > (map->height / 2));
				if (in_mirror_half) {
					if (vertical) check_x = map->width - 1 - next_x;
					else check_y = map->height - 1 - next_y;
				}

				if (map->entities[k].x == check_x && map->entities[k].y == check_y)
				{
					ent_index = k;
					break;
				}
			}
		}

		if (target == '#')
		{
			sprintf(message, "Там стена!");
			event_msg = 1;
		}
		else if (ent_index != -1)
		{
			Entity* e = &map->entities[ent_index];

			int damage_to_ent = p->attack;
			e->hp -= damage_to_ent;
			sprintf(message, "Вы ударили %c на %d урона! (Осталось HP: %d)", e->symbol, damage_to_ent, e->hp > 0 ? e->hp : 0);

			if (e->hp <= 0)
			{
				e->is_alive = 0;
				if (e->type == ENT_MONSTER)
				{
					p->gold += BASE_GOLD_REWARD;
					sprintf(message, "Монстр побеждён! +%d золота.", BASE_GOLD_REWARD);
				}
				else if (e->type == ENT_HEAL)
				{
					p->hp += 20;
					if (p->hp > p->max_hp) p->hp = p->max_hp;
					sprintf(message, "Вы подобрали аптечку! +20 HP.");
				}
				else
				{
					int added = 0;
					for (int i = 0; i < MAX_ITEMS; i++)
					{
						if (p->inventory[i] == 0)
						{
							p->inventory[i] = e->drop_item_id;
							added = 1;
							break;
						}
					}
					if (added) sprintf(message, "Ресурс собран! Добавлен в инвентарь.");
					else sprintf(message, "Инвентарь полон! Ресурс потерян.");
				}
			}

			if (e->is_alive && e->type == ENT_MONSTER)
			{
				int damage_to_player = e->attack - p->defense;
				if (damage_to_player < 1) damage_to_player = 1;
				p->hp -= damage_to_player;
				sprintf(message + strlen(message), " Монстр бьёт в ответ на %d!", damage_to_player);
			}

			event_msg = 1;
			p->stamina -= 1;
			if (p->stamina < 0) p->stamina = 0;
		}
		else
		{
			p->y = next_y;
			p->x = next_x;

			int stamina_cost = 1;
			if (p->cls.scout_bonus && (rand() % 100) < 50) stamina_cost = 0;
			if (p->cls.farmer_bonus && (rand() % 100) < 30) stamina_cost = -1;
			p->stamina -= stamina_cost;
			if (p->stamina < 0) p->stamina = 0;
			if (p->stamina > p->max_stamina) p->stamina = p->max_stamina;

			if (target == 'S')
			{
				mirror_active = 0;
				rebuild_right_half(map, vertical, p->y, p->x);
				map->cells[p->y][p->x] = '.';
				sprintf(message, "Вы наступили на переключатель! Зеркало разбилось.");
				event_msg = 1;
			}
			else if (target == 'U')
			{
				int next_level = map->level + 1;
				printf("Память структуры по адресу %p освобождена\n", (void*)map->cells);
				free_map(map);

				generate_map(map, &p->y, &p->x, &vertical, &s_y, &s_x);
				map->level = next_level;
				mirror_active = 1;

				p->stamina -= 5;
				if (p->stamina < 0) p->stamina = 0;
				*hour += 1;
				if (*hour >= 24) { *hour = 0; *day += 1; }

				int reward = BASE_GOLD_REWARD * map->level;
				if (p->cls.miner_bonus)      reward += reward / 2;
				if (p->cls.lumberjack_bonus) reward += 2;
				p->gold += reward;

				sprintf(message, "Вы поднялись на %d этаж! Награда: %d золота.", map->level, reward);
				event_msg = 1;
			}
		}

		if (p->hp <= 0)
		{
			system("cls");
			printf("\nВы погибли! Игра окончена.\n");
			printf("Память структуры по адресу %p освобождена\n", (void*)map);
			free_map(map);
			free(map);
			system("pause");
			break;
		}

		if (p->stamina <= 0 || *hour == 2)
		{
			system("cls");
			if (p->stamina <= 0)
				printf("\nВы упали без сил! Спасательная служба доставляет вас домой.\n");
			else
				printf("\nНаступило 02:00 ночи! Фермер падает без сил.\n");

			int lost = p->gold / 2;
			p->gold -= lost;
			printf("За услуги забрали %d золота. Осталось %d.\n", lost, p->gold);

			printf("Память структуры по адресу %p освобождена\n", (void*)map);
			free_map(map);
			free(map);
			system("pause");
			break;
		}
	}
}