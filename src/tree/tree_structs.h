#ifndef AKINATOR_SRC_TREE_TREE_STRUCTS_H
#define AKINATOR_SRC_TREE_TREE_STRUCTS_H

#include <stdio.h>

typedef struct TreeNode
{
    void* data;
    size_t size;

    struct TreeNode* lt;
    struct TreeNode* rt;
} tree_node_t;


typedef int (*tree_compare_t)(const void* const first, const void* const second);

typedef struct Tree
{
    tree_node_t* Groot;
    size_t size;
    tree_compare_t compare;
} tree_t;

#endif /*AKINATOR_SRC_TREE_TREE_STRUCTS_H*/