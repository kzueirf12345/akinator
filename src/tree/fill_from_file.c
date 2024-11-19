#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "tree.h"
#include "logger/liblogger.h"
#include "utils/utils.h"

#define  MAX_DATA_SIZE_ 256
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


static enum TreeError init_str_from_file_           (const char* const input_filename, 
                                                     char** const str, size_t* const str_size);
static enum TreeError init_str_size_from_file_      (size_t* const str_size, const int fd);

static enum TreeError fill_tree_from_str_           (tree_t* const tree, char* str);
static enum TreeError fill_tree_from_str_recursive_(tree_t* const tree, tree_node_t* const parent, 
                                                     char** cur_str, const bool is_lt);

enum TreeError fill_tree_from_file(tree_t* const tree, const char* const input_filename)
{
    TREE_VERIFY(tree, NULL);
    lassert(!is_invalid_ptr(input_filename), "");
    lassert(tree->size == 0, "");

    size_t str_size = 0;
    char* str = NULL;
    TREE_ERROR_HANDLE(init_str_from_file_(input_filename, &str, &str_size));

    TREE_ERROR_HANDLE(fill_tree_from_str_(tree, str));

    return TREE_ERROR_SUCCESS;
}

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

    *str = mmap(NULL, *str_size, PROT_READ, MAP_PRIVATE, fd, 0);

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

static enum TreeError fill_tree_from_str_(tree_t* const tree, char* str)
{
    TREE_VERIFY(tree, NULL);
    lassert(str, "");

    char* cur_str = str;

    int count_scan = 0;
    sscanf(cur_str, "{%n", &count_scan);

    if (count_scan != 1)
        return TREE_ERROR_SUCCESS;


    STRCHR_NEW_STR_VERIFY_(cur_str);

    char data[MAX_DATA_SIZE_] = {};
    if (sscanf(cur_str, "%*[ \t]'%[^']'", data) <= 0)
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
    TREE_ERROR_HANDLE(fill_tree_from_str_recursive_(tree, tree->Groot, &cur_str, true));

    STRCHR_NEW_STR_VERIFY_(cur_str,                                   tree_node_dtor(tree->Groot););

    sscanf(str, "%*[ \t]%s", direction);
    TREE_ERROR_HANDLE(fill_tree_from_str_recursive_(tree, tree->Groot, &cur_str, false));

    return TREE_ERROR_SUCCESS;
}

static enum TreeError fill_tree_from_str_recursive_(tree_t* const tree, tree_node_t* const parent, 
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
        fill_tree_from_str_recursive_(tree, (is_lt ? parent->lt : parent->rt), cur_str, true), 
                                                                               tree_node_dtor(node);
    );

    STRCHR_NEW_STR_VERIFY_(*cur_str,                                         tree_node_dtor(node););

    sscanf(*cur_str, "%*[ \t]%s", direction);
    TREE_ERROR_HANDLE(
        fill_tree_from_str_recursive_(tree, (is_lt ? parent->lt : parent->rt), cur_str, false), 
                                                                               tree_node_dtor(node);
    );

    STRCHR_NEW_STR_VERIFY_(*cur_str,                                         tree_node_dtor(node););

    sscanf(*cur_str, "%*[ \t]}");

    return TREE_ERROR_SUCCESS;
}

#undef DIRECTION_SIZE_
#undef MAX_DATA_SIZE_