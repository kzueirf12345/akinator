#include <stdio.h>
#include <getopt.h>

#include "logger/liblogger.h"
#include "tree/tree.h"
#include "utils.h"
#include "verification/verification.h"
#include "dumb/dumb.h"

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

    TREE_DUMB_ERROR_HANDLE(tree_dumb_ctor(),                                
                                                                  logger_dtor(); tree_dumb_dtor(););
    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_html_file("./log/dumb.html"),  
                                                                  logger_dtor(); tree_dumb_dtor(););
    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_dot_file ("./log/dumb.dot"),   
                                                                  logger_dtor(); tree_dumb_dtor(););
    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_svg_file ("./log/dumb"),   
                                                                  logger_dtor(); tree_dumb_dtor(););

//--------------------------------------------------------------------------------------------------

#define ARR_SIZE 100
    int arr[ARR_SIZE] = {};

    for (size_t i = 0; i < ARR_SIZE; ++i)
    {
        arr[i] = rand() % ARR_SIZE * 10;
        // fprintf(stderr, "%d ", arr[i]);
    }
    fprintf(stderr, "\n");

    tree_t tree = {};
    TREE_ERROR_HANDLE(tree_ctor(&tree, compare_int_wrapper),            
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););

    for (size_t i = 0; i < ARR_SIZE; ++i)
    {
        TREE_ERROR_HANDLE(tree_insert(&tree, tree_node_ctor(&arr[i], sizeof(int))), 
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););
    }
    tree_dumb(&tree, NULL);

    tree_dtor(&tree);

//--------------------------------------------------------------------------------------------------

    LOGG_ERROR_HANDLE(        logger_dtor());
    TREE_DUMB_ERROR_HANDLE(tree_dumb_dtor());
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
    if (a >  b) return  1;

    return -2;
}