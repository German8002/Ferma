#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int fool_check(int scanf_result) //Проверка на дурака
{
	char c;
	while ((c = getchar()) != '\n' && c != EOF); //Чистим буфер

	if (scanf_result != 1) { //Сама проверка
		printf("Не целое число.\n");
		return 0;
	}
	return 1;
}
int main()
{
	SetConsoleCP(65001); //Эта, а также следующая строчка были созданы для грамматной работы консоли(у меня работает только так))
	SetConsoleOutputCP(65001);
	int current_day = 1; //Инициализируем переменные
	int current_hour = 8;
	int inventory[10] = { 0 };
	int userAction = -1;
	int stamina = 0;
	int add_time = 0;
	do
	{
		printf("[0] Выход \n"); //Выводим меню
		printf("[1] Посмотреть на часы \n");
		printf("[2] Промотать время (Поработать) \n");
		printf("[3] Посмотреть инвентарь \n");
		printf("[4] Положить предмет в слот \n");
		printf("[5] Выбросить предмет \n");
		if (!fool_check(scanf("%d", &userAction))) //Запрашиваем пункт меню с проверкой на дурака
		{
			continue;
		}
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
			if (!fool_check(scanf("%d", &add_time))); //Запрашиваем время, которое человек желает потратить на работу. Делаем это с проверкой на дурака
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
	} while (userAction != 0); // Условие для do-while
	printf("Пока-пока!");
}