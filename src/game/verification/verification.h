#ifndef AKINATOR_SRC_GAME_VERIFICATION_VERIFICATION_H
#define AKINATOR_SRC_GAME_VERIFICATION_VERIFICATION_H

#include <assert.h>

enum GameError
{
    GAME_ERROR_SUCCESS          = 0,
    GAME_ERROR_STANDARD_ERRNO   = 1,
    GAME_ERROR_TREE             = 2,
    GAME_ERROR_TI_PIDOR         = 3,
};
static_assert(GAME_ERROR_SUCCESS  == 0);

const char* game_strerror(const enum GameError error);

#define GAME_ERROR_HANDLE(call_func, ...)                                                           \
    do {                                                                                            \
        enum GameError error_handler = call_func;                                                   \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            game_strerror(error_handler));                                          \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)
    

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

#define NODE_DATA_MAX_SIZE 256

#endif /* AKINATOR_SRC_GAME_VERIFICATION_VERIFICATION_H */