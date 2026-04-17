#define _CRT_SECURE_NO_WARNINGS

#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Вспомогательная функция для создания узла */
BSTNode* create_bst_node(long long val, char* str) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    if (!node) return NULL;
    node->key = val;
    node->str = _strdup(str);
    node->left = node->right = NULL;
    return node;
}

/* Вставка пары key/value */
BSTNode* bst_insert(BSTNode* node, long long val, char* str, int* status) {
    if (node == NULL) {
        *status = 1;
        return create_bst_node(val, str);
    }

    if (val < node->key) {
        node->left = bst_insert(node->left, val, str, status);
    }
    else if (val > node->key) {
        node->right = bst_insert(node->right, val, str, status);
    }
    else {
        /* Замена значения при повторе ключа */
        free(node->str);
        node->str = _strdup(str);
        *status = 0;
    }

    return node;
}

/* Вспомогательные функции для поиска замены при удалении */
BSTNode* bst_get_min(BSTNode* n) {
    while (n && n->left) n = n->left;
    return n;
}

/* Удаление по ключу */
BSTNode* bst_delete(BSTNode* root, long long val, int* status) {
    if (root == NULL) {
        *status = 0;
        return NULL;
    }

    if (val < root->key) {
        root->left = bst_delete(root->left, val, status);
    }
    else if (val > root->key) {
        root->right = bst_delete(root->right, val, status);
    }
    else {
        *status = 1;

        // Случай 1 и 2: 0 или 1 потомок
        if (root->left == NULL) {
            BSTNode* temp = root->right;
            free(root->str);
            free(root);
            return temp;
        }
        else if (root->right == NULL) {
            BSTNode* temp = root->left;
            free(root->str);
            free(root);
            return temp;
        }

        // Случай 3: 2 потомка
        // Ищем минимальный элемент в правом поддереве
        BSTNode* temp = bst_get_min(root->right);

        free(root->str);
        root->key = temp->key;
        root->str = _strdup(temp->str);

        int temp_status;
        root->right = bst_delete(root->right, temp->key, &temp_status);
    }
    return root;
}

/* Поиск по ключу */
BSTNode* bst_search(BSTNode* root, long long val) {
    if (root == NULL || root->key == val) return root;
    if (val < root->key) return bst_search(root->left, val);
    return bst_search(root->right, val);
}

/* Обход с выводом по возрастанию */
void bst_print(BSTNode* n) {
    if (n == NULL) return;
    bst_print(n->left);
    printf("%lld: %s\n", n->key, n->str);
    bst_print(n->right);
}

/* Вывод структуры дерева */
void bst_print_struct(BSTNode* n, char* prefix, bool is_right, bool is_root) {
    if (n == NULL) return;

    printf("%s", prefix);

    if (!is_root) {
        printf(is_right ? "├── R: " : "└── L: ");
    }
    else {
        printf("└── Root: ");
    }

    printf("[%lld]\n", n->key);

    // Подготовка префикса для дочерних элементов
    char new_prefix[256];
    sprintf(new_prefix, "%s%s", prefix, is_root ? "    " : (is_right ? "│   " : "    "));

    // Проверяем наличие детей
    if (n->left || n->right) {
        // Сначала печатаем Правое поддерево (сверху)
        if (n->right) {
            bst_print_struct(n->right, new_prefix, true, false);
        }
        else {
            printf("%s├── R: (null)\n", new_prefix);
        }

        // Затем печатаем Левое поддерево (снизу)
        if (n->left) {
            bst_print_struct(n->left, new_prefix, false, false);
        }
        else {
            printf("%s└── L: (null)\n", new_prefix);
        }
    }
}

/* Освобождение всей памяти дерева */
void free_bst_node(BSTNode* root) {
    if (root == NULL) return;
    free_bst_node(root->left);
    free_bst_node(root->right);
    free(root->str);
    free(root);
}