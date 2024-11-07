#include <stdio.h>
#include <getopt.h>

#include "logger/liblogger.h"

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

    lassert(!printf("Hello, world!\n"), "");

//--------------------------------------------------------------------------------------------------

    LOGG_ERROR_HANDLE(logger_dtor());
    return 0;
}