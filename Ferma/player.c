#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "player.h"
#include "utils.h"
#define MAX_FARMER_NAME 32
#define MAX_ITEMS 10
#define MAX_NAME_LEN 32

Profession profession_choice(void)
{
    int tmp;
    Profession prof = FARMER;
    for (;;) {
        printf("Введите номер специальности/профессии, за которую хотите играть:\n");
        printf("[1] Фермер\n[2] Лесоруб\n[3] Шахтёр\n[4] Разведчик\n");
        if (scanf("%d", &tmp) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Некорректный ввод.\n");
            continue;
        }
        if (tmp >= 1 && tmp <= 4) {
            prof = (Profession)(tmp - 1);
            break;
        }
        printf("Нужно число от 1 до 4.\n");
    }
    return prof;
}

void create_player(Player** p) 
{
    *p = (Player*)malloc(sizeof(Player)); //выделяем память под структуру 
    if (*p == NULL) 
    {
        printf("Ошибка: не удалось выделить память под игрока\n");
        return;
    }
    printf("Введите имя фермера: ");
    fgets((*p)->name, MAX_FARMER_NAME, stdin);
    for (int i = 0; (*p)->name[i] != '\0'; i++) 
    {
        if ((*p)->name[i] == '\n') {
            (*p)->name[i] = '\0';
            break;
        }
    }
    Profession prof = profession_choice();

    switch (prof)
    {
    case FARMER:
    {
        (*p)->cls = (Profession_desc)
        {
            .bonus_hp = 20,
            .bonus_stamina = 10,
            .farmer_bonus = 1,
            .lumberjack_bonus = 0,
            .miner_bonus = 0,
            .scout_bonus = 0
        };
        break;
    }
    case LUMBERJACK:
    {
        (*p)->cls = (Profession_desc)
        {
            .bonus_hp = 10,
            .bonus_stamina = 20,
            .farmer_bonus = 0,
            .lumberjack_bonus = 1,
            .miner_bonus = 0,
            .scout_bonus = 0
        };
        break;
    }
    case MINER:
    {
        (*p)->cls = (Profession_desc)
        {
            .bonus_hp = 15,
            .bonus_stamina = 15,
            .farmer_bonus = 0,
            .lumberjack_bonus = 0,
            .miner_bonus = 1,
            .scout_bonus = 0
        };
        break;
    }
    case SCOUT:
    {
        (*p)->cls = (Profession_desc)
        {
            .bonus_hp = 10,
            .bonus_stamina = 25,
            .farmer_bonus = 0,
            .lumberjack_bonus = 0,
            .miner_bonus = 0,
            .scout_bonus = 1
        };
        break;
    }
    }
    (*p)->max_hp = 100 + (*p)->cls.bonus_hp;
    (*p)->hp = (*p)->max_hp;
    (*p)->max_stamina = 50 + (*p)->cls.bonus_stamina;
    (*p)->stamina = (*p)->max_stamina;
    (*p)->gold = 0;
    int start_inventory[MAX_ITEMS] = { 0,1,2,3,4,5,6,7,8,9 }; // ID предметов
    for (int i = 0; i < MAX_ITEMS; i++) 
    {
        (*p)->inventory[i] = start_inventory[i]; //копируем инвентарь в структуру 
    }
    printf("Персонаж %s успешно создан!\n", (*p)->name);
}
void print_player_card(Player* p, int day, int hour, char item_names[MAX_ITEMS][MAX_NAME_LEN]) {
    if (p == NULL) 
    {
        printf("Ошибка: персонаж не создан!\n");
        return;
    }
    printf("\n=== КАРТОЧКА ПЕРСОНАЖА ===\n");
    printf("Имя: %s\n", p->name);
    printf("Здоровье: %d/%d\n", p->hp, p->max_hp);
    printf("Выносливость: %d/%d\n", p->stamina, p->max_stamina);
    printf("Золото: %d\n", p->gold);
    printf("Текущее время: День %d, %02d:00\n", day, hour);

    printf("Инвентарь:\n");
    for (int i = 0; i < MAX_ITEMS; i++) 
    {
        int id = p->inventory[i];
        printf("  Слот %d: %s\n", i, item_names[id]);
    }
}
void delete_player(Player** p) //принимает адрес указателя на игрока 
{ 
    if (*p != NULL) 
    {
        printf("Память структуры по адресу %p освобождена\n", (void*)*p); //%p-вывод адреса памяти 
        free(*p);
        *p = NULL;
    }
}
void handle_create_player(Player** p) 
{
    if (*p != NULL) // Если персонаж уже есть
    { 
        printf("Персонаж уже существует. Точно удалить и создать нового? (1 - Да, 0 - Нет): ");
        int confirm;
        if (scanf_s("%d", &confirm) == 1 && confirm == 1) {
            delete_player(p);      // Удаляем старого
            clear_buffer();        // Чистим буфер
            create_player(p);      // Создаем нового
        }
        else 
        {
            printf("Создание отменено.\n");
            clear_buffer();
        }
    }
    else // Если персонажа еще нет
    { 
        clear_buffer();
        create_player(p);
    }
}