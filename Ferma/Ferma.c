#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
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
	for (;;)//бесконечный цикл for
	{
		printf("Введите имя фермера:");
		fflush(stdout);

		if (fgets(farmer_name, sizeof(farmer_name), stdin) == NULL)
		{
			printf("\nВвод прерван.\n");
			farmer_name[0] = '\0';
			return;
		}

		size_t len = strlen(farmer_name);

		if (len > 0 && farmer_name[len - 1] == '\n') //Убираем перевод строки
		{
			farmer_name[--len] = '\0';
		}
		else
		{
			int c = getchar();
			if (c != '\n' && c != EOF)//Проверка на символ, если последний - не конец строки/перевод - ошибка
			{
				while (c != '\n' && c != EOF) c = getchar();
				printf("Слишком длинное имя. Максимум %d символов.\n", MAX_FARMER_NAME - 1);
				continue;
			}
		}

		if (len == 0)
		{
			printf("Имя не может быть пустым. Попробуйте снова.\n");
			continue;
		}

		return; //Имя корректно: 1-31 символ
	}
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
		if (inventory[i] == found_id)
		{
			printf("%d ", i);
			found = 1;
		}
	}
	if (!found)
	{
		printf("нет");
	}
	printf("\n");
}

void diary_writer(void)
{
	FILE* f = fopen("diary.txt", "a");//открываем файл для дозаписи
	if (f == NULL)
	{
		printf("Не удалось считать/найти файл.");
		return;
	}
	fprintf(f, "Фермер:%s\n", farmer_name);
	fprintf(f, "День: %d, Час: %d\n", current_day, current_hour);
	fprintf(f, "Инвентарь:\n");
	for (int i = 0; i < MAX_ITEMS; i++)
	{
		int id = inventory[i];
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

void time_printer(void)
{
	printf("Текущее время: %d день %d час\n", current_day, current_hour);
}

void time_winder(void)
{
	printf("Сколько времени вы хотите потратить на работу?\n");
	if (!fool_check(scanf("%d", &add_time))) //Запрашиваем время, которое человек желает потратить на работу. Делаем это с проверкой на дурака
	{
		return;
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
		return;
	}
	printf("Выберите ID предмета, которы хотите положить в этот слот.\n");
	if (!fool_check(scanf("%d", &item_id)))//Запрашиваем ID предмета, который нужно положить в указанный слот.Тоже проверка на дурака
	{
		return;
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
		return;
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

void print_map(char** map, int height, int width, int player_y, int player_x) //Значения переменных(и указателей) даются в основной функции
{
	system("cls"); //Очищаем экран перед каждым показом
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (i == player_y && j == player_x)
			{
				printf("@"); //Рисуем героя
			}
			else {
				printf("%c", map[i][j]);
			}
		}
		printf("\n");
	}
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

	mirror_map(map, height, width, vertical); //Зеркалим

	map[y][x] = 'D'; //Лестница вниз под героем
	map[s_y][s_x] = 'S'; //Переключатель

	int u_side = rand() % 2;// Ставим ровно одну U: либо в левой/верхней, либо в правой/нижней половине
	int placed = 0;
	int guard = 0;
	while (!placed && guard < 10000)
	{
		int ry, rx;
		if (vertical)
		{
			if (u_side == 0) { ry = rand() % (height - 2) + 1; rx = rand() % (mid - 1) + 1; }
			else { ry = rand() % (height - 2) + 1; rx = rand() % (width - mid - 2) + mid + 1; }
		}
		else
		{
			if (u_side == 0) { ry = rand() % (mid - 1) + 1; rx = rand() % (width - 2) + 1; }
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

	int old_u_inside = 0;//Запоминаем, была ли U в перестраиваемой половине
	if (vertical)
	{
		for (int i = 1; i < height - 1; i++)
			for (int j = mid + 1; j < width - 1; j++)
				if (map[i][j] == 'U') old_u_inside = 1;
	}
	else
	{
		for (int i = mid + 1; i < height - 1; i++)
			for (int j = 1; j < width - 1; j++)
				if (map[i][j] == 'U') old_u_inside = 1;
	}

	if (vertical)// 1. Убираем разделяющую стену — делаем её проходимой
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

	int u_y = -1, u_x = -1;	// 3. Если U была в перестроенной половине — возвращаем ровно одну
	if (old_u_inside)
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
	else //4.U осталась в старой половине — найдём её координаты
	{
		for (int i = 1; i < height - 1; i++)
			for (int j = 1; j < width - 1; j++)
				if (map[i][j] == 'U') { u_y = i; u_x = j; }
	}

	if (u_y >= 0)//Прокладываем дорогу до U от точки входа в новую половину
	{
		int entry_y, entry_x;
		if (vertical)
		{
			entry_x = mid;
			entry_y = player_y;
			if (entry_y <= 0 || entry_y >= height - 1) entry_y = height / 2;
		}
		else
		{
			entry_y = mid;
			entry_x = player_x;
			if (entry_x <= 0 || entry_x >= width - 1) entry_x = width / 2;
		}

		int u_in_new_half = vertical ? (u_x > mid) : (u_y > mid);
		if (u_in_new_half)
		{
			carve_path(map, entry_y, entry_x, u_y, u_x);
			map[u_y][u_x] = 'U'; //Если U нет, то возвращаем её
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

	generate_map(&map, &height, &width, &player_y, &player_x, &vertical, &s_y, &s_x);

	while (1)
	{
		print_map(map, height, width, player_y, player_x);
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
		}
		else
		{
			player_y = next_y;
			player_x = next_x;
			if (map[player_y][player_x] == 'S') //Наступили на переключатель
			{
				rebuild_right_half(map, height, width, vertical, player_y, player_x);
				map[player_y][player_x] = '.'; //Переключатель сработал (одноразовый)
				system("pause");
			}
		}
	}
}

int main()
{
	SetConsoleCP(65001); //Эта, а также следующая строчка были созданы для грамматной работы консоли(у меня работает только так)
	SetConsoleOutputCP(65001);
	srand((unsigned)time(NULL)); //Нужен для будующей генерации карты(сид зависит от времени)
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
		printf("[10] Отправиться в приключение \n");
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
			compare_items();
			break;
		}
		case 8:
		{
			diary_writer();
			break;
		}
		case 9:
		{
			map_unpacker();
			break;
		}
		case 10:
		{
			start_adventure();
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