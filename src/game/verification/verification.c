#include "verification.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* game_strerror(const enum GameError error)
{
    switch(error)
    {
        CASE_ENUM_TO_STRING_(GAME_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(GAME_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(GAME_ERROR_TREE);
        CASE_ENUM_TO_STRING_(GAME_ERROR_STACK);
        CASE_ENUM_TO_STRING_(GAME_ERROR_VOICING);
        CASE_ENUM_TO_STRING_(GAME_ERROR_TI_PIDOR);
        default:
            return "UNKNOWN_GAME_ERROR";
    }
    return "UNKNOWN_GAME_ERROR";
}
#undef CASE_ENUM_TO_STRING_