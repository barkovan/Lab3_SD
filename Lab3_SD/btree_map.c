#define _CRT_SECURE_NO_WARNINGS

#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Создание нового узла
BTreeNode* create_btree_node(int t, bool leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->n = 0;
    node->leaf = leaf;

    node->keys = (long long*)malloc((2 * t - 1) * sizeof(long long));
    node->values = (char**)malloc((2 * t - 1) * sizeof(char*));
    node->children = (BTreeNode**)malloc((2 * t) * sizeof(BTreeNode*));

    for (int i = 0; i < 2 * t; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// Поиск значения по ключу
char* btree_search(BTreeNode* x, long long key) {
    if (x == NULL) return NULL;

    int i = 0;
    while (i < x->n && key > x->keys[i]) {
        i++;
    }

    if (i < x->n && key == x->keys[i]) {
        return x->values[i];
    }

    if (x->leaf) {
        return NULL;
    }

    return btree_search(x->children[i], key);
}

// Разделение переполненного дочернего узла (y), который является i-м ребенком узла x
void btree_split_child(BTreeNode* x, int i, BTreeNode* y, int t) {
    BTreeNode* z = create_btree_node(t, y->leaf);
    z->n = t - 1;

    // Переносим правую половину ключей и значений из y в z
    for (int j = 0; j < t - 1; j++) {
        z->keys[j] = y->keys[j + t];
        z->values[j] = y->values[j + t];
    }

    // Если это не лист, переносим и детей
    if (!y->leaf) {
        for (int j = 0; j < t; j++) {
            z->children[j] = y->children[j + t];
            y->children[j + t] = NULL;
        }
    }
    y->n = t - 1;

    // Сдвигаем детей родителя x, чтобы освободить место для нового узла z
    for (int j = x->n; j >= i + 1; j--) {
        x->children[j + 1] = x->children[j];
    }
    x->children[i + 1] = z;

    // Сдвигаем ключи родителя x, чтобы поднять медианный ключ из y
    for (int j = x->n - 1; j >= i; j--) {
        x->keys[j + 1] = x->keys[j];
        x->values[j + 1] = x->values[j];
    }

    // Поднимаем медианный ключ
    x->keys[i] = y->keys[t - 1];
    x->values[i] = y->values[t - 1];
    x->n = x->n + 1;
}

// Вставка в неполный узел
void btree_insert_non_full(BTreeNode* x, long long key, const char* value, int t) {
    int i = x->n - 1;

    // Сначала проверяем, есть ли такой ключ в текущем узле (для замены)
    for (int j = 0; j < x->n; j++) {
        if (x->keys[j] == key) {
            free(x->values[j]);
            x->values[j] = _strdup(value);
            return;
        }
    }

    if (x->leaf) {
        // Сдвигаем элементы, чтобы вставить новый ключ в правильное место
        while (i >= 0 && key < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            x->values[i + 1] = x->values[i];
            i--;
        }
        x->keys[i + 1] = key;
        x->values[i + 1] = _strdup(value);
        x->n = x->n + 1;
    }
    else {
        // Ищем, в какого ребенка спускаться
        while (i >= 0 && key < x->keys[i]) {
            i--;
        }
        i++;

        // Если ребенок переполнен, разделяем его перед тем, как спуститься
        if (x->children[i]->n == 2 * t - 1) {
            btree_split_child(x, i, x->children[i], t);
            if (key > x->keys[i]) {
                i++;
            }
        }
        btree_insert_non_full(x->children[i], key, value, t);
    }
}

// Главная функция вставки
void btree_insert(Map* map, long long key, const char* value) {
    BTreeNode* root = (BTreeNode*)map->root;
    int t = map->btree_t;

    if (root == NULL) {
        root = create_btree_node(t, true);
        root->keys[0] = key;
        root->values[0] = _strdup(value);
        root->n = 1;
        map->root = root;
    }
    else {
        if (root->n == 2 * t - 1) {
            // Корень переполнен: дерево растет в высоту
            BTreeNode* s = create_btree_node(t, false);
            s->children[0] = root;
            btree_split_child(s, 0, root, t);

            // Определяем, в какую половину вставлять
            int i = 0;
            if (s->keys[0] < key) i++;
            btree_insert_non_full(s->children[i], key, value, t);

            map->root = s; // Обновляем корень в структуре Map
        }
        else {
            btree_insert_non_full(root, key, value, t);
        }
    }
}

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ УДАЛЕНИЯ ---

// Найти первое вхождение ключа, большего или равного заданному
static int btree_find_key(BTreeNode* node, long long key) {
    int idx = 0;
    while (idx < node->n && node->keys[idx] < key) {
        ++idx;
    }
    return idx;
}

// Удаление из листа
static void btree_remove_from_leaf(BTreeNode* node, int idx) {
    free(node->values[idx]);
    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
        node->values[i - 1] = node->values[i];
    }
    node->n--;
}

// Заимствование у левого брата
static void btree_borrow_from_prev(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx - 1];

    // Сдвигаем все ключи в child на шаг вперед
    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
        child->values[i + 1] = child->values[i];
    }
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
    }

    // Спускаем ключ из родителя в child
    child->keys[0] = node->keys[idx - 1];
    child->values[0] = node->values[idx - 1];

    // Поднимаем последний ключ из sibling в родителя
    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->n];
    }
    node->keys[idx - 1] = sibling->keys[sibling->n - 1];
    node->values[idx - 1] = sibling->values[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;
}

