#ifndef cansignalparse_H
#define cansignalparse_H
#include "cantype.h"
#define candatabuffer4sigFull       0x55
#define candatabuffer4sigNotFull    0xAA

void cansignalparseInit();
void cansignalparserWrite(CanMsgType_t *msgdata, BYTE ChIndex);
DWORD cansignalidget(char *signalnametemp,DWORD MsgIDtemp, BYTE hwIndex);
#endif