#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tree.h"
#include "logger/liblogger.h"
#include "utils.h"
#include "tree_structs.h"
#include "../verification/verification.h"


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

    tree_node->size      = size;
    tree_node->lt        = NULL;
    tree_node->rt        = NULL;

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
    lassert(!is_invalid_ptr(compare), "");

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

enum TreeError tree_print_recursive(FILE* out, const tree_node_t* const node, 
                                    elem_to_str_t elem_to_str);

enum TreeError tree_print(FILE* out, const tree_t* const tree, elem_to_str_t elem_to_str)
{
    if (!elem_to_str) elem_to_str = data_to_str;

    lassert(elem_to_str, "");
    lassert(out, "");
    TREE_VERIFY(tree, NULL); 

    TREE_ERROR_HANDLE(tree_print_recursive(out, tree->Groot, elem_to_str));
    fputc('\n', out);

    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_print_recursive(FILE* out, const tree_node_t* const node,
                                    elem_to_str_t elem_to_str)
{
    if (!elem_to_str) elem_to_str = data_to_str;

    lassert(!is_invalid_ptr(node),       "");
    lassert(!is_invalid_ptr(node->data), "");
    lassert(!is_invalid_ptr(out),        "");
    lassert(                node->size,  "");
    lassert(                elem_to_str, "");

    if (node->lt)
    {
        fputc('(', out);
        TREE_ERROR_HANDLE(tree_print_recursive(out, node->lt, elem_to_str));
        fputc(')', out);
    }

    const size_t data_str_size = 4 * node->size;
    char* data_str = calloc(data_str_size, sizeof(char));

    if (!data_str)
    {
        perror("Can't calloc data_str");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    if (elem_to_str(node->data, node->size, &data_str, data_str_size))
    {
        fprintf(stderr, "Can't elem_to_str\n");
        return TREE_ERROR_ELEM_TO_STR;
    }

    if (fputs(data_str, out) <= 0)
    {
        perror("Can't fputs data_str");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    free(data_str); data_str = NULL;

    if (node->rt)
    {
        fputc('(', out);
        TREE_ERROR_HANDLE(tree_print_recursive(out, node->rt, elem_to_str));
        fputc(')', out);
    }

    return TREE_ERROR_SUCCESS;
}