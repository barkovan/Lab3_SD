#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "map.h"

void print_menu() {
    printf("\n========================================\n");
    printf("   Управление Ассоциативным Массивом    \n");
    printf("========================================\n");
    printf("1. Создать/Переключиться на BST-дерево\n");
    printf("2. Создать/Переключиться на B-дерево\n");
    printf("3. Вставить элемент\n");
    printf("4. Найти элемент\n");
    printf("5. Удалить элемент\n");
    printf("6. Вывести структуру дерева\n");
    printf("7. Вывести дерево (по возрастанию)");
    printf("0. Выход\n");
    printf("========================================\n");
    printf("Ваш выбор: ");
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    Map* current_map = NULL;
    int choice;
    long long key;
    char value[256];
    int t_param;

    while (1) {
        print_menu();

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("[Ошибка] Введите число!\n");
            continue;
        }

        switch (choice) {
        case 1:
            if (current_map != NULL) {
                map_destroy(current_map);
                printf("[Info] Память от предыдущего дерева освобождена.\n");
            }
            current_map = create_map(BST_TYPE, 0);
            printf("[OK] AVL-дерево успешно создано и готово к работе!\n");
            break;

        case 2:
            printf("Введите параметр t (минимальная степень) для B-дерева: ");
            if (scanf("%d", &t_param) != 1 || t_param < 2) {
                printf("[Ошибка] Некорректный параметр t. Должно быть >= 2.\n");
                while (getchar() != '\n');
                break;
            }
            if (current_map != NULL) {
                map_destroy(current_map);
                printf("[Info] Память от предыдущего дерева освобождена.\n");
            }
            current_map = create_map(BTREE_TYPE, t_param);
            printf("[OK] B-дерево (t=%d) успешно создано!\n", t_param);
            break;

        case 3:
            if (!current_map) {
                printf("[Ошибка] Сначала создайте дерево (Пункт 1 или 2)!\n");
                break;
            }
            printf("Введите ключ (целое число): ");
            scanf("%lld", &key);
            printf("Введите значение (строка без пробелов): ");
            scanf("%255s", value);

            map_insert(current_map, key, value);
            printf("[OK] Элемент [%lld: %s] успешно вставлен.\n", key, value);
            break;

        case 4:
            if (!current_map) {
                printf("[Ошибка] Сначала создайте дерево!\n");
                break;
            }
            printf("Введите ключ для поиска: ");
            scanf("%lld", &key);

            char* res = map_find(current_map, key);
            if (res) {
                printf("[Результат] Найден: %lld -> %s\n", key, res);
            }
            else {
                printf("[Результат] Ключ %lld не найден.\n", key);
            }
            break;

        case 5:
            if (!current_map) {
                printf("[Ошибка] Сначала создайте дерево!\n");
                break;
            }
            printf("Введите ключ для удаления: ");
            scanf("%lld", &key);

            map_delete(current_map, key);
            printf("[OK] Операция удаления завершена (если ключ существовал).\n");
            break;

        case 6:
            if (!current_map) {
                printf("[Ошибка] Сначала создайте дерево!\n");
                break;
            }
            printf("\n--- Структура дерева ---\n");
            map_print_struct(current_map);
            printf("--------------------------\n");
            break;
        case 7:
            if (!current_map) {
                printf("[Ошибка] Сначала создайте дерево!\n");
                break;
            }
            printf("\n--- Содержимое дерева по возрастанию ---\n");
            map_print(current_map);
            printf("--------------------------\n");
            break;
        case 0:
            if (current_map != NULL) {
                map_destroy(current_map);
                printf("[Info] Память очищена.\n");
            }
            printf("Выход из программы...\n");
            return 0;

        default:
            printf("[Ошибка] Неверный пункт меню. Попробуйте снова.\n");
        }
    }

    return 0;
}