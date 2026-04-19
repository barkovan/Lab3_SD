#define _CRT_SECURE_NO_WARNINGS
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED   0
#define BLACK 1

// Создание узла 
static RBNode* create_rb_node(long long key, const char* value) {
    RBNode* node = (RBNode*)malloc(sizeof(RBNode));
    if (!node) return NULL;
    node->key = key;
    node->value = _strdup(value);
    node->color = RED;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    return node;
}

static int is_red(RBNode* node) {
    return node != NULL && node->color == RED;
}

static int is_black(RBNode* node) {
    return node == NULL || node->color == BLACK;
}

// --- Повороты ---

// Левый поворот
static void rb_rotate_left(Map* map, RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;

    if (y->left != NULL) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == NULL) {
        map->root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

// Правый поворот
static void rb_rotate_right(Map* map, RBNode* y) {
    RBNode* x = y->left;
    y->left = x->right;

    if (x->right != NULL) {
        x->right->parent = y;
    }

    x->parent = y->parent;

    if (y->parent == NULL) {
        map->root = x;
    }
    else if (y == y->parent->right) {
        y->parent->right = x;
    }
    else {
        y->parent->left = x;
    }

    x->right = y;
    y->parent = x;
}

// --- Балансировка после вставки ---

static void rb_insert_fixup(Map* map, RBNode* z) {
    while (is_red(z->parent)) {
        if (z->parent == z->parent->parent->left) {
            // Дядя справа
            RBNode* y = z->parent->parent->right;

            if (is_red(y)) {
                // Случай 1: дядя красный
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    // Случай 2: z - правый ребенок
                    z = z->parent;
                    rb_rotate_left(map, z);
                }
                // Случай 3: z - левый ребенок
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rb_rotate_right(map, z->parent->parent);
            }
        }
        else {
            // Симметрично (дядя слева)
            RBNode* y = z->parent->parent->left;

            if (is_red(y)) {
                // Случай 1: дядя красный
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    // Случай 2: z - левый ребенок
                    z = z->parent;
                    rb_rotate_right(map, z);
                }
                // Случай 3: z - правый ребенок
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rb_rotate_left(map, z->parent->parent);
            }
        }
    }

    ((RBNode*)map->root)->color = BLACK;
}

// --- Вставка ---

void rb_insert(Map* map, long long key, const char* value) {
    if (map == NULL) return;

    RBNode* z = create_rb_node(key, value);
    if (!z) return;

    RBNode* y = NULL;
    RBNode* x = (RBNode*)map->root;

    // Обычный поиск места вставки как в BST
    while (x != NULL) {
        y = x;
        if (key < x->key) {
            x = x->left;
        }
        else if (key > x->key) {
            x = x->right;
        }
        else {
            // Ключ уже существует - заменяем значение
            free(x->value);
            x->value = _strdup(value);
            free(z->value);
            free(z);
            return;
        }
    }

    z->parent = y;

    if (y == NULL) {
        map->root = z;
    }
    else if (key < y->key) {
        y->left = z;
    }
    else {
        y->right = z;
    }

    z->color = RED;
    rb_insert_fixup(map, z);
}

// --- Поиск ---

char* rb_search(RBNode* root, long long key) {
    RBNode* current = root;

    while (current != NULL) {
        if (key < current->key) {
            current = current->left;
        }
        else if (key > current->key) {
            current = current->right;
        }
        else {
            return current->value;
        }
    }

    return NULL;
}

// --- Обход по возрастанию ---

void rb_print(RBNode* root) {
    if (root == NULL) return;
    rb_print(root->left);
    printf("%lld: %s\n", root->key, root->value);
    rb_print(root->right);
}

// --- Визуализация ---

void rb_print_struct(RBNode* n, char* prefix, bool is_right, bool is_root) {
    if (n == NULL) return;

    printf("%s", prefix);

    if (!is_root) {
        printf(is_right ? "├── R: " : "└── L: ");
    }
    else {
        printf("└── Root: ");
    }

    printf("[%lld] (%s)\n", n->key, n->color == RED ? "R" : "B");

    // Подготовка префикса для дочерних элементов
    char new_prefix[256];
    sprintf(new_prefix, "%s%s", prefix, is_root ? "    " : (is_right ? "│   " : "    "));

    // Проверяем наличие детей
    if (n->left || n->right) {
        // Сначала печатаем Правое поддерево (сверху)
        if (n->right) {
            rb_print_struct(n->right, new_prefix, true, false);
        }
        else {
            printf("%s├── R: (null)\n", new_prefix);
        }

        // Затем печатаем Левое поддерево (снизу)
        if (n->left) {
            rb_print_struct(n->left, new_prefix, false, false);
        }
        else {
            printf("%s└── L: (null)\n", new_prefix);
        }
    }
}

