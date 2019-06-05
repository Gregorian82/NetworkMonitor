#ifndef ascname_H
#define ascname_H
#include "../can/cantype.h"
#define rawfile 0
#define eventfile 1
#define eventrawfile 2
char *getascname(BYTE nametype);
#endif