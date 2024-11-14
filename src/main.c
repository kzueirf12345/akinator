#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <locale.h>

#include "logger/liblogger.h"
#include "tree/tree.h"
#include "utils.h"
#include "verification/verification.h"
#include "dumb/dumb.h"

enum Mode
{
    MODE_GAME           = 0,
    MODE_DEFINITION     = 1,
    MODE_COMPARE        = 2,
    MODE_PRINT          = 3,
    MODE_TEST           = 1488,
};

int logger_init(char* const log_folder);
int logger_destroy(void);

int compare_int_wrapper(const void* const a, const void* const b);
int compare_int        (const int         a, const int         b);

int main(int argc, char* argv[])
{
    if (!setlocale(LC_ALL, "ru_RU.utf8"))
    {
        fprintf(stderr, "Can't setlocale\n");
        return EXIT_FAILURE;
    }

    char log_folder  [FILENAME_MAX_SIZE] = "./log/";
    char out_filename[FILENAME_MAX_SIZE] = "./output.txt";
    char in_filename [FILENAME_MAX_SIZE] = "./input.txt";
    enum Mode mode = MODE_GAME;     
    
    int getopt_rez = 0;
    while ((getopt_rez = getopt(argc, argv, "l:m:o:i:")) != -1)
    {
        switch (getopt_rez)
        {
            case 'l':
            {
                strncpy(log_folder, optarg, FILENAME_MAX_SIZE);
                break;
            }
            case 'm':
            {
                mode = (enum Mode)atoi(optarg);
                break;
            }
            case 'o':
            {
                strncpy(out_filename, optarg, FILENAME_MAX_SIZE);
                break;
            }
            case 'i':
            {
                strncpy(in_filename, optarg, FILENAME_MAX_SIZE);
                break;
            }

            default:
            {
                fprintf(stderr, "Getopt error - d: %d, c: %c\n", getopt_rez, (char)getopt_rez);
                break;
            }
        }
    }

    if (logger_init(log_folder))
    {
        fprintf(stderr, "Can't logger init\n");
        logger_destroy();
        return EXIT_FAILURE;
    }

//--------------------------------------------------------------------------------------------------

    //TODO
    switch (mode)
    {
        case MODE_GAME:
        {

            break;
        }
        case MODE_DEFINITION:
        {

            break;
        }
        case MODE_COMPARE:
        {

            break;
        }
        case MODE_PRINT:
        {
            tree_t tree = {};
            TREE_ERROR_HANDLE(tree_ctor(&tree, compare_int_wrapper), 
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););
            TREE_ERROR_HANDLE(fill_tree_from_file(&tree, "./input.txt"), 
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););
            TREE_ERROR_HANDLE(tree_print_inorder(stdout, &tree, NULL),
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););
            tree_dtor(&tree);
            break;
        }

        case MODE_TEST:
        {
#define ARR_SIZE 20
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
                printf("out:\n");
                TREE_ERROR_HANDLE(tree_print_inorder(stdout, &tree, NULL),
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););
                printf("\n");
            }
            FILE* file = fopen("kekw.txt", "wb");
            if (!file)
            {
                fprintf(stderr, "Can't fopen file\n");
                return EXIT_FAILURE;
            }
            TREE_ERROR_HANDLE(tree_print_preorder(file, &tree, NULL),
                                                tree_dtor(&tree); tree_dumb_dtor(); logger_dtor(););

            tree_dtor(&tree);
            break;
        }
        default:
        {
            fprintf(stderr, "Unknown mode\n");
            break;
        }
    }


//--------------------------------------------------------------------------------------------------

    if (logger_destroy())
    {
        fprintf(stderr, "Can't logger destroy\n");
        return EXIT_FAILURE;
    }

    return 0;
}

#define LOGOUT_FILENAME "logout.log"
#define   DUMB_FILENAME "dumb"
int logger_init(char* const log_folder)
{
    lassert(log_folder, "");

    char logout_filename[FILENAME_MAX_SIZE] = {};
    if (snprintf(logout_filename, FILENAME_MAX_SIZE, "%s%s", log_folder, LOGOUT_FILENAME) <= 0)
    {
        perror("Can't snprintf logout_filename");
        return EXIT_FAILURE;
    }

    char dumb_filename[FILENAME_MAX_SIZE] = {};
    if (snprintf(dumb_filename, FILENAME_MAX_SIZE, "%s%s", log_folder, DUMB_FILENAME) <= 0)
    {
        perror("Can't snprintf dumb_filename");
        return EXIT_FAILURE;
    }


    LOGG_ERROR_HANDLE(logger_ctor());
    LOGG_ERROR_HANDLE(logger_set_level_details(LOG_LEVEL_DETAILS_ALL));
    LOGG_ERROR_HANDLE(logger_set_logout_file(logout_filename));


    TREE_DUMB_ERROR_HANDLE(tree_dumb_ctor());
    TREE_DUMB_ERROR_HANDLE(tree_dumb_set_out_file(dumb_filename));
    
    return 0;
}
#undef LOGOUT_FILENAME
#undef   DUMB_FILENAME

int logger_destroy(void)
{
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