#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int fool_check(int scanf_result) //Проверка на дурака
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF); //Чистим буфер

	if (scanf_result != 1) { //Сама проверка
		printf("Не целое число.\n");
		return 0;
	}
	return 1;
}

int main()
{
	SetConsoleCP(65001); //Эта, а также следующая строчка были созданы для грамматной работы консоли(у меня работает только так)
	SetConsoleOutputCP(65001);
	int current_day = 1; //Инициализируем переменные и массив
	int current_hour = 8;
	int inventory[10] = { 0 };
	for (int n = 0; n < 10; n++)
		inventory[n] = n;
	int userAction = -1;
	int add_time = 0;
	do
	{
		printf("[0] Выход \n"); //Выводим меню
		printf("[1] Посмотреть на часы \n");
		printf("[2] Промотать время (Поработать) \n");
		printf("[3] Посмотреть инвентарь \n");
		printf("[4] Положить предмет в слот \n");
		printf("[5] Выбросить предмет \n");
		printf("[6] Уникальные находки \n");
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
			if (!fool_check(scanf("%d", &add_time))) //Запрашиваем время, которое человек желает потратить на работу. Делаем это с проверкой на дурака
			{
				continue;
			}
			{
				current_hour = current_hour + add_time;
				while (current_hour >= 24)
				{
					current_hour = current_hour - 24;
					current_day++;
				}
			}
			break;
		}
		case 3:
		{
			char* item = "";//Создаём литерал
			for (int i = 0; i < 10; i++)
			{
				switch (inventory[i])
				{
					case 0:
						item = "";
						break;
					case 1:
						item = "Дерево";
						break;
					case 2:
						item = "Семена";
						break;
					case 3:
						item = "Ведро с водой";
						break;
					case 4:
						item = "Пустое ведро";
						break;
					case 5:
						item = "Пустое ведро";
						break;
					case 6:
						item = "Морковь";
						break;
					case 7:
						item = "Яйца";
						break;
					case 8:
						item = "Пшеница";
						break;
					case 9:
						item = "Молоко";
						break;
				}
			printf("Слот %d: [%d] ( %s ) \n", i, inventory[i], item);
			}
			break;
		}
		case 4:
		{
			int item_id;//Определяем переменные
			int slot_id;
			printf("Выберите слот, в который хотите положить предмет.\n");
			if (!fool_check(scanf("%d", &slot_id)))//Запрашиваем слот, в который нужно положить предмет.Делаем это с провркой на дурака
			{
				continue;
			}
			printf("Выберите ID предмета, которы хотите положить в этот слот.\n");
			if (!fool_check(scanf("%d", &item_id)))//Запрашиваем ID предмета, который нужно положить в указанный слот.Тоже проверка на дурака
			{
				continue;
			}
			if (item_id >= 0 && item_id < 10 && slot_id >= 0 && slot_id < 10)//Проверяем, не ошибся ли пользователь в ID или номере слота
				inventory[slot_id] = item_id;//Присваиваем элементу значение, указанное пользователем
			else
				printf("Вы ввели неправильное значение слота или ID предмета.\n");
			break;
		}
		case 5:
		{
			int drop_choice;
			printf("Введите слот, предмет из которого нужно выбросить(от 0 до 9).\n");
			if (!fool_check(scanf("%d", &drop_choice)))//Запрашиваем слот, из которого нужно выбросить предметы
			{
				continue;
			}
			if (drop_choice >= 0 && drop_choice < 10)//Проверяем, указал ли пользователь допустимый слот
			{
				inventory[drop_choice] = 0;//Обнуляем этот слот
			}
			else
			{
				printf("Вы ввели неверное значение.\n");
			}
			break;
		}
		case 6:
		{
			int counts[10] = { 0 }; //Создаём массив-счётчик, в который будут записываться повторы
			for (int i = 0; i < 10; i++)
			{
				if (inventory[i] != 0)//Если элемент не пустой(не равен нулю), то считаем
					counts[inventory[i]]++;
			}

			printf("Уникальные находки:\n");
			for (int id = 1; id < 10; id++)
			{
				if (counts[id] > 0)//Выводим, если повторов не ноль.
					printf("ID предмета: %d, количество повторов: %d\n", id, counts[id]);
			}
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