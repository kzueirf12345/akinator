#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "utils.h"
#include "logger/liblogger.h"

enum PtrState is_invalid_ptr(const void* ptr)
{
    lassert(!errno, "");

    if (ptr == NULL)
    {
        return PTR_STATES_NULL;
    }

    char filename[] = "/tmp/chupapi_munyanya.XXXXXX";
    const int fd = mkstemp(filename);

    if (fd == -1) 
    {
        perror("Can't mkstemp file");
        return PTR_STATES_ERROR;
    }

    const ssize_t write_result = write(fd, ptr, 1);

    if (remove(filename))
    {
        perror("Can't remove temp file");
        return PTR_STATES_ERROR;
    }

    if (close(fd))
    {
        perror("Can't close temp file");
        return PTR_STATES_ERROR;
    }

    if (write_result == 1) 
    {
        return PTR_STATES_VALID;
    } 
    else if (errno == EFAULT) 
    {
        errno = 0;
        return PTR_STATES_INVALID;
    }
    
    perror("Unpredictable errno state, after write into temp file");
    return PTR_STATES_ERROR;
}