#ifndef AKINATOR_SRC_TREE_TREE_H
#define AKINATOR_SRC_TREE_TREE_H

#include <stdio.h>

enum TreeError
{
    TREE_ERROR_SUCCESS          = 0,
    TREE_ERROR_STANDARD_ERRNO   = 1,
};

const char* tree_strerror(const enum TreeError error);

#define TREE_ERROR_HANDLE(call_func, ...)                                                           \
    do {                                                                                            \
        enum TreeError error_handler = call_func;                                                   \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            tree_strerror(error_handler));                                          \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)


typedef struct TreeNode
{
    void* data;
    size_t size;

    struct TreeNode* lt;
    struct TreeNode* rt;
} tree_node_t;

tree_node_t* tree_node_ctor(const void* const data, const size_t size, 
                            tree_node_t* const lt, tree_node_t* const rt);
void         tree_node_dtor(tree_node_t* const tree_node);


typedef int (*tree_compare_t)(const void* const first, const void* const second);

typedef struct Tree
{
    tree_node_t* Groot;
    size_t size;
    tree_compare_t compare;
} tree_t;

enum TreeError tree_ctor(tree_t* const tree, const tree_compare_t compare);
void           tree_dtor(tree_t* const tree);

enum TreeError tree_push_node(tree_t* const tree, tree_node_t* const tree_node);

#endif /*AKINATOR_SRC_TREE_TREE_H*/