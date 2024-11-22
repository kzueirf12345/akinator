#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tree.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "tree_structs.h"
#include "verification/verification.h"


enum TreeError tree_ctor(tree_t* const tree, const tree_compare_t compare)
{
    lassert(!is_invalid_ptr(tree), "");

    tree->Groot = NULL;
    tree->compare = compare;
    tree->size  = 0;

    TREE_VERIFY(tree, NULL);
    return TREE_ERROR_SUCCESS;
}

void tree_dtor_recursive_(tree_node_t* const tree_node);

void tree_dtor(tree_t* const tree)
{
    TREE_VERIFY(tree, NULL);

    tree_dtor_recursive_(tree->Groot); IF_DEBUG(tree->Groot = NULL;)
    IF_DEBUG(tree->size    = 0;)
    IF_DEBUG(tree->compare = NULL;)
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


enum TreeError tree_insert_recursive_(tree_node_t** const tree, tree_node_t* const tree_node,
                                      const tree_compare_t compare);

enum TreeError tree_insert(tree_t* const tree, tree_node_t* const tree_node)
{
    TREE_VERIFY(tree, NULL);
    lassert(!is_invalid_ptr(tree_node), "");
    lassert(!is_invalid_ptr(tree_node->data), "");

    ++tree->size;
    
    if (!tree->Groot)
    {
        tree->Groot = tree_node;
        return TREE_ERROR_SUCCESS;
    }
    
    lassert(!is_invalid_ptr(tree->compare), "");

    TREE_ERROR_HANDLE(tree_insert_recursive_(&tree->Groot, tree_node, tree->compare));

    TREE_VERIFY(tree, NULL);
    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_insert_recursive_(tree_node_t** const tree, tree_node_t* const tree_node,
                                      const tree_compare_t compare)
{
    lassert(!is_invalid_ptr(tree), "");
    lassert(!is_invalid_ptr(tree_node), "");
    lassert(!is_invalid_ptr(tree_node->data), "");
    lassert(!is_invalid_ptr(compare), "");

    if (!*tree)
    {
        *tree = tree_node;
        return TREE_ERROR_SUCCESS;
    }

    if (compare(tree_node->data, (*tree)->data) <= 0)
    {
        TREE_ERROR_HANDLE(tree_insert_recursive_(&(*tree)->lt, tree_node, compare));
    }
    else
    {
        TREE_ERROR_HANDLE(tree_insert_recursive_(&(*tree)->rt, tree_node, compare));
    }

    return TREE_ERROR_SUCCESS;
}


tree_node_t* tree_find_node_recursive_(tree_node_t* const node, const void* const data, 
                                       const size_t size);

tree_node_t* tree_find_node(const tree_t* const tree, const void* const data, const size_t size)
{
    TREE_VERIFY(tree, NULL);
    lassert(!is_invalid_ptr(data), "");
    lassert(size, "");

    return tree_find_node_recursive_(tree->Groot, data, size);
}

tree_node_t* tree_find_node_recursive_(tree_node_t* const node, const void* const data, 
                                       const size_t size)
{
    if (node == NULL) return NULL;

    lassert(!is_invalid_ptr(node), "");
    lassert(!is_invalid_ptr(data), "");
    lassert(size, "");

    if (memcmp(node->data, data, MIN(node->size, size)) == 0)
        return node;
    
    tree_node_t* lt_find_res = tree_find_node_recursive_(node->lt, data, size);
    if (lt_find_res)
        return lt_find_res;
    
    return tree_find_node_recursive_(node->rt, data, size);
}