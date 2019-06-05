#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../cfg/cfg.h"
#include "canasc.h"


static char *print_line2="base hex  timestamps absolute\ninternal events logged\n// version 10.0.0";
static char *msg_end_print=" Length = 0 BitCount = 0 ID = ";
static char *msg_middle_print="Rx   d ";
static char messagebuffer[128];
static char Eventmessagebuffer[128];
static struct timeval tv;
static struct timeval Eventtv;


char *asc_ConstPrint()
{
  return print_line2;
}
char *asc_EventHeadPrint()
{
  int i,j;
  time_t timenow;
  gettimeofday(&Eventtv,NULL);
  struct tm * timeinfo;
  char buffer0 [64];
  char buffer1 [16];
  char buffer2 [8];
  char *buffer;
  time(&timenow);
  timeinfo = localtime (&timenow);
  strftime (buffer0,sizeof(buffer0),"date %a %b %d %I:%M:%S.",timeinfo );
  strftime (buffer1,sizeof(buffer1),"%p %Y",timeinfo );
  buffer1[0]=buffer1[0]+32;
  buffer1[1]=buffer1[1]+32;
  sprintf(buffer2, "%ld ", Eventtv.tv_usec/1000);
  buffer=(char *) malloc(strlen(buffer1) + strlen(buffer2) + strlen(buffer0) + 1);
  i=0;
  j=0;
  while (*(buffer0+i) != '\0')
  {
		*(buffer+j) = *(buffer0+i);
    j++;
    i++;
	}
  i=0;
  while (*(buffer2+i)!= '\0') {
    *(buffer+j) = *(buffer2+i);
    j++;
    i++;
  }
  i=0;
  while (*(buffer1+i) != '\0') {
    *(buffer+j) = *(buffer1+i);
    j++;
    i++;
  }
  *(buffer+j)='\0';
  return buffer;
}




char *asc_EventMessagePrint(CanMsgType_t *msgt,BYTE ChIndex)
{
  BYTE i;

  char messageID[4];
  char messageData[25];
  char messageBlank[19];
  DWORD secondst;
  double microsecondst;
  double deltaT;
  microsecondst=msgt->MsgTimeUs_t-Eventtv.tv_usec;
  secondst=msgt->MsgTimeS_t-Eventtv.tv_sec;
  deltaT=microsecondst/1000000.0;
  deltaT=deltaT+(double)secondst;
  if(msgt->MsgID_t>255)
  {
	if(msgt->MsgType_t)
	{
         sprintf(messageID, "%d  %03Xx",ChIndex+1,msgt->MsgID_t);
	}
	else
	{
         sprintf(messageID, "%d  %03X",ChIndex+1,msgt->MsgID_t);
	}
  }
  else
  {
	if(msgt->MsgType_t)
	{
    		sprintf(messageID, "%d  %02Xx",ChIndex+1,msgt->MsgID_t);
	}
	else	
	{
    		sprintf(messageID, "%d  %02X",ChIndex+1,msgt->MsgID_t);
	}
  }
  for (i=0;i<(19-strlen(messageID));i++)
  {
      messageBlank[i] = ' ';
  }
  messageBlank[i]='\0';

  sprintf(messageData, "%d ",msgt->MsgLen_t);
  for (i=0;i<msgt->MsgLen_t;i++)
  {
    sprintf((messageData+2+(i)*3), "%02X ",msgt->MsgData_t[i]);
  }
  *(messageData+2+(msgt->MsgLen_t+1)*3)='\0';
  #ifdef print_main
  //printf("%s\n",messageData);
  #endif
  if(deltaT<10.0)
  {
    sprintf(Eventmessagebuffer, "   %f %s%s%s%s%s%d", deltaT, messageID,messageBlank,msg_middle_print,messageData,msg_end_print,msgt->MsgID_t);
  }
  else if(deltaT<100.0)
  {
    sprintf(Eventmessagebuffer, "  %f %s%s%s%s%s%d", deltaT, messageID,messageBlank,msg_middle_print,messageData,msg_end_print,msgt->MsgID_t);
  }
  else if(deltaT<1000.0)
  {
    sprintf(Eventmessagebuffer, " %f %s%s%s%s%s%d", deltaT, messageID,messageBlank,msg_middle_print,messageData,msg_end_print,msgt->MsgID_t);
  }
  else if(deltaT<10000.0)
  {
    sprintf(Eventmessagebuffer, " %f %s%s%s%s%s%d", deltaT, messageID,messageBlank,msg_middle_print,messageData,msg_end_print,msgt->MsgID_t);
  }
  else
  {
    #ifdef print_main
    printf("%s\n","error! logging time overflow");
    #endif
    return NULL;
  }
   if(msgt->MsgType_t)
  {
    sprintf(Eventmessagebuffer, "%sx", Eventmessagebuffer);
  }
  return Eventmessagebuffer;

}


char *asc_MessagePrint(CanMsgType_t *msgt,BYTE ChIndex)
{
  BYTE i;

  char messageID[4];
  char messageData[25];
  char messageBlank[19];
  DWORD secondst;
  double microsecondst;
  double deltaT;
  microsecondst=msgt->MsgTimeUs_t-tv.tv_usec;
  secondst=msgt->MsgTimeS_t-tv.tv_sec;
  deltaT=microsecondst/1000000.0;
  deltaT=deltaT+(double)secondst;
  if(msgt->MsgID_t>255)
  {
    	if(msgt->MsgType_t)
	{
         sprintf(messageID, "%d  %03Xx",ChIndex+1,msgt->MsgID_t);
	}
	else
	{
         sprintf(messageID, "%d  %03X",ChIndex+1,msgt->MsgID_t);
	}
  }
  else
  {
    	if(msgt->MsgType_t)
	{
    		sprintf(messageID, "%d  %02Xx",ChIndex+1,msgt->MsgID_t);
	}
	else	
	{
    		sprintf(messageID, "%d  %02X",ChIndex+1,msgt->MsgID_t);
	}
  }
  for (i=0;i<(19-strlen(messageID));i++)
  {
      messageBlank[i] = ' ';
  }
  messageBlank[i]='\0';

  sprintf(messageData, "%d ",msgt->MsgLen_t);
  for (i=0;i<msgt->MsgLen_t;i++)
  {
    sprintf((messageData+2+(i)*3), "%02X ",msgt->MsgData_t[i]);
  }
  *(messageData+2+(msgt->MsgLen_t+1)*3)='\0';
  #ifdef print_main
  //printf("%s\n",messageData);
  #endif
  if(deltaT<10.0)
  {
    sprintf(messagebuffer, "   %f %s%s%s%s%s%d", deltaT, messageID,messageBlank,msg_middle_print,messageData,msg_end_print,msgt->MsgID_t);
  }
  else if(deltaT<100.0)
  {
    sprintf(messagebuffer, "  %f %s%s%s%s%s%d", deltaT, messageID,messageBlank,msg_middle_print,messageData,msg_end_print,msgt->MsgID_t);
  }
  else if(deltaT<1000.0)
  {
    sprintf(messagebuffer, " %f %s%s%s%s%s%d", deltaT, messageID,messageBlank,msg_middle_print,messageData,msg_end_print,msgt->MsgID_t);
  }
  else if(deltaT<10000.0)
  {
    sprintf(messagebuffer, " %f %s%s%s%s%s%d", deltaT, messageID,messageBlank,msg_middle_print,messageData,msg_end_print,msgt->MsgID_t);
  }
  else
  {
    #ifdef print_main
    printf("%s\n","error! logging time overflow");
    #endif
    return NULL;
  }
  if(msgt->MsgType_t)
  {
    sprintf(messagebuffer, "%sx", messagebuffer);
  }
  return messagebuffer;

}

char *asc_HeadPrint()
{
  int i,j;
  time_t timenow;
  gettimeofday(&tv,NULL);
  struct tm * timeinfo;
  char buffer0 [64];
  char buffer1 [16];
  char buffer2 [8];
  char *buffer;
  time(&timenow);
  timeinfo = localtime (&timenow);
  strftime (buffer0,sizeof(buffer0),"date %a %b %d %I:%M:%S.",timeinfo );
  strftime (buffer1,sizeof(buffer1),"%p %Y",timeinfo );
  buffer1[0]=buffer1[0]+32;
  buffer1[1]=buffer1[1]+32;
  sprintf(buffer2, "%ld ", tv.tv_usec/1000);
  buffer=(char *) malloc(strlen(buffer1) + strlen(buffer2) + strlen(buffer0) + 1);
  i=0;
  j=0;
  while (*(buffer0+i) != '\0')
  {
		*(buffer+j) = *(buffer0+i);
    j++;
    i++;
	}
  i=0;
  while (*(buffer2+i)!= '\0') {
    *(buffer+j) = *(buffer2+i);
    j++;
    i++;
  }
  i=0;
  while (*(buffer1+i) != '\0') {
    *(buffer+j) = *(buffer1+i);
    j++;
    i++;
  }
  *(buffer+j)='\0';
  return buffer;
}