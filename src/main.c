#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "logger/liblogger.h"
#include "tree/tree.h"
#include "utils.h"
#include "verification/verification.h"
#include "dumb/dumb.h"

#define LOGOUT_FILENAME "logout"
#define   DUMB_FILENAME "dumb"
#define LOG_FOLDER_SIZE 256

int compare_int_wrapper(const void* const a, const void* const b);
int compare_int        (const int         a, const int         b);

int main(int argc, char* argv[])
{
    char log_folder[LOG_FOLDER_SIZE] = "./log/";

    int getopt_rez = 0;
    while ((getopt_rez = getopt(argc, argv, "l:")) != -1)
    {
        switch (getopt_rez)
        {
        case 'l':
            strncpy(log_folder, optarg, LOG_FOLDER_SIZE);
            break;
        
        default:
            fprintf(stderr, "Getopt error - d: %d, c: %c\n", getopt_rez, (char)getopt_rez);
            break;
        }
    }

    char logout_filename[LOG_FOLDER_SIZE] = {};
    if (snprintf(logout_filename, LOG_FOLDER_SIZE, "%s%s", log_folder, LOGOUT_FILENAME) <= 0)
    {
        perror("Can;t snprintf logout_filename");
        return EXIT_FAILURE;
    }

    char dumb_filename[LOG_FOLDER_SIZE] = {};
    if (snprintf(dumb_filename, LOG_FOLDER_SIZE, "%s%s", log_folder, DUMB_FILENAME) <= 0)
    {
        perror("Can;t snprintf dumb_filename");
        return EXIT_FAILURE;
    }


    LOGG_ERROR_HANDLE(logger_ctor(),                                                logger_dtor(););
    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL),              logger_dtor(););
    LOGG_ERROR_HANDLE(logger_set_logout_file(logout_filename),                      logger_dtor(););

    TREE_DUMB_ERROR_HANDLE(tree_dumb_ctor(),                                
                                                                  logger_dtor(); tree_dumb_dtor(););
    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_file(dumb_filename),
                                                                  logger_dtor(); tree_dumb_dtor(););

//--------------------------------------------------------------------------------------------------

#define ARR_SIZE 10
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
        TREE_ERROR_HANDLE(tree_print(stdout, &tree, NULL),
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