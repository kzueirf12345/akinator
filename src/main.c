#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "logger/liblogger.h"
#include "tree/tree.h"
#include "utils/utils.h"
#include "verification/verification.h"
#include "dumb/dumb.h"
#include "flags/flags.h"
#include "game/game.h"


int logger_init(char* const log_folder);
int logger_destroy(void);

int main(const int argc, char* const argv[])
{
    if (!setlocale(LC_ALL, "ru_RU.utf8"))
    {
        fprintf(stderr, "Can't setlocale\n");
        return EXIT_FAILURE;
    }

    flags_objs_t flags_objs = {};
    FLAGS_ERROR_HANDLE(flags_objs_ctor(&flags_objs));

    FLAGS_ERROR_HANDLE(flags_processing(&flags_objs, argc, argv));

    if (logger_init(flags_objs.log_folder))
    {
        fprintf(stderr, "Can't logger init\n");
                                                     flags_objs_dtor(&flags_objs); logger_destroy();
        return EXIT_FAILURE;
    }

//--------------------------------------------------------------------------------------------------

    GAME_ERROR_HANDLE(do_akinator(&flags_objs),
                                                   flags_objs_dtor(&flags_objs); logger_destroy(););

//--------------------------------------------------------------------------------------------------

    if (logger_destroy())
    {
        fprintf(stderr, "Can't logger destroy\n");
                                                                       flags_objs_dtor(&flags_objs);
        return EXIT_FAILURE;
    }

    FLAGS_ERROR_HANDLE(flags_objs_dtor(&flags_objs));

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