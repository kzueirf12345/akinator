#include "test.h"
#include "tree/tree.h"
#include "flags/flags.h"
#include "utils/utils.h"
#include "game/verification/verification.h"

int compare_int_wrapper(const void* const a, const void* const b);
int compare_int        (const int         a, const int         b);

#define ARR_SIZE 20
enum GameError mode_test(flags_objs_t* const flags_objs)
{
    lassert(!is_invalid_ptr(flags_objs), "");

    int arr[ARR_SIZE] = {};
    for (size_t i = 0; i < ARR_SIZE; ++i)
    {
        arr[i] = rand() % ARR_SIZE * 10;
        // fprintf(stderr, "%d ", arr[i]);
    }
    fprintf(stderr, "\n");

    tree_t tree = {};
    TREE_ERROR_HANDLE_(tree_ctor(&tree, compare_int_wrapper),            
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););

    for (size_t i = 0; i < ARR_SIZE; ++i)
    {
        TREE_ERROR_HANDLE_(tree_insert(&tree, tree_node_ctor(&arr[i], sizeof(int))), 
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););
        printf("out:\n");
        TREE_ERROR_HANDLE_(tree_print_inorder(stdout, &tree, NULL),
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););
        printf("\n");
    }

    FILE* file = fopen(flags_objs->out_filename, "wb");
    if (!file)
    {
        fprintf(stderr, "Can't fopen file\n");
        return GAME_ERROR_STANDARD_ERRNO;
    }

    TREE_ERROR_HANDLE_(tree_print_preorder(file, &tree, NULL),
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););

                                                                                   tree_dtor(&tree);

    return GAME_ERROR_SUCCESS;
}
#undef ARR_SIZE

int compare_int_wrapper(const void* const a, const void* const b)
{
    lassert(!is_invalid_ptr(a), "");
    lassert(!is_invalid_ptr(b), "");

    return compare_int(*(const int*)a, *(const int*)b);
}

int compare_int(const int a, const int b)
{
    if (a <  b) return -1;
    if (a == b) return  0;
    if (a >  b) return  1;

    return -2;
}