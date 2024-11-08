#include <stdio.h>
#include <getopt.h>

#include "logger/liblogger.h"
#include "tree/tree.h"
#include "utils.h"

int compare_int_wrapper(const void* const a, const void* const b);
int compare_int        (const int         a, const int         b);

int main(int argc, char* argv[])
{
    const char* logout_filename = "./log/logout.log";

    int getopt_rez = 0;
    while ((getopt_rez = getopt(argc, argv, "l:")) != -1)
    {
        switch (getopt_rez)
        {
        case 'l':
            logout_filename = optarg;
            break;
        
        default:
            fprintf(stderr, "Getopt error - d: %d, c: %c\n", getopt_rez, (char)getopt_rez);
            break;
        }
    }

    LOGG_ERROR_HANDLE(logger_ctor(),                                                logger_dtor(););
    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL),              logger_dtor(););
    LOGG_ERROR_HANDLE(logger_set_logout_file(logout_filename),                      logger_dtor(););

//--------------------------------------------------------------------------------------------------

    tree_t tree = {};
    TREE_ERROR_HANDLE(tree_ctor(&tree, compare_int_wrapper),            
                                                                  tree_dtor(&tree); logger_dtor(););

    int num = 9;
    TREE_ERROR_HANDLE(tree_push_node(&tree, tree_node_ctor(&num, sizeof(int), NULL, NULL)), 
                                                                  tree_dtor(&tree); logger_dtor(););
    num = 10;
    TREE_ERROR_HANDLE(tree_push_node(&tree, tree_node_ctor(&num, sizeof(int), NULL, NULL)), 
                                                                  tree_dtor(&tree); logger_dtor(););

    printf("Hello, world!\n");

    tree_dtor(&tree);

//--------------------------------------------------------------------------------------------------

    LOGG_ERROR_HANDLE(logger_dtor());
    return 0;
}

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
    if (a <  b) return  1;

    return -2;
}