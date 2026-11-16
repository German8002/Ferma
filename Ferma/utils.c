#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "player.h"
#include "utils.h"

void clear_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF); //очистка 
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
    if (scanf_s("%d", &add_hours) != 1)
    {
        printf("Неверный ввод\n");
        clear_buffer();
        return;
    }

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
        }
        case 2:
        {
            wait_time(day, hour);
        }
        case 3:
        {
            sleep_time(day, hour, p);
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
    }while (choice != 0);
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