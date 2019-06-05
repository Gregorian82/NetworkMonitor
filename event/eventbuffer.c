#include "eventbuffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../log/nmsyslog.h"
#include "../log/nmsysdiag.h"

static eventquenodetype_t queuenodeptr[eventquenodeNum];
static DWORD queuenodehead;
static DWORD queuenodeend;
static DWORD queuenodeusedNUM;

int eventbuffer_init(void)
{
    DWORD i;
    for(i=0;i<eventquenodeNum;i++)
    {
        queuenodeptr[i].nodestatus=eventquenode_EMPTY;
    }
    queuenodehead=0;
    queuenodeend=0;
    queuenodeusedNUM=0;
    return 0;
}

int eventbuffer_write(CanMsgType_t *MessageBufferPtr, DWORD MessageLengthTemp)
{
    CanMsgType_t* MsgBufferPtr;
    DWORD MsgBufferLen;
    MsgBufferPtr=MessageBufferPtr;
    MsgBufferLen=MessageLengthTemp;
    if(memcpy(queuenodeptr[queuenodeend].CanMessageBuffer,MsgBufferPtr,sizeof(CanMsgType_t)*CanMessageBufferLength)==NULL)
    {
        nmsyslog("error: Event Buffer Writing Error");
        nmsysdiagset(EVENTWRBUFFERERROR);
        return -1;
    }
    queuenodeptr[queuenodeend].CanMessageBufferUsedLen=MsgBufferLen;
    queuenodeptr[queuenodeend].nodestatus=eventquenode_USED;
    queuenodeend++;
    queuenodeusedNUM++;
    if(queuenodeusedNUM>eventquenodeNum)
    {
       queuenodeusedNUM=eventquenodeNum;
        queuenodehead++; 
    }
    if(queuenodehead>=eventquenodeNum)
    {
       queuenodehead=0; 
    }
    if(queuenodeend>=eventquenodeNum)
    {
       queuenodeend=0; 
    }
    return 0;
}

int eventbuffer_read(eventquenodetype_t *quenode_t)
{
    if(queuenodeusedNUM<=0)
    {
        return 1;
    }
    if(queuenodeptr[queuenodehead].nodestatus!=eventquenode_USED)
    {
        nmsyslog("error: Event Reading Buffer is not used");
        return -1;
    }
    if(memcpy(quenode_t,&queuenodeptr[queuenodehead],sizeof(eventquenodetype_t))==NULL)
    {
        nmsyslog("error: Event Buffer Reading Error");
        nmsysdiagset(EVENTRDBUFFERERROR);
        return -1;
    }
    queuenodeptr[queuenodehead].nodestatus=eventquenode_EMPTY;
    queuenodehead++;
    queuenodeusedNUM--;

    if(queuenodehead>=eventquenodeNum)
    {
       queuenodehead=0; 
    }
    return 0;
}

int eventbuffer_close(void)
{
    queuenodehead=0;
    queuenodeend=0;
    queuenodeusedNUM=0;
    return 0;
}
