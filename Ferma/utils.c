#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "player.h"

int current_day = 1;
int current_hour = 8;
char item_names[MAX_ITEMS][MAX_NAME_LEN];

void clear_buffer(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF); //очистка 
}

int fool_check(int scanf_result) //Проверка на дурака
{
	if (scanf_result != 1) { //Сама проверка
		printf("Не целое число.\n");
		clear_buffer();
		return 0;
	}
	clear_buffer();
	return 1;
}

static void show_time(int day, int hour)
{
	printf("Текущее время: День %d, %02d:00\n", day, hour);
}

static void wait_time(int* day, int* hour)
{
	int add_hours;
	if (*hour >= 2 && *hour < 8)
	{
		printf("Ночью фермер должен спать!\n");
		return;
	}
	printf("Сколько часов подождать?: ");
	if (scanf("%d", &add_hours) != 1)
	{
		printf("Неверный ввод\n");
		clear_buffer();
		return;
	}
	clear_buffer();

	if (add_hours < 1) {
		printf("Ошибка: нельзя отработать отрицательное количество времени\n");
		return;
	}

	for (int i = 1; i <= add_hours; i++)
	{
		int h = (*hour + i) % 24;
		if (h >= 2 && h < 8)
		{
			printf("Нельзя проматывать время через 02:00 ночи! Фермер должен спать.\n");
			return;
		}
	}

	int total_hours = *hour + add_hours;
	*day = *day + (total_hours / 24);
	*hour = total_hours % 24;
	printf("Текущее время: День %d, %02d:00\n", *day, *hour);
}

static void sleep_time(int* day, int* hour, Player* p)
{
	printf("Фермер лег спать\n");
	*day += 1;
	*hour = 8; // Просыпаемся в 8 утра
	if (p != NULL)
	{
		p->hp = p->max_hp;
		p->stamina = p->max_stamina;
		printf("Здоровье и выносливость восстановлены!\n");
	}
	printf("Наступило утро: День %d, 08:00\n", *day);
}

void handle_time_menu(int* day, int* hour, Player* p) {
	int choice;
	do
	{
		printf("\n=== УПРАВЛЕНИЕ ВРЕМЕНЕМ ===\n");
		printf("1. Посмотреть на часы\n");
		printf("2. Подождать (промотать время)\n");
		printf("3. Лечь спать (до утра)\n");
		printf("0. Назад в главное меню\n");
		printf("Ваш выбор: ");

		if (scanf("%d", &choice) != 1)
		{
			printf("Неверный ввод\n");
			clear_buffer();
			continue;
		}
		clear_buffer();

		switch (choice)
		{
		case 1:
		{
			show_time(*day, *hour);
			break;
		}
		case 2:
		{
			wait_time(day, hour);
			break;
		}
		case 3:
		{
			sleep_time(day, hour, p);
			break;
		}
		case 0:
		{
			break;
		}
		default:
		{
			printf("Неверный пункт меню\n");
		}
		}
	} while (choice != 0);
}

int can_go_adventure(Player* p, int current_hour) //проверка, может ли герой отправиться в приключение 
{
	if (p == NULL) {
		printf("Сначала создайте персонажа!\n");
		return 0;
	}
	if (p->stamina <= 0) {
		printf("У вас нет сил! Выносливость на нуле. Идите спать.\n");
		return 0;
	}
	if (current_hour >= 2 && current_hour < 8) {
		printf("На улице ночь (после 02:00)! Фермер должен спать.\n");
		return 0;
	}
	return 1; // Все проверки пройдены
}

void read_file(void)
{
	strcpy(item_names[0], "Пусто");
	strcpy(item_names[1], "Дерево");
	strcpy(item_names[2], "Камень");
	strcpy(item_names[3], "Семена");
	strcpy(item_names[4], "Ведро");
	strcpy(item_names[5], "Картошка");
	strcpy(item_names[6], "Морковь");
	strcpy(item_names[7], "Яйца");
	strcpy(item_names[8], "Пшеница");
	strcpy(item_names[9], "Молоко");
	strcpy(item_names[10], "Клык");
	FILE* f = fopen("items.txt", "r");
	if (f == NULL)
	{
		printf("Не удалось считать/найти items.txt\n");
		return;
	}
	{
		int id;
		char name[MAX_NAME_LEN];
		while (fscanf(f, "%d %31s", &id, name) == 2)
		{
			if (id >= 0 && id < MAX_ITEMS)
			{
				strcpy(item_names[id], name);
			}
		}
		fclose(f);
	}
}

void diary_writer(Player* p)
{
	FILE* f = fopen("diary.txt", "a");//открываем файл для дозаписи
	if (f == NULL)
	{
		printf("Не удалось считать/найти файл.");
		return;
	}
	fprintf(f, "Фермер:%s\n", p->name);
	fprintf(f, "День: %d, Час: %d\n", current_day, current_hour);
	fprintf(f, "Инвентарь:\n");
	for (int i = 0; i < MAX_ITEMS; i++)
	{
		int id = p->inventory[i];
		if (id >= 0 && id < MAX_ITEMS)
		{
			fprintf(f, "Слот %d: [%d] - %s\n", i, id, item_names[id]);
		}
		else
		{
			fprintf(f, "Слот %d: [%d] - Неизвестный предмет\n", i, id);
		}
	}
	fclose(f);
}

void map_unpacker(void)
{
	FILE* in = fopen("input.txt", "r");
	if (!in) { printf("Нет input.txt\n"); return; }

	int cap = 1024, size = 0;//Динамический буфер для карты
	char* map = (char*)malloc(cap);

	int c, num = 0, has_num = 0;//Переменные для RLE-распаковки

	int len = 0, expected = -1, line = 1, error = 0;//Переменные для проверки геометрии

	while ((c = fgetc(in)) != EOF)//Читаем и распаковываем весь файл
	{
		if (isdigit(c)) {//Определяем, если число
			num = num * 10 + (c - '0');//Считам десятки, сотни и т.д.
			has_num = 1;//Запмсываем, что это число
		}
		else if (c == '\n') {// Проверка длины строки
			if (expected < 0) expected = len;
			else if (len != expected) { error = 1; break; }

			if (size + 1 >= cap) { cap *= 2; map = (char*)realloc(map, cap); }// Сохраняем перевод строки
			map[size++] = '\n';

			len = 0; num = 0; has_num = 0; line++;// Сброс строки
		}
		else {
			int reps = has_num ? num : 1;// Повторяем символ (если числа не было — 1 раз)
			for (int i = 0; i < reps; i++) {
				if (size + 1 >= cap) { cap *= 2; map = (char*)realloc(map, cap); }
				map[size++] = (char)c;
				len++;
			}
			num = 0; has_num = 0;//обнуляем число
		}
	}

	if (!error && len > 0) {//Проверка последней строки, если она без '\n'
		if (expected < 0) expected = len;
		else if (len != expected) error = 1;
		else {
			if (size + 1 >= cap) { cap *= 2; map = (char*)realloc(map, cap); }//Расширяем массив, если размер не выходит
			map[size++] = '\n';
		}
	}

	map[size] = '\0'; // Завершаем строку для вывода
	fclose(in);

	FILE* out = fopen("output.txt", "w");// Вывод результата
	if (out) {
		if (error) {
			printf("Ошибка в строке %d: длина %d, ожидалось %d\n", line, len, expected);
			fprintf(out, "Ошибка в строке %d: длина %d, ожидалось %d\n", line, len, expected);
		}
		else {
			printf("%s", map);
			fprintf(out, "%s", map);
			printf("Готово. Результат в output.txt\n");
		}
		fclose(out);
	}
	free(map);
}

void compare_items(Player* p)
{
	if (p == NULL)
	{
		printf("Сначала создайте персонажа!\n");
		return;
	}

	char item_to_compare[MAX_NAME_LEN];
	printf("Введите название предмета, который хотите найти: ");
	fgets(item_to_compare, sizeof(item_to_compare), stdin);
	item_to_compare[strcspn(item_to_compare, "\n")] = '\0';

	int found_id = -1;
	for (int id = 0; id < MAX_ITEMS; id++)
	{
		if (strcmp(item_names[id], item_to_compare) == 0)
		{
			found_id = id;
			break;
		}
	}

	if (found_id == -1)
	{
		printf("Предмет с названием \"%s\" не найден.\n", item_to_compare);
		return;
	}

	int found = 0;
	printf("Предмет \"%s\" (ID: %d) найден в слотах: ", item_names[found_id], found_id);
	for (int i = 0; i < MAX_ITEMS; i++)
	{
		if (p->inventory[i] == found_id)
		{
			printf("%d ", i);
			found = 1;
		}
	}
	if (!found)
		printf("нет");
	printf("\n");
}