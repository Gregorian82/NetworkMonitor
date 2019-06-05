#include <syslog.h>
#include "nmsyslog.h"
BYTE nmsyslog(char *logmessage)
{
    openlog("nm_syslog", LOG_CONS | LOG_PID, 0); 
    syslog(LOG_USER | LOG_INFO, "%s\n",logmessage);
    closelog();
    return NM_SYSLOG_OK;
}

//int main(int argc, char *argv[])
