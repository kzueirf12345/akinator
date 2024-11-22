#ifndef AKINATOR_SRC_VOICING_VOICING_H
#define AKINATOR_SRC_VOICING_VOICING_H

#include <assert.h>

enum VoicingError
{
    VOICING_ERROR_SUCCESS     = 0,
    VOICING_ERROR_FAILURE     = 1,
};
static_assert(VOICING_ERROR_SUCCESS == 0);

const char* voicing_strerror(const enum VoicingError error);

#define VOICING_ERROR_HANDLE(call_func, ...)                                                        \
    do {                                                                                            \
        enum VoicingError error_handler = call_func;                                                \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't %s. Error: %s\n",                                                \
                             #call_func, voicing_strerror(error_handler));                          \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

enum VoicingError voicing(const char* const format, ...);

//TODO - wrapper va_list

#define VOICINGF(format, ...)                                                                       \
    do                                                                                              \
    {                                                                                               \
        printf(format, ##__VA_ARGS__);                                                              \
        VOICING_ERROR_HANDLE(voicing(format, ##__VA_ARGS__));                                       \
    } while (0)
    

#endif /* AKINATOR_SRC_VOICING_VOICING_H */