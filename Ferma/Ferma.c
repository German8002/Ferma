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
#include "entity.h"

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
		switch ((MenuOption)userAction)
		{
		case MENU_TIME:
		{
			handle_time_menu(&current_day, &current_hour, my_player);
			break;
		}
		case MENU_CREATE:
		{
			handle_create_player(&my_player);
			break;
		}
		case MENU_CARD:
		{
			print_player_card(my_player, current_day, current_hour, item_names);
			break;
		}
		case MENU_ITEM_PLACE:
		{
			item_place(my_player);
			break;
		}
		case MENU_ITEM_DROP:
		{
			item_drop(my_player);
			break;
		}
		case MENU_UNIQUE:
		{
			unique_finder(my_player);
			break;
		}
		case MENU_COMPARE:
		{
			compare_items(my_player);
			break;
		}
		case MENU_DIARY:
		{
			diary_writer(my_player);
			break;
		}
		case MENU_UNPACK:
		{
			map_unpacker();
			break;
		}
		case MENU_ADVENTURE:
		{
			start_adventure(my_player, &current_day, &current_hour);
			break;
		}
		case MENU_EXIT:
			break;
		default:
			printf("Неверный ввод\n");
		}
	} while (userAction != 0); // Условие для do-while
	printf("Пока-пока!");
}