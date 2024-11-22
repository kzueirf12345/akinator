#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tree.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "tree_structs.h"
#include "verification/verification.h"


enum TreeError tree_print_inorder_recursive_(FILE* out, const tree_node_t* const node, 
                                            elem_to_str_t elem_to_str);

enum TreeError tree_print_inorder(FILE* out, const tree_t* const tree, elem_to_str_t elem_to_str)
{
    if (!elem_to_str) elem_to_str = data_to_str;

    lassert(elem_to_str, "");
    lassert(out, "");
    TREE_VERIFY(tree, NULL); 

    TREE_ERROR_HANDLE(tree_print_inorder_recursive_(out, tree->Groot, elem_to_str));
    fputc('\n', out);

    return TREE_ERROR_SUCCESS;
}

enum TreeError tree_print_inorder_recursive_(FILE* out, const tree_node_t* const node,
                                            elem_to_str_t elem_to_str)
{
    if (!elem_to_str) elem_to_str = data_to_str;

    lassert(!is_invalid_ptr(node),       "node: %p", node);
    lassert(!is_invalid_ptr(node->data), "");
    lassert(!is_invalid_ptr(out),        "");
    lassert(                node->size,  "");
    lassert(                elem_to_str, "");

    if (node->lt)
    {
        fputc('(', out);
        TREE_ERROR_HANDLE(tree_print_inorder_recursive_(out, node->lt, elem_to_str));
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
        TREE_ERROR_HANDLE(tree_print_inorder_recursive_(out, node->rt, elem_to_str));
        fputc(')', out);
    }

    return TREE_ERROR_SUCCESS;
}


enum TreeError tree_print_preorder_recursive_(FILE* out, const tree_node_t* const node, 
                                              elem_to_str_t elem_to_str, const size_t deep);

enum TreeError tree_print_preorder(FILE* out, const tree_t* const tree, elem_to_str_t elem_to_str)
{
    if (!elem_to_str) elem_to_str = data_to_str;

    lassert(elem_to_str, "");
    lassert(out, "");
    TREE_VERIFY(tree, NULL); 

    TREE_ERROR_HANDLE(tree_print_preorder_recursive_(out, tree->Groot, elem_to_str, 0));

    return TREE_ERROR_SUCCESS;
}


void tabulate_deep_(FILE* out, size_t deep);

enum TreeError tree_print_preorder_recursive_(FILE* out, const tree_node_t* const node,
                                              elem_to_str_t elem_to_str, const size_t deep)
{
    if (!node) return TREE_ERROR_SUCCESS;
    if (!elem_to_str) elem_to_str = data_to_str;

    lassert(!is_invalid_ptr(node),          "");
    lassert(!is_invalid_ptr(node->data),    "");
    lassert(!is_invalid_ptr(out),           "");
    lassert(!is_invalid_ptr(elem_to_str),   "");
    lassert(                node->size,     "");


    tabulate_deep_(out, deep);
    fputs("{\n", out);

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

    tabulate_deep_(out, deep + 1);
    if (fprintf(out, "'%s'\n", data_str) <= 0)
    {
        perror("Can't fputs data_str");
        return TREE_ERROR_STANDARD_ERRNO;
    }
    free(data_str); data_str = NULL;

    tabulate_deep_(out, deep + 1);
    fputs("lt\n", out);
    TREE_ERROR_HANDLE(tree_print_preorder_recursive_(out, node->lt, elem_to_str, deep + 1));
    tabulate_deep_(out, deep + 1);
    fputs("rt\n", out);
    TREE_ERROR_HANDLE(tree_print_preorder_recursive_(out, node->rt, elem_to_str, deep + 1));

    tabulate_deep_(out, deep);
    fputs("}\n", out);

    return TREE_ERROR_SUCCESS;
}

void tabulate_deep_(FILE* out, size_t deep)
{
    while(deep--) 
        fputc('\t', out);
}