#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tree.h"
#include "logger/liblogger.h"
#include "utils/utils.h"
#include "tree_structs.h"
#include "verification/verification.h"


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
    // lassert(!is_invalid_ptr(compare), "");

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


static enum TreeError init_str_from_file_(const char* const input_filename, 
                                          char** const str, size_t* const str_size);

static enum TreeError fill_tree_from_file_recursive_(tree_t* const tree, tree_node_t* const parent, 
                                                     char** cur_str, const bool is_lt);

#define MAX_DATA_SIZE_ 256
#define DIRECTION_SIZE_ 3
#define STRCHR_NEW_STR_VERIFY_(str, ...)                                                            \
        do{                                                                                         \
            if (!(str = strchr(str, '\n') + 1))                                                     \
            {                                                                                       \
                fprintf(stderr, "Can't strchr\n");                                                  \
                __VA_ARGS__                                                                         \
                return TREE_ERROR_STANDARD_ERRNO;                                                   \
            }                                                                                       \
        } while(0)

enum TreeError fill_tree_from_file(tree_t* const tree, const char* const input_filename)
{
    TREE_VERIFY(tree, NULL);
    lassert(!is_invalid_ptr(input_filename), "");
    lassert(tree->size == 0, "");

    size_t str_size = 0;
    char* str = NULL;
    TREE_ERROR_HANDLE(init_str_from_file_(input_filename, &str, &str_size));


    char* cur_str = str;

    int count_scan = 0;
    sscanf(cur_str, "{%n", &count_scan);

    if (count_scan != 1)
        return TREE_ERROR_SUCCESS;


    STRCHR_NEW_STR_VERIFY_(cur_str);

    char data[MAX_DATA_SIZE_] = {};
    if (sscanf(cur_str, "%*[ \t]'%[^']'", data) == 0)
    {
        fprintf(stderr, "Can't sscanf\n");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    tree->Groot = tree_node_ctor(data, MAX_DATA_SIZE_);
    if (!tree->Groot)
    {
        fprintf(stderr, "Can't tree_node_ctor\n");
        return TREE_ERROR_NODE_IS_NVALID;
    }
    ++tree->size;


    char direction[DIRECTION_SIZE_] = {};

    STRCHR_NEW_STR_VERIFY_(cur_str,                                   tree_node_dtor(tree->Groot););

    sscanf(cur_str, "%*[ \t]%s", direction);
    TREE_ERROR_HANDLE(fill_tree_from_file_recursive_(tree, tree->Groot, &cur_str, true));

    STRCHR_NEW_STR_VERIFY_(cur_str,                                   tree_node_dtor(tree->Groot););

    sscanf(str, "%*[ \t]%s", direction);
    TREE_ERROR_HANDLE(fill_tree_from_file_recursive_(tree, tree->Groot, &cur_str, false));

    return TREE_ERROR_SUCCESS;
}

static enum TreeError init_str_size_from_file_ (size_t* const str_size, const int fd);

static enum TreeError init_str_from_file_(const char* const input_filename, 
                                          char** const str, size_t* const str_size)
{
    lassert(!is_invalid_ptr(input_filename), "");
    lassert(!is_invalid_ptr(str), "");
    lassert(str_size, "");

    int fd = open(input_filename, O_RDWR);
    if (fd == -1)
    {
        perror("Can't fopen input file");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    TREE_ERROR_HANDLE(init_str_size_from_file_(str_size, fd));

    *str = mmap(NULL, *str_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);

    if (*str  == MAP_FAILED)
    {
        perror("Can't mmap");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    if (close(fd))
    {
        perror("Can't fclose input file");
        return TREE_ERROR_STANDARD_ERRNO;
    }
    IF_DEBUG(fd = -1;)

    (*str)[*str_size - 1] = '\0';

    return TREE_ERROR_SUCCESS;
}

static enum TreeError init_str_size_from_file_ (size_t* const str_size, const int fd)
{
    lassert(str_size, "");
    lassert(fd != -1, "");

    struct stat fd_stat = {};

    if (fstat(fd, &fd_stat))
    {
        perror("Can't fstat");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    *str_size = (size_t)fd_stat.st_size + 1;

    return TREE_ERROR_SUCCESS;
}

static enum TreeError fill_tree_from_file_recursive_(tree_t* const tree, tree_node_t* const parent, 
                                                     char** cur_str, const bool is_lt)
{
    TREE_VERIFY(tree, NULL);
    lassert(!is_invalid_ptr(parent), "");
    lassert(!is_invalid_ptr(cur_str), "");

    char* const prev_str = *cur_str;

    STRCHR_NEW_STR_VERIFY_(*cur_str);

    sscanf(*cur_str, "%*[ \t]{");

    if ((*cur_str)[strchr(*cur_str, '\n') - *cur_str - 1] != '{')
    {
        *cur_str = prev_str;
        is_lt ? (parent->lt = NULL) : (parent->rt = NULL);
        return TREE_ERROR_SUCCESS;
    }


    STRCHR_NEW_STR_VERIFY_(*cur_str);

    char data[MAX_DATA_SIZE_] = {};
    if (sscanf(*cur_str, "%*[ \t]'%[^']'", data) == 0)
    {
        fprintf(stderr, "Can't sscanf\n");
        return TREE_ERROR_STANDARD_ERRNO;
    }

    tree_node_t* node = tree_node_ctor(data, MAX_DATA_SIZE_);
    if (!node)
    {
        fprintf(stderr, "Can't tree_node_ctor\n");
        return TREE_ERROR_NODE_IS_NVALID;
    }

    is_lt ? (parent->lt = node) : (parent->rt = node);

    ++tree->size;


    char direction[DIRECTION_SIZE_] = {};

    STRCHR_NEW_STR_VERIFY_(*cur_str,                                         tree_node_dtor(node););

    sscanf(*cur_str, "%*[ \t]%s", direction);
    TREE_ERROR_HANDLE(
        fill_tree_from_file_recursive_(tree, (is_lt ? parent->lt : parent->rt), cur_str, true), 
                                                                               tree_node_dtor(node);
    );

    STRCHR_NEW_STR_VERIFY_(*cur_str,                                         tree_node_dtor(node););

    sscanf(*cur_str, "%*[ \t]%s", direction);
    TREE_ERROR_HANDLE(
        fill_tree_from_file_recursive_(tree, (is_lt ? parent->lt : parent->rt), cur_str, false), 
                                                                               tree_node_dtor(node);
    );

    STRCHR_NEW_STR_VERIFY_(*cur_str,                                         tree_node_dtor(node););

    sscanf(*cur_str, "%*[ \t]}");

    return TREE_ERROR_SUCCESS;
}
#undef DIRECTION_SIZE_
#undef MAX_DATA_SIZE_


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

tree_node_t* tree_find_node_recursive_(const tree_node_t* const node, const void* const data, 
                                       const size_t size);

tree_node_t* tree_find_node(const tree_t* const tree, const void* const data, const size_t size)
{
    TREE_VERIFY(tree, NULL);
    lassert(!is_invalid_ptr(data), "");
    lassert(size, "");

    return tree_find_node_recursive_(tree->Groot, data, size);
}

tree_node_t* tree_find_node_recursive_(const tree_node_t* const node, const void* const data, 
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
    if (fprintf(out, "data = '%s'\n", data_str) <= 0)
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