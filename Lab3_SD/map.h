#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum { BST_TYPE, BTREE_TYPE, RBTREE_TYPE } MapType;

// Универсальная структура
typedef struct {
    void* root; // Указатель на корень
    MapType type;
    int btree_t; // Параметр t для BTree
} Map;

// Создание структуры
Map* create_map(MapType type, int t);

// Универсальные функции
void map_insert(Map* map, long long key, char* value);
char* map_find(Map* map, long long key);
void map_delete(Map* map, long long key);
void map_print(Map* map);
void map_print_struct(Map* map);
void map_destroy(Map* map);


// для тестера
int map_height(Map* map);

// BST-дерево
typedef struct Node {
    long long key;
    char* str;
    struct Node* left;
    struct Node* right;
} BSTNode;

BSTNode* bst_insert(BSTNode* node, long long val, char* str, int* status);
BSTNode* bst_delete(BSTNode* root, long long val, int* status);
BSTNode* bst_search(BSTNode* root, long long val);
void bst_print(BSTNode* n);
void bst_print_struct(BSTNode* n, char* prefix, bool is_left, bool is_root); 
void free_bst_node(BSTNode* root);
int bst_height(BSTNode* node);

// B-дерево
typedef struct BTreeNode {
    long long* keys;
    char** values;
    struct BTreeNode** children;
    int n;
    bool leaf;
} BTreeNode;

void btree_insert(Map* map, long long key, const char* value);
void btree_delete(Map* map, long long key);
char* btree_search(BTreeNode* x, long long key);
void btree_print(BTreeNode* x);
void btree_print_struct(BTreeNode* x, int level, char* prefix, bool is_last);
void free_btree_node(BTreeNode* x);
int btree_height(BTreeNode* node);

// Красно-черное дерево
typedef struct RBNode {
    long long key;
    char* value;
    int color;
    struct RBNode* left;
    struct RBNode* right;
    struct RBNode* parent;
} RBNode;

void rb_insert(Map* map, long long key, const char* value);
void rb_delete(Map* map, long long key);
char* rb_search(RBNode* root, long long key);
void rb_print(RBNode* root);
void rb_print_struct(RBNode* root, char* prefix, bool is_right, bool is_root);
void free_rb_node(RBNode* root);
int rb_height(RBNode* root);