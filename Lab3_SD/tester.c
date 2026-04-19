#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "map.h"

/* Простой shuffle массива */
static void shuffle(long long* arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        long long temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    printf("========================================\n");
    printf("   ТЕСТЕР БИБЛИОТЕКИ MAP      \n");
    printf("========================================\n\n");

    /* Выбор режима данных */
    int mode;
    printf("Выберите тип набора данных:\n");
    printf("1. Случайный (перемешанный)\n");
    printf("2. Отсортированный (1..N)\n");
    printf("Ваш выбор: ");
    if (scanf("%d", &mode) != 1 || (mode != 1 && mode != 2)) {
        printf("[ОШИБКА] Некорректный выбор.\n");
        return 1;
    }

    /* Размер теста */
    int N;
    printf("\nВведите количество элементов N (рекомендуется 1000–20000): ");
    if (scanf("%d", &N) != 1 || N <= 0) {
        printf("[ОШИБКА] N должен быть положительным числом.\n");
        return 1;
    }

    /* Параметр t только для B-дерева */
    int t = 5;
    printf("Введите параметр t для B-дерева (минимум 2, рекомендуется 4–8): ");
    if (scanf("%d", &t) != 1 || t < 2) {
        t = 5;
        printf("[INFO] t установлен по умолчанию = 5\n");
    }

    /* Количество удаляемых элементов — половина */
    int del_count = N / 2;

    printf("\nЗапуск тестирования на %d элементах (%s набор, t=%d для B-дерева)...\n\n",
        N, (mode == 1 ? "СЛУЧАЙНЫЙ" : "ОТСОРТИРОВАННЫЙ"), t);

    /* Генерируем один и тот же набор ключей для всех реализаций */
    long long* keys = (long long*)malloc(N * sizeof(long long));
    for (int i = 0; i < N; i++) {
        keys[i] = (long long)i + 1;   /* 1, 2, ..., N */
    }
    if (mode == 1) {
        srand((unsigned int)time(NULL));
        shuffle(keys, N);
    }

    /* Ключи для удаления (половина от исходного набора) */
    long long* del_keys = (long long*)malloc(del_count * sizeof(long long));
    memcpy(del_keys, keys, del_count * sizeof(long long));
    if (mode == 1) {
        shuffle(del_keys, del_count);   /* ещё раз перемешиваем, чтобы удаление было случайным */
    }

    /* Таблица результатов */
    printf("%-12s | %-12s | %-12s | %-12s | %-8s\n",
        "Реализация", "Вставка (мс)", "Поиск (мс)", "Удаление (мс)", "Высота");
    printf("-----------------------------------------------------------------\n");

    MapType types[3] = { BST_TYPE, BTREE_TYPE, RBTREE_TYPE };
    const char* type_names[3] = { "BST", "B-Tree", "Red-Black" };

    for (int i = 0; i < 3; i++) {
        /* Создаём карту нужного типа */
        Map* map = create_map(types[i], (types[i] == BTREE_TYPE ? t : 0));
        if (!map) {
            printf("%-12s | [ОШИБКА создания]\n", type_names[i]);
            continue;
        }

        /* === 1. ВСТАВКА N элементов === */
        clock_t start = clock();
        for (int j = 0; j < N; j++) {
            char value[64];
            sprintf(value, "val_%lld", keys[j]);
            map_insert(map, keys[j], value);
        }
        clock_t end = clock();
        double insert_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

        /* === 2. ПОИСК всех N ключей === */
        start = clock();
        for (int j = 0; j < N; j++) {
            map_find(map, keys[j]);   /* ищем все вставленные ключи */
        }
        end = clock();
        double search_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

        /* === 3. УДАЛЕНИЕ del_count элементов === */
        start = clock();
        for (int j = 0; j < del_count; j++) {
            map_delete(map, del_keys[j]);
        }
        end = clock();
        double delete_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

        /* === 4. Высота дерева после операций === */
        int height = map_height(map);

        /* Вывод строки таблицы */
        printf("%-12s | %-12.2f | %-12.2f | %-12.2f | %-8d\n",
            type_names[i], insert_ms, search_ms, delete_ms, height);

        map_destroy(map);
    }

    free(keys);
    free(del_keys);

    printf("-----------------------------------------------------------------\n");
    printf("Тестирование завершено.\n");
    printf("Результаты можно скопировать в отчёт.\n\n");
    printf("Нажмите Enter для выхода...");
    getchar(); getchar();

    return 0;
}