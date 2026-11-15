#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "map.h"

#define BASE_GOLD_REWARD 10

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

void mirror_map(char** map, int height, int width, int vertical)
{
	if (vertical) //Стена вертикальная
	{
		int mid = width / 2;
		for (int i = 1; i < height - 1; i++)
		{
			for (int j = mid + 1; j < width - 1; j++)
			{
				map[i][j] = map[i][width - 1 - j]; //Зергалим по горизонатели
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

void print_map(char** map, int height, int width, int player_y, int player_x, int vertical, int show_mirror)
{
	system("cls");

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	const WORD COLOR_NORMAL = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // обычный серый (7)
	const WORD COLOR_DIM = FOREGROUND_INTENSITY;                                   // тёмно-серый (8)

	int mid = vertical ? width / 2 : height / 2;
	int mirror_y = vertical ? player_y : height - 1 - player_y;
	int mirror_x = vertical ? width - 1 - player_x : player_x;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int in_mirror_half = show_mirror && (vertical ? j > mid : i > mid);// Клетка за стеной (отзеркаленная половина), пока двойник активен

			SetConsoleTextAttribute(h, in_mirror_half ? COLOR_DIM : COLOR_NORMAL);

			if ((i == player_y && j == player_x) ||
				(show_mirror && i == mirror_y && j == mirror_x))
			{
				printf("@");
			}
			else {
				printf("%c", map[i][j]);
			}
		}
		SetConsoleTextAttribute(h, COLOR_NORMAL);
		printf("\n");
	}
	SetConsoleTextAttribute(h, COLOR_NORMAL); // возвращаем обычный цвет для остального текста
}

void free_map(char** map, int height)
{
	for (int i = 0; i < height; i++)
	{
		free(map[i]); //Освобождаем каждую строку
	}
	free(map); //Освобождаем список адресов строк
}

void generate_map(char*** map_ptr, int* out_height, int* out_width, int* out_y, int* out_x, int* out_vertical, int* out_sy, int* out_sx)
{
	int vertical = rand() % 2; //1 - стена вертикальная, 0 - горизонтальная
	int height = rand() % 26 + 5; //от 5 до 30(Высота)
	int width = rand() % 26 + 5; //Длина
	if (vertical) width = (rand() % 12) * 2 + 7; //Делаем длину нечётной для нормального отзеркаливания
	else height = (rand() % 12) * 2 + 7; //Делаем высоту нечётной для нормального отзеркаливания
	int mid = vertical ? width / 2 : height / 2; //Номер строчки или столба, по которому идёт сама стена(для зеркала)
	*out_height = height;
	*out_width = width;
	*out_vertical = vertical;

	char** map = (char**)malloc(height * sizeof(char*)); //Память под список строк
	for (int i = 0; i < height; i++)
	{
		map[i] = (char*)malloc(width * sizeof(char)); //Память под каждую строку
	}
	*map_ptr = map;

	for (int i = 0; i < height; i++)// Наполнение карты
	{
		for (int j = 0; j < width; j++)
		{
			int on_middle = vertical ? (j == mid) : (i == mid);
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1 || on_middle)//Выставляем стены
			{
				map[i][j] = '#';
			}
			else
			{
				int chance = rand() % 100;
				if (chance < 15) map[i][j] = 'T'; //15% дерево(Откуда оно в башне?Ну, я не знаю)
				else if (chance < 25) map[i][j] = 'R'; //10% камень
				else map[i][j] = '.';
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

	carve_path(map, y, x, u_y, u_x);//Прокладываем дорогу для U
	carve_path(map, y, x, s_y, s_x);//Прокладываем дорогу для S

	map[y][x] = 'D'; //Лестница вниз под героем — до зеркала
	map[s_y][s_x] = 'S'; //Переключатель — до зеркала

	int placed = 0;//U — в случайную половину, до зеркала (если слева — отзеркалится вправо)
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
	if (!placed)//Подстраховка: если не нашли место — берём любую свободную клетку
	{
		for (int i = 1; i < height - 1 && !placed; i++)
			for (int j = 1; j < width - 1 && !placed; j++)
				if (map[i][j] == '.' && !(i == y && j == x) && !(i == s_y && j == s_x))
				{
					map[i][j] = 'U';
					placed = 1;
				}
	}

	mirror_map(map, height, width, vertical); //Зеркалим — если U была слева, она появится и справа

	*out_y = y;
	*out_x = x;
	*out_sy = s_y;
	*out_sx = s_x;
}

void clear_buffer(void)//Функция очищения буфера, нужна будет для 10 пункта
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

void rebuild_right_half(char** map, int height, int width, int vertical,
	int player_y, int player_x)
{
	int mid = vertical ? width / 2 : height / 2;

	int u_y = -1, u_x = -1;//Запоминаем, была ли U в перестраиваемой половине и где именно
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

	if (vertical)// Убираем разделяющую стену — делаем её проходимой
		for (int i = 1; i < height - 1; i++) map[i][mid] = '.';
	else
		for (int j = 1; j < width - 1; j++) map[mid][j] = '.';

	if (vertical) //Перестраиваем половину
	{
		for (int i = 1; i < height - 1; i++)
			for (int j = mid + 1; j < width - 1; j++)
			{
				int chance = rand() % 100;
				if (chance < 15) map[i][j] = 'T';
				else if (chance < 25) map[i][j] = 'R';
				else map[i][j] = '.';
			}
	}
	else
	{
		for (int i = mid + 1; i < height - 1; i++)
			for (int j = 1; j < width - 1; j++)
			{
				int chance = rand() % 100;
				if (chance < 15) map[i][j] = 'T';
				else if (chance < 25) map[i][j] = 'R';
				else map[i][j] = '.';
			}
	}

	if (u_y >= 0)// Если U уже была справа — возвращаем её на место, новую не ставим
	{
		map[u_y][u_x] = 'U';
	}
	else// U справа не было — создаём новую
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

	if (u_y >= 0)//Прокладываем дорогу до U и до D от игрока, чтобы не запереть
	{
		carve_path(map, player_y, player_x, u_y, u_x);
		map[u_y][u_x] = 'U'; //Если U нет, то возвращаем её

		int d_y = -1, d_x = -1;//Ищем D, чтобы прорубить дорогу и до него
		for (int i = 1; i < height - 1; i++)
			for (int j = 1; j < width - 1; j++)
				if (map[i][j] == 'D') { d_y = i; d_x = j; }
		if (d_y >= 0)
		{
			carve_path(map, player_y, player_x, d_y, d_x);
			map[d_y][d_x] = 'D';
		}
	}
}

void start_adventure(void)
{
	char** map = NULL;
	int height = 0, width = 0;
	int player_y = 0, player_x = 0;
	int vertical = 0;
	int level = 1;
	int s_y = 0, s_x = 0;
	int mirror_active = 1;

	generate_map(&map, &height, &width, &player_y, &player_x, &vertical, &s_y, &s_x);

	while (1)
	{
		print_map(map, height, width, player_y, player_x, vertical, mirror_active);
		printf("=== Этаж: %d ===\n", level);
		printf("\nУправление: W-вверх, A-влево, S-вниз, D-вправо, Q-вернуться домой (только на лестнице D)\n");

		printf("Ваш ход: ");
		char choice;
		if (scanf(" %c", &choice) != 1) //Ввод закрыт — выходим, чтобы не зациклиться
		{
			free_map(map, height);
			break;
		}
		clear_buffer(); //Убираем Enter и всё, что напечатали после первой буквы

		if (choice == 'q' || choice == 'Q')
		{
			if (map[player_y][player_x] == 'D') //Выход из башни
			{
				free_map(map, height);
				break;
			}
			printf("\nВыход домой возможен только на лестнице вниз (D)!\n");
			system("pause");
			continue;
		}

		int next_y = player_y;
		int next_x = player_x;
		if (choice == 'w' || choice == 'W') next_y--;
		else if (choice == 's' || choice == 'S') next_y++;
		else if (choice == 'a' || choice == 'A') next_x--;
		else if (choice == 'd' || choice == 'D') next_x++;

		char target = map[next_y][next_x]; //Ограничение по препятствиям
		if (target == '#' || target == 'T' || target == 'R')
		{
			printf("\nТам препятствие! Путь заблокирован\n");
			system("pause");
		}
		else if (target == 'U') //Лестница наверх
		{
			free_map(map, height);
			level++;
			generate_map(&map, &height, &width, &player_y, &player_x, &vertical, &s_y, &s_x);
			mirror_active = 1;
		}
		else
		{
			player_y = next_y;
			player_x = next_x;
			if (map[player_y][player_x] == 'S') //Наступили на переключатель
			{
				mirror_active = 0;
				rebuild_right_half(map, height, width, vertical, player_y, player_x);
				map[player_y][player_x] = '.'; //Переключатель сработал (одноразовый)
				system("pause");
			}
		}
	}
}