// --- Вспомогательные функции для удаления ---

// Поиск минимального узла в поддереве
static RBNode* rb_minimum(RBNode* node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

// Замена одного поддерева другим (transplant)
static void rb_transplant(Map* map, RBNode* u, RBNode* v) {
    if (u->parent == NULL) {
        map->root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    if (v != NULL) {
        v->parent = u->parent;
    }
}

// Балансировка после удаления
static void rb_delete_fixup(Map* map, RBNode* x, RBNode* x_parent) {
    while (x != map->root && is_black(x)) {
        if (x == x_parent->left) {
            RBNode* w = x_parent->right; // брат
            if (is_red(w)) {
                // Случай 1: брат красный
                w->color = BLACK;
                x_parent->color = RED;
                rb_rotate_left(map, x_parent);
                w = x_parent->right;
            }
            if (is_black(w->left) && is_black(w->right)) {
                // Случай 2: оба ребёнка брата чёрные
                w->color = RED;
                x = x_parent;
                x_parent = x->parent;
            }
            else {
                if (is_black(w->right)) {
                    // Случай 3: правый ребёнок брата чёрный, левый красный
                    w->left->color = BLACK;
                    w->color = RED;
                    rb_rotate_right(map, w);
                    w = x_parent->right;
                }
                // Случай 4: правый ребёнок брата красный
                w->color = x_parent->color;
                x_parent->color = BLACK;
                w->right->color = BLACK;
                rb_rotate_left(map, x_parent);
                x = map->root; // выход из цикла
            }
        }
        else {
            // Симметричный случай (x - правый ребёнок)
            RBNode* w = x_parent->left;
            if (is_red(w)) {
                w->color = BLACK;
                x_parent->color = RED;
                rb_rotate_right(map, x_parent);
                w = x_parent->left;
            }
            if (is_black(w->right) && is_black(w->left)) {
                w->color = RED;
                x = x_parent;
                x_parent = x->parent;
            }
            else {
                if (is_black(w->left)) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rb_rotate_left(map, w);
                    w = x_parent->left;
                }
                w->color = x_parent->color;
                x_parent->color = BLACK;
                w->left->color = BLACK;
                rb_rotate_right(map, x_parent);
                x = map->root;
            }
        }
    }
    if (x != NULL) {
        x->color = BLACK;
    }
}

// Главная функция удаления
void rb_delete(Map* map, long long key) {
    if (map == NULL || map->root == NULL) return;

    RBNode* z = (RBNode*)map->root;
    // Поиск удаляемого узла
    while (z != NULL) {
        if (key < z->key) {
            z = z->left;
        }
        else if (key > z->key) {
            z = z->right;
        }
        else {
            break; // нашли
        }
    }
    if (z == NULL) return; // ключ не найден

    RBNode* y = z;
    RBNode* x;
    RBNode* x_parent;
    int y_original_color = y->color;

    if (z->left == NULL) {
        // Случай 1: нет левого ребёнка
        x = z->right;
        x_parent = z->parent;
        rb_transplant(map, z, z->right);
    }
    else if (z->right == NULL) {
        // Случай 2: нет правого ребёнка
        x = z->left;
        x_parent = z->parent;
        rb_transplant(map, z, z->left);
    }
    else {
        // Случай 3: оба ребёнка есть
        y = rb_minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x_parent = y;
        }
        else {
            rb_transplant(map, y, y->right);
            y->right = z->right;
            y->right->parent = y;
            x_parent = y->parent;
        }
        rb_transplant(map, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    free(z->value);
    free(z);

    // Если удалённый узел был чёрным, нужна балансировка
    if (y_original_color == BLACK) {
        rb_delete_fixup(map, x, x_parent);
    }
}


// --- Высота дерева ---

int rb_height(RBNode* root) {
    if (root == NULL) return 0;
    int left_h = rb_height(root->left);
    int right_h = rb_height(root->right);
    return 1 + (left_h > right_h ? left_h : right_h);
}

// --- Освобождение памяти ---

void free_rb_node(RBNode* root) {
    if (root == NULL) return;
    free_rb_node(root->left);
    free_rb_node(root->right);
    free(root->value);
    free(root);
}