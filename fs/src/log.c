#include <log.h>

#define LOG_FILE "fs.log"
#define PS_NAME "FS"

t_log* fs_log_create()
{
    return log_create(LOG_FILE, PS_NAME, 1, LOG_LEVEL_INFO);
}