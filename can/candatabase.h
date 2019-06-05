#ifndef candatabase_H
#define candatabase_H
#include "cantype.h"

MsgDataType_t *getMessagefromJson(char *cjsonfilepath);
DWORD getsignalnumber();

#endif