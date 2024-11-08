#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tree.h"
#include "logger/liblogger.h"
#include "utils.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* tree_strerror(const enum TreeError error)
{
    switch(error)
    {
        CASE_ENUM_TO_STRING_(TREE_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(TREE_ERROR_STANDARD_ERRNO);
        default:
            return "UNKNOWN_TREE_ERROR";
    }
    return "UNKNOWN_TREE_ERROR";
}
#undef CASE_ENUM_TO_STRING_


tree_node_t* tree_node_ctor(const void* const data, const size_t size, 
                            tree_node_t* const lt, tree_node_t* const rt)
{
    lassert(!is_invalid_ptr(data),   "");
    lassert(size,                    "");

    tree_node_t* tree_node = (tree_node_t*)calloc(1, sizeof(tree_node_t));

    if (!tree_node)
    {
        perror("Can't calloc tree node");
        return NULL;
    }

    tree_node->size      = size;
    tree_node->lt        = lt;
    tree_node->rt        = rt;

    tree_node->data = calloc(1, size);

    if (!tree_node->data)
    {
        perror("Can't calloc tree node data");
        free(tree_node); tree_node = NULL;
        return NULL;
    }

    memcpy(tree_node->data, data, size);

    return tree_node;
}

void tree_node_dtor(tree_node_t* const tree_node)
{
    lassert(!is_invalid_ptr(tree_node), "");
    lassert(!is_invalid_ptr(tree_node->data), "");

#ifndef NDEBUG
    tree_node->size      = 0;
    tree_node->lt        = NULL;
    tree_node->rt        = NULL;
#endif /*NDEBUG*/

    free(tree_node->data); IF_DEBUG(tree_node->data = NULL;)
    free(tree_node);
}



enum TreeError tree_ctor(tree_t* const tree, const tree_compare_t compare)
{
    lassert(!is_invalid_ptr(tree), "");

    tree->Groot = NULL;
    tree->compare = compare;
    tree->size  = 0;

    return TREE_ERROR_SUCCESS;
}

void tree_dtor_recursive_(tree_node_t* const tree_node);

void tree_dtor(tree_t* const tree)
{
    lassert(!is_invalid_ptr(tree), "");

    tree_dtor_recursive_(tree->Groot); IF_DEBUG(tree->Groot = NULL;)
    IF_DEBUG(tree->size    = 0;)
    IF_DEBUG(tree->compare = 0;)
}

void tree_dtor_recursive_(tree_node_t* const tree_node)
{
    static_assert(PTR_STATES_NULL     == 1);
    lassert(is_invalid_ptr(tree_node) <= 1, "");

    if (!tree_node)
        return;
    
    tree_dtor_recursive_(tree_node->lt);
    tree_dtor_recursive_(tree_node->rt);

    tree_node_dtor(tree_node);
}


enum TreeError tree_push_node_recursive_(tree_node_t** const tree, tree_node_t* const tree_node,
                                         const tree_compare_t compare);

enum TreeError tree_push_node(tree_t* const tree, tree_node_t* const tree_node)
{
    lassert(!is_invalid_ptr(tree), "");
    lassert(!is_invalid_ptr(tree_node), "");
    lassert(!is_invalid_ptr(tree_node->data), "");

    ++tree->size;
    
    if (!tree->Groot)
    {
        tree->Groot = tree_node;
        return TREE_ERROR_SUCCESS;
    }

    TREE_ERROR_HANDLE(tree_push_node_recursive_(&tree->Groot, tree_node, tree->compare));

    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_push_node_recursive_(tree_node_t** const tree, tree_node_t* const tree_node,
                                         const tree_compare_t compare)
{
    lassert(!is_invalid_ptr(tree), "");
    lassert(!is_invalid_ptr(tree_node), "");
    lassert(!is_invalid_ptr(tree_node->data), "");

    if (!*tree)
    {
        *tree = tree_node;
        return TREE_ERROR_SUCCESS;
    }

    if (compare(tree_node->data, (*tree)->data) <= 0)
    {
        TREE_ERROR_HANDLE(tree_push_node_recursive_(&(*tree)->lt, tree_node, compare));
    }
    else
    {
        TREE_ERROR_HANDLE(tree_push_node_recursive_(&(*tree)->rt, tree_node, compare));
    }

    return TREE_ERROR_SUCCESS;
}