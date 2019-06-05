#ifndef nmsyslog_H
#define nmsyslog_H
#include "../can/cantype.h"

#define NM_SYSLOG_OK    0
#define NM_SYSLOG_ERROR 1

BYTE nmsyslog(char *logmessage);
#endif