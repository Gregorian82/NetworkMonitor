#ifndef peakcanrx_H
#define peakcanrx_H
#include "../can/cantype.h"
CanMsgType_t *PeakCanRead(void);
BYTE PeakCanInit(int baudratetemp);
#endif