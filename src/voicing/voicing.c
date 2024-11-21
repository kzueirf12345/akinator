#include <stdarg.h>

#include "voicing.h"
#include "logger/liblogger.h"
#include "utils/utils.h"

//TODO voicing smth not all it's boring

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* voicing_strerror(const enum VoicingError error)
{
    switch(error)
    {
        CASE_ENUM_TO_STRING_(VOICING_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(VOICING_ERROR_FAILURE);
        default:
            return "UNKNOWN_VOICING_ERROR";
    }
    return "UNKNOWN_VOICING_ERROR";
}
#undef CASE_ENUM_TO_STRING_

#define MAX_SYSTEM_STR_SIZE_ 1000
enum VoicingError voicing(const char* const format, ...)
{
    lassert(!is_invalid_ptr(format), "");
    
#ifdef VOICING_ON

    char temp_str[MAX_SYSTEM_STR_SIZE_] = {};

    // fprintf(stderr, "format: '%s'\n", format);

    va_list args;
    va_start(args, format);

    if (vsnprintf(temp_str, MAX_SYSTEM_STR_SIZE_, format, args) <= 0)
    {
        perror("Can't vsnprintf system_str args");
        return VOICING_ERROR_FAILURE;
    }

    va_end(args);

    // fprintf(stderr, "temp_str: '%s'\n", temp_str);

    char system_str[MAX_SYSTEM_STR_SIZE_] = {};

    if (snprintf(system_str, MAX_SYSTEM_STR_SIZE_, 
                 "echo \"%s\" | festival --tts --language russian", temp_str) <= 0)
    {
        perror("Can't snprintf system_str");
        return VOICING_ERROR_FAILURE;
    }

    if (system(system_str) == -1)
    {
        perror("Can't system system_str");
        return VOICING_ERROR_FAILURE;
    }
#endif /*VOICING_ON*/
    
    return VOICING_ERROR_SUCCESS;
}
#undef MAX_SYSTEM_STR_SIZE_     