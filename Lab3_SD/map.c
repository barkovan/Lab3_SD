#include "map.h"

// Создание структуры с выделением памяти
Map* create_map(MapType type, int t) {
    Map* map = (Map*)malloc(sizeof(Map));
    if (map != NULL) {
        map->type = type;
        map->root = NULL;
    }
    if (type == BTREE_TYPE) map->btree_t = t;
    else map->btree_t = 0;
    return map;
}

// Вставка
void map_insert(Map* map, long long key, char* value) {
    if (map == NULL) return;

    int status;
    if (map->type == BST_TYPE) {
        map->root = bst_insert((BSTNode*)map->root, key, value, &status);
    }
    else if (map->type == BTREE_TYPE) {
        btree_insert(map, key, value);
    }
}

// Поиск
char* map_find(Map* map, long long key) {
    if (map == NULL || map->root == NULL) return NULL;

    if (map->type == BST_TYPE) {
        BSTNode* res = bst_search((BSTNode*)map->root, key);
        return res ? res->str : NULL;
    }
    else if (map->type == BTREE_TYPE) {
        return btree_search((BTreeNode*)map->root, key);
    }

    return NULL;
}

// Удаление
void map_delete(Map* map, long long key) {
    if (map == NULL || map->root == NULL) return;

    int status;
    if (map->type == BST_TYPE) {
        map->root = bst_delete((BSTNode*)map->root, key, &status);
    }
    else if (map->type == BTREE_TYPE) {
        btree_delete(map, key);
    }
}

// Вывод
void map_print(Map* map) {
    if (map == NULL) return;

    if (map->type == BST_TYPE) {
        bst_print((BSTNode*)map->root);
    }
    else if (map->type == BTREE_TYPE) {
        btree_print((BTreeNode*)map->root);
    }
}

// Вывод структуры
void map_print_struct(Map* map) {
    if (map == NULL || map->root == NULL) {
        printf("Tree is empty.\n");
        return;
    }

    if (map->type == BST_TYPE) {
        printf("\n--- Структура BST-дерева ---\n");
        bst_print_struct((BSTNode*)map->root, 0);
    }
    else if (map->type == BTREE_TYPE) {
        printf("\n--- Структура B-дерева ---\n");
        btree_print_struct((BTreeNode*)map->root, 0, "", true);
    }
}

// Освобождение памяти (удаление мапы)
void map_destroy(Map* map) {
    if (map == NULL) return;

    if (map->type == BST_TYPE) {
        free_bst_node((BSTNode*)map->root);
    }
    else if (map->type == BTREE_TYPE) {
        free_btree_node((BTreeNode*)map->root);
    }

    free(map); // Не забываем удалить саму обертку
}