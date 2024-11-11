#ifndef AKINATOR_SRC_VERIFICATION_VERIFICATION_H
#define AKINATOR_SRC_VERIFICATION_VERIFICATION_H

#include <assert.h>

#include "../tree/tree_structs.h"
#include "logger/liblogger.h"
#include "../dumb/dumb.h"

enum TreeError
{
    TREE_ERROR_SUCCESS              = 0,
    TREE_ERROR_STANDARD_ERRNO       = 1,
    TREE_ERROR_TREE_IS_NULL         = 2,
    TREE_ERROR_TREE_IS_INVALID      = 3,
    TREE_ERROR_COMPARE_IS_NULL      = 4,
    TREE_ERROR_COMPARE_IS_INVALID   = 5,
    TREE_ERROR_NODE_IS_NVALID       = 6,
    TREE_ERROR_SIZE_GREATER         = 7,
    TREE_ERROR_NODE_DATA_IS_NULL    = 8,
    TREE_ERROR_NODE_DATA_IS_INVALID = 9,
    TREE_ERROR_NODE_SIZE_IS_ZERO    = 10,
    TREE_ERROR_SIZE_LESSER          = 11,
    TREE_ERROR_ELEM_TO_STR          = 12,
    TREE_ERROR_UNKNOWN              = 20
};
static_assert(TREE_ERROR_SUCCESS == 0);

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


#ifndef NDEBUG

enum TreeError tree_verify_NOT_USE(const tree_t* const tree);


#define TREE_VERIFY(tree, elem_to_str)                                                              \
        do {                                                                                        \
            const enum TreeError error = tree_verify_NOT_USE(tree);                                 \
            if (error)                                                                              \
            {                                                                                       \
                tree_dumb(tree, elem_to_str);                                                       \
                lassert(false, "Tree error: %s", tree_strerror(error));                             \
            }                                                                                       \
        } while(0)

#else /*NDEBUG*/

#define TREE_VERIFY(tree, elem_to_str) do {} while(0)

#endif /*NDEBUG*/

#endif /*AKINATOR_SRC_VERIFICATION_VERIFICATION_H*/