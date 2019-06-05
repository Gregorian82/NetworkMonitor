#ifndef canasc_H
#define canasc_H
#include "cantype.h"

char *asc_HeadPrint();
char *asc_EventHeadPrint();
char *asc_ConstPrint();
char *asc_MessagePrint(CanMsgType_t *msgt,BYTE ChIndex);
char *asc_EventMessagePrint(CanMsgType_t *msgt,BYTE ChIndex);
#endif