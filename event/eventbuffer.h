#ifndef eventbuffer_H
#define eventbuffer_H
#include "../can/cantype.h"
#include "../can/canrxbuffer.h"

#define eventquenodeNum     0x0F
#define eventquenode_EMPTY  0x00
#define eventquenode_USED   0x01


typedef struct eventquenodetype{
    CanMsgType_t CanMessageBuffer[CanMessageBufferLength];
    DWORD CanMessageBufferUsedLen;
    BYTE nodestatus;
}eventquenodetype_t;

int eventbuffer_init(void);
int eventbuffer_write(CanMsgType_t *MessageBufferPtr, DWORD MessageLengthTemp);
int eventbuffer_read(eventquenodetype_t *quenode_t);
int eventbuffer_close(void);

#endif