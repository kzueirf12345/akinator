#ifndef ASINATOR_UTILS_UTILS_H
#define ASINATOR_UTILS_UTILS_H

#include <stdio.h>
#include <assert.h>

#include "concole.h"


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#ifndef NDEBUG
#define IF_DEBUG(...) __VA_ARGS__
#define IF_ELSE_DEBUG(smth, other_smth) smth
#else /*NDEBUG*/
#define IF_DEBUG(...)
#define IF_ELSE_DEBUG(smth, other_smth) other_smth
#endif /*NDEBUG*/


enum PtrState
{
    PTR_STATES_VALID   = 0,
    PTR_STATES_NULL    = 1,
    PTR_STATES_INVALID = 2,
    PTR_STATES_ERROR   = 3
};
static_assert(PTR_STATES_VALID == 0);

enum PtrState is_invalid_ptr(const void* ptr);

int is_empty_file (FILE* file);

int data_to_str(const void* const data, const size_t size, char* const * str,
                const size_t str_size);

#define TREE_INOUT_ELEM_CODE "%d"
#define TREE_INOUT_ELEM_T int

#endif /*ASINATOR_UTILS_UTILS_H*/