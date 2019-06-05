#ifndef canrxbuffer_H
#define canrxbuffer_H
#include "cantype.h"

#define CanMessageBufferLength  0xFF
#define CanMessageBufferEmpty   0x01
#define CanMessageBufferFull    0xFF
#define CanMessageBufferUsed    0x02
#define CanMessageBufferError   0x55
#define CanMessageBufferOK      0x00
#define CanMessageBufferOV      0x03   
#define CanMessageBufferWork    0x04



BYTE can_MsgBufferWrite(CanMsgType_t *MsgPtr,BYTE ChIndex);
BYTE can_MsgBufferGet(CanMsgType_t *MsgPtr,BYTE ChIndex);
BYTE can_MsgBufferInit(void);
BYTE can_MsgBufferGetWork(CanMsgType_t *MsgPtr,DWORD *LengthPtr,BYTE ChIndex);
#endif