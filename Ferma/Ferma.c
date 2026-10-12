#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define MAX_FARMER_NAME 32
#define MAX_ITEMS 10
#define MAX_NAME_LEN 32

int current_day = 1; //Инициализируем переменные и массив
int current_hour = 8;
int inventory[MAX_ITEMS] = { 0,1,2,3,4,5,6,7,8,9 };
int add_time = 0;
char farmer_name[MAX_FARMER_NAME];
char item_names[MAX_ITEMS][MAX_NAME_LEN];

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

void naming(void)
{
	printf("Введите имя фермера:");
	scanf("%31s", farmer_name);
}

void read_file(void)
{
	strcpy(item_names[0], "Пусто");
	strcpy(item_names[1], "Дерево");
	strcpy(item_names[2], "Камень");
	strcpy(item_names[3], "Семена");
	strcpy(item_names[4], "Пустое ведро");
	strcpy(item_names[5], "Ведро с водой");
	strcpy(item_names[6], "Морковь");
	strcpy(item_names[7], "Яйца");
	strcpy(item_names[8], "Пшеница");
	strcpy(item_names[9], "Молоко");
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

void read_inventory(void)
{
	for (int i = 0; i < MAX_ITEMS; i++)
	{
		int id = inventory[i];
		if (id >= 0 && id < MAX_ITEMS)
		{
			printf("Слот %d: [%d] - %s\n", i, id, item_names[id]);
		}
		else
		{
			printf("Слот %d: [%d] - Неизвестный предмет\n", i, id);
		}
	}
}

void compare_items(void)
{
	printf("Введите название предмета, который хотите найти: ");
}

void time_printer(void)
{
	printf("Текущее время: %d день %d час\n", current_day, current_hour);
}

void time_winder(void)
{
	printf("Сколько времени вы хотите потратить на работу?\n");
	if (!fool_check(scanf("%d", &add_time))) //Запрашиваем время, которое человек желает потратить на работу. Делаем это с проверкой на дурака
	{
		return 0;
	}
	if (add_time > 0)
	{
		current_hour = current_hour + add_time;
		while (current_hour >= 24)
		{
			current_hour = current_hour - 24;
			current_day++;
		}
	}
	else
		printf("Вы ввели число меньше нуля\n");
}

void item_place(void)
{
	int item_id;//Определяем переменные
	int slot_id;
	printf("Выберите слот, в который хотите положить предмет.\n");
	if (!fool_check(scanf("%d", &slot_id)))//Запрашиваем слот, в который нужно положить предмет.Делаем это с провркой на дурака
	{
		return 0;
	}
	printf("Выберите ID предмета, которы хотите положить в этот слот.\n");
	if (!fool_check(scanf("%d", &item_id)))//Запрашиваем ID предмета, который нужно положить в указанный слот.Тоже проверка на дурака
	{
		return 0;
	}
	if (item_id >= 0 && item_id < 10 && slot_id >= 0 && slot_id < 10)//Проверяем, не ошибся ли пользователь в ID или номере слота
		inventory[slot_id] = item_id;//Присваиваем элементу значение, указанное пользователем
	else
		printf("Вы ввели неправильное значение слота или ID предмета.\n");
}

void item_drop(void)
{
	int drop_choice;
	printf("Введите слот, предмет из которого нужно выбросить(от 0 до 9).\n");
	if (!fool_check(scanf("%d", &drop_choice)))//Запрашиваем слот, из которого нужно выбросить предметы
	{
		return 0;
	}
	if (drop_choice >= 0 && drop_choice < 10)//Проверяем, указал ли пользователь допустимый слот
	{
		inventory[drop_choice] = 0;//Обнуляем этот слот
	}
	else
	{
		printf("Вы ввели неверное значение.\n");
	}
}

void unique_finder(void)
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
}
int main()
{
	SetConsoleCP(65001); //Эта, а также следующая строчка были созданы для грамматной работы консоли(у меня работает только так)
	SetConsoleOutputCP(65001);
	read_file();
	naming();
	int userAction = -1;
	do
	{
		printf("Привет, %s !\n", farmer_name); //Выводим имя фермера
		printf("[0] Выход \n"); //Выводим меню
		printf("[1] Посмотреть на часы \n");
		printf("[2] Промотать время (Поработать) \n");
		printf("[3] Посмотреть инвентарь \n");
		printf("[4] Положить предмет в слот \n");
		printf("[5] Выбросить предмет \n");
		printf("[6] Уникальные находки \n");
		printf("[7] Поиск предмета в рюкзаке по названию \n");
		printf("[8] Записать состояние в дневник фермера (diary.txt) \n");
		printf("[9] Распаковщик карты \n");
		if (!fool_check(scanf("%d", &userAction))) //Запрашиваем пункт меню с проверкой на дурака
		{
			continue;
		}
		switch (userAction)
		{
		case 1:
		{
			time_printer();
			break;
		}
		case 2:
		{
			time_winder();
			break;
		}
		case 3:
		{
			read_inventory();
			break;
		}
		case 4:
		{
			item_place();
			break;
		}
		case 5:
		{
			item_drop();
			break;
		}
		case 6:
		{
			unique_finder();
			break;
		}
		case 7:
		{
			break;
		}
		case 8:
		{
			break;
		}
		case 9:
		{
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