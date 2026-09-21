#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int main()
{
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	int current_day = 1;
	int current_hour = 8;
	int inventory[10] = { 0,1,2,3,4,5,6,7,8,9 };
	int userAction;
	int stamina = 0;
	int add_time = 0;
	do
	{
		printf("[0] Выход \n");
		printf("[1] Посмотреть на часы \n");
		printf("[2] Промотать время (Поработать) \n");
		printf("[3] Посмотреть инвентарь \n");
		printf("[4] Положить предмет в слот \n");
		printf("[5] Выбросить предмет \n");
		scanf("%d", &userAction);
		switch (userAction)
		{
		case 1:
		{
			printf("Текущее время: %d день %d час\n", current_day, current_hour);
			break;
		}
		case 2:
		{
			printf("Сколько времени вы хотите потратить на работу?\n");
			scanf("%d", &add_time);
			current_hour = current_hour + add_time;
			while (current_hour >= 24)
			{
				current_hour = current_hour - 24;
				current_day++;
			}
			break;
		}
		case 3:
		{
			for (int i = 0; i < 10; i++)
			{
				printf("Слот [%d]: \n", inventory[i]);
			}
			break;
		}
		case 4:
			printf("тут пока ничего нет");
		case 5:
			printf("тут пока ничего нет");
		case 6:
			printf("тут пока ничего нет");
		case 0:
			break;
		default:
			printf("Неверный ввод");
		}
	} while (userAction != 0);
	printf("Пока-пока!");
}