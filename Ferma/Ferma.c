#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "map.h"
#include "utils.h"
#include "player.h"

int main()
{
	SetConsoleCP(65001); //Эта, а также следующая строчка были созданы для грамматной работы консоли(у меня работает только так)
	SetConsoleOutputCP(65001);
	srand((unsigned)time(NULL)); //Нужен для будующей генерации карты(сид зависит от времени)
	read_file();
	Player* my_player = NULL;
	int userAction = -1;
	do
	{
		printf("Привет, странник! \n");
		printf("[0] Выход \n"); //Выводим меню
		printf("[1] Управление временем \n");
		printf("[2] Создать персонажа \n");
		printf("[3] Карточка персонажа \n");
		printf("[4] Положить предмет в слот \n");
		printf("[5] Выбросить предмет \n");
		printf("[6] Уникальные находки \n");
		printf("[7] Поиск предмета в рюкзаке по названию \n");
		printf("[8] Записать состояние в дневник фермера (diary.txt) \n");
		printf("[9] Распаковщик карты \n");
		printf("[10] Отправиться в приключение \n");
		if (!fool_check(scanf("%d", &userAction))) //Запрашиваем пункт меню с проверкой на дурака
		{
			continue;
		}
		switch (userAction)
		{
		case 1:
		{
			handle_time_menu(&current_day, &current_hour, my_player);
			break;
		}
		case 2:
		{
			handle_create_player(&my_player);
			break;
		}
		case 3:
		{
			if (my_player == NULL) printf("Сначала создайте персонажа!\n");
			else print_player_card(my_player, current_day, current_hour, item_names);
			break;
		}
		case 4:
		{
			item_place(my_player);
			break;
		}
		case 5:
		{
			item_drop(my_player);
			break;
		}
		case 6:
		{
			unique_finder(my_player);
			break;
		}
		case 7:
		{
			compare_items(my_player);
			break;
		}
		case 8:
		{
			diary_writer(my_player);
			break;
		}
		case 9:
		{
			map_unpacker();
			break;
		}
		case 10:
		{
			start_adventure(my_player, &current_day, &current_hour);
			break;
		}
		case 0:
			break;
		default:
			printf("Неверный ввод\n");
		}
	} while (userAction != 0); // Условие для do-while
	printf("Пока-пока!");
}