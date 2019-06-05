#include "canrxbuffer.h"
#include "../cfg/cfg.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef struct CanMessgeBufferType{
  CanMsgType_t CanMessageBuffer[CanMessageBufferLength];
  DWORD CanMessageBufferEndPtr;
  DWORD CanMessageBufferUsedLen;
  DWORD CanMessageBufferMaxLen;
  BYTE Status;
}CanMessgeBufferType_t;
static CanMessgeBufferType_t CanMessageBuffer[2][CanHwNum];
static BYTE emptycycle;

BYTE can_MsgBufferInit(void)
{
  BYTE i;
  for(i=0;i<CanHwNum;i++)
  {
      memset(CanMessageBuffer,0,sizeof(CanMessgeBufferType_t)*CanHwNum*2);
      CanMessageBuffer[0][i].CanMessageBufferMaxLen=CanMessageBufferLength;
      CanMessageBuffer[1][i].CanMessageBufferMaxLen=CanMessageBufferLength;
      CanMessageBuffer[0][i].Status=CanMessageBufferWork;
      CanMessageBuffer[1][i].Status=CanMessageBufferEmpty;
  }
  return CanMessageBufferOK;
}
BYTE can_MsgBufferGetWork(CanMsgType_t *MsgPtr,DWORD *LengthPtr,BYTE ChIndex)
{
  BYTE i,j;
  DWORD getLengtemp;
  CanMsgType_t *MsgTemp;
  i=ChIndex;
  MsgTemp=MsgPtr;
  
  if(CanMessageBuffer[0][i].Status==CanMessageBufferWork)
  {
    j=0;
  }
  else if(CanMessageBuffer[1][i].Status==CanMessageBufferWork)
  {
    j=1;
  }
  else
  {
    return CanMessageBufferError;
  }
  if(MsgTemp==NULL)
  {
    return CanMessageBufferError;
  }
  getLengtemp=CanMessageBuffer[j][i].CanMessageBufferUsedLen;
  *LengthPtr=getLengtemp;
  memcpy(MsgTemp,CanMessageBuffer[j][i].CanMessageBuffer,sizeof(CanMsgType_t)*getLengtemp);
  CanMessageBuffer[j][i].CanMessageBufferEndPtr=0;
  CanMessageBuffer[j][i].CanMessageBufferUsedLen=0;
  CanMessageBuffer[j][i].Status=CanMessageBufferEmpty;
  return CanMessageBufferOK;
}

BYTE can_MsgBufferGet(CanMsgType_t *MsgPtr,BYTE ChIndex)
{
  BYTE i,j;
  DWORD getLengtemp;
  DWORD bufferindextemp0,bufferindextemp1;
  CanMsgType_t *MsgTemp;

  i=ChIndex;
  MsgTemp=MsgPtr;
  if(CanMessageBuffer[0][i].Status==CanMessageBufferFull)
  {
    j=0;
  }
  else if(CanMessageBuffer[1][i].Status==CanMessageBufferFull)
  {
    j=1;
  }
  else
  {
    return CanMessageBufferEmpty;
  }

  if(MsgTemp==NULL)
  {
    return CanMessageBufferError;
  }
  getLengtemp=CanMessageBuffer[j][i].CanMessageBufferMaxLen;
  memcpy(MsgTemp,CanMessageBuffer[j][i].CanMessageBuffer,sizeof(CanMsgType_t)*getLengtemp);
  CanMessageBuffer[j][i].CanMessageBufferEndPtr=0;
  CanMessageBuffer[j][i].CanMessageBufferUsedLen=0;
  CanMessageBuffer[j][i].Status=CanMessageBufferEmpty;
  return CanMessageBufferOK;
}

BYTE can_MsgBufferWrite(CanMsgType_t *MsgPtr,BYTE ChIndex)
{
  BYTE i,j;
  DWORD bufferindextemp;
  i=ChIndex;
  if(CanMessageBuffer[0][i].Status==CanMessageBufferWork)
  {
    j=0;
  }
  else if (CanMessageBuffer[1][i].Status==CanMessageBufferWork)
  {
    j=1;
  }
  else
  {
    if((CanMessageBuffer[0][i].Status==CanMessageBufferEmpty)&&(CanMessageBuffer[1][i].Status==CanMessageBufferEmpty))
    {
      j=0;
      CanMessageBuffer[j][i].Status=CanMessageBufferWork;
    }
    else{
    #ifdef nm_debug
    printf("CanMessageBuffer0 Status:%d, CanMessageBuffer1 Status:%d\n",CanMessageBuffer[0][i].Status,CanMessageBuffer[1][i].Status);
    while(1);
    #endif
    return CanMessageBufferError;
    }
  }
    #ifdef nm_debug
    printf("CanMessageBuffer0 Status:%d, CanMessageBuffer1 Status:%d\n",CanMessageBuffer[0][i].Status,CanMessageBuffer[1][i].Status);
    #endif
  CanMessageBuffer[j][i].CanMessageBufferUsedLen++;
  if(CanMessageBuffer[j][i].CanMessageBufferUsedLen>CanMessageBuffer[j][i].CanMessageBufferMaxLen)
  {
    CanMessageBuffer[j][i].CanMessageBufferUsedLen--;
    CanMessageBuffer[j][i].Status=CanMessageBufferFull;
    j=j^0x01;
    CanMessageBuffer[j][i].Status=CanMessageBufferWork;
    CanMessageBuffer[j][i].CanMessageBufferUsedLen++;
  }
  #ifdef nm_debug
  printf("can message buffer group id:%d, usedlength: %d",j,CanMessageBuffer[j][i].CanMessageBufferUsedLen);
  #endif
  bufferindextemp=CanMessageBuffer[j][i].CanMessageBufferEndPtr;
  CanMessageBuffer[j][i].CanMessageBufferEndPtr++;
  memcpy(&CanMessageBuffer[j][i].CanMessageBuffer[bufferindextemp],MsgPtr,sizeof(CanMsgType_t));
  return CanMessageBufferOK;
}
