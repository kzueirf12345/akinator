#ifndef AKINATOR_SRC_GAME_UTILS_H
#define AKINATOR_SRC_GAME_UTILS_H

#include <assert.h>

enum GameError
{
    GAME_ERROR_SUCCESS          = 0,
    GAME_ERROR_STANDARD_ERRNO   = 1,
    GAME_ERROR_TREE             = 2,
    GAME_ERROR_TI_PIDOR         = 3,
};
static_assert(GAME_ERROR_SUCCESS  == 0);


#define TREE_ERROR_HANDLE_(call_func, ...)                                                          \
    do {                                                                                            \
        enum TreeError error_handler = call_func;                                                   \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            tree_strerror(error_handler));                                          \
            __VA_ARGS__                                                                             \
            return GAME_ERROR_TREE;                                                                 \
        }                                                                                           \
    } while(0)

#endif /*AKINATOR_SRC_GAME_UTILS_H*/