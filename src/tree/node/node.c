#include <stdio.h>
#include <string.h>

#include "node.h"
#include "logger/liblogger.h"
#include "utils/utils.h"

tree_node_t* tree_node_ctor(const void* const data, const size_t size)
{
    lassert(!is_invalid_ptr(data),   "");
    lassert(size,                    "");

    tree_node_t* tree_node = (tree_node_t*)calloc(1, sizeof(tree_node_t));

    if (!tree_node)
    {
        perror("Can't calloc tree node");
        return NULL;
    }

    tree_node->size     = size;
    tree_node->lt       = NULL;
    tree_node->rt       = NULL;

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