// Заимствование у правого брата
static void btree_borrow_from_next(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx + 1];

    // Спускаем ключ из родителя в конец child
    child->keys[child->n] = node->keys[idx];
    child->values[child->n] = node->values[idx];

    if (!child->leaf) {
        child->children[child->n + 1] = sibling->children[0];
    }

    // Поднимаем первый ключ из sibling в родителя
    node->keys[idx] = sibling->keys[0];
    node->values[idx] = sibling->values[0];

    // Сдвигаем ключи в sibling
    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
        sibling->values[i - 1] = sibling->values[i];
    }
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    child->n += 1;
    sibling->n -= 1;
}

// Слияние узла child с правым братом
static void btree_merge(BTreeNode* node, int idx, int t) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx + 1];

    // Спускаем ключ из родителя в середину child
    child->keys[t - 1] = node->keys[idx];
    child->values[t - 1] = node->values[idx];

    // Копируем ключи из sibling в child
    for (int i = 0; i < sibling->n; ++i) {
        child->keys[i + t] = sibling->keys[i];
        child->values[i + t] = sibling->values[i];
    }
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i) {
            child->children[i + t] = sibling->children[i];
        }
    }

    // Сдвигаем ключи и детей в родителе
    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
        node->values[i - 1] = node->values[i];
    }
    for (int i = idx + 2; i <= node->n; ++i) {
        node->children[i - 1] = node->children[i];
    }

    child->n += sibling->n + 1;
    node->n--;

    // Освобождаем оболочку sibling
    free(sibling->keys);
    free(sibling->values);
    free(sibling->children);
    free(sibling);
}

// Гарантирует, что у ребенка node->children[idx] есть хотя бы t ключей
static void btree_fill(BTreeNode* node, int idx, int t) {
    if (idx != 0 && node->children[idx - 1]->n >= t) {
        btree_borrow_from_prev(node, idx);
    }
    else if (idx != node->n && node->children[idx + 1]->n >= t) {
        btree_borrow_from_next(node, idx);
    }
    else {
        if (idx != node->n) {
            btree_merge(node, idx, t);
        }
        else {
            btree_merge(node, idx - 1, t);
        }
    }
}

// Получить предшественника (самый правый в левом поддереве)
static void btree_get_pred(BTreeNode* node, int idx, long long* p_key, char** p_val) {
    BTreeNode* cur = node->children[idx];
    while (!cur->leaf) {
        cur = cur->children[cur->n];
    }
    *p_key = cur->keys[cur->n - 1];
    *p_val = _strdup(cur->values[cur->n - 1]);
}

// Получить преемника (самый левый в правом поддереве)
static void btree_get_succ(BTreeNode* node, int idx, long long* s_key, char** s_val) {
    BTreeNode* cur = node->children[idx + 1];
    while (!cur->leaf) {
        cur = cur->children[0];
    }
    *s_key = cur->keys[0];
    *s_val = _strdup(cur->values[0]);
}

static void btree_delete_internal(BTreeNode* node, long long key, int t);

// Удаление из внутреннего узла
static void btree_remove_from_non_leaf(BTreeNode* node, int idx, int t) {
    long long key = node->keys[idx];

    if (node->children[idx]->n >= t) {
        long long p_key;
        char* p_val;
        btree_get_pred(node, idx, &p_key, &p_val);

        free(node->values[idx]);
        node->keys[idx] = p_key;
        node->values[idx] = p_val;
        btree_delete_internal(node->children[idx], p_key, t);
    }
    else if (node->children[idx + 1]->n >= t) {
        long long s_key;
        char* s_val;
        btree_get_succ(node, idx, &s_key, &s_val);

        free(node->values[idx]);
        node->keys[idx] = s_key;
        node->values[idx] = s_val;
        btree_delete_internal(node->children[idx + 1], s_key, t);
    }
    else {
        btree_merge(node, idx, t);
        btree_delete_internal(node->children[idx], key, t);
    }
}

// Рекурсивный обход для удаления
static void btree_delete_internal(BTreeNode* node, long long key, int t) {
    int idx = btree_find_key(node, key);

    if (idx < node->n && node->keys[idx] == key) {
        if (node->leaf) {
            btree_remove_from_leaf(node, idx);
        }
        else {
            btree_remove_from_non_leaf(node, idx, t);
        }
    }
    else {
        if (node->leaf) {
            return;
        }

        bool flag = ((idx == node->n) ? true : false);

        if (node->children[idx]->n < t) {
            btree_fill(node, idx, t);
        }

        if (flag && idx > node->n) {
            btree_delete_internal(node->children[idx - 1], key, t);
        }
        else {
            btree_delete_internal(node->children[idx], key, t);
        }
    }
}

// Удаление
void btree_delete(Map* map, long long key) {
    if (map == NULL || map->root == NULL) return;

    BTreeNode* root = (BTreeNode*)map->root;
    int t = map->btree_t;

    btree_delete_internal(root, key, t);

    // Если корень стал пустым
    if (root->n == 0) {
        if (root->leaf) {
            map->root = NULL;
        }
        else {
            map->root = root->children[0];
        }

        // Освобождаем память только оболочки старого корня
        free(root->keys);
        free(root->values);
        free(root->children);
        free(root);
    }
}

// Симметричный обход: выводит элементы по возрастанию ключа
void btree_print(BTreeNode* x) {
    if (x == NULL) return;
    int i;
    for (i = 0; i < x->n; i++) {
        if (!x->leaf) {
            btree_print(x->children[i]);
        }
        printf("%lld: %s\n", x->keys[i], x->values[i]);
    }
    if (!x->leaf) {
        btree_print(x->children[i]);
    }
}

/* Вывод структуры дерева */
void btree_print_struct(BTreeNode* x, int level, char* prefix, bool is_last) {
    if (x == NULL) return;

    // Печатаем текущий узел
    printf("%s", prefix);
    printf(is_last ? "└── " : "├── ");

    // Выводим все ключи узла в одной строке
    printf("[");
    for (int i = 0; i < x->n; i++) {
        printf("%lld%s", x->keys[i], (i == x->n - 1 ? "" : " | "));
    }
    printf("]%s\n", x->leaf ? " (leaf)" : "");

    // Если не лист, выводим детей
    if (!x->leaf) {
        for (int i = 0; i <= x->n; i++) {
            // Формируем префикс для следующего уровня
            char new_prefix[256];
            sprintf(new_prefix, "%s%s", prefix, is_last ? "    " : "│   ");

            // Определяем, последний ли это ребенок
            bool last_child = (i == x->n);

            // Печатаем индекс ветки для ясности
            // i=0 - ветка левее первого ключа, i=n - правее последнего
            btree_print_struct(x->children[i], level + 1, new_prefix, last_child);
        }
    }
}

// Полное освобождение памяти
void free_btree_node(BTreeNode* x) {
    if (x == NULL) return;

    // Рекурсивно удаляем детей
    if (!x->leaf) {
        for (int i = 0; i <= x->n; i++) {
            free_btree_node(x->children[i]);
        }
    }

    // Очищаем строки значений
    for (int i = 0; i < x->n; i++) {
        free(x->values[i]);
    }

    free(x->keys);
    free(x->values);
    free(x->children);
    free(x);
}