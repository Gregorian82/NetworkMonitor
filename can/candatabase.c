#include <stdio.h>
#include <stdlib.h>
#include "../cjson/cJSON.h"
#include "candatabase.h"
#include "../cfg/cfg.h"
#include "../log/nmsyslog.h"

static DWORD signalNumberIndex;

#ifdef print_debug
void printJson(cJSON * root)
{
    int i;
    for(i=0; i<cJSON_GetArraySize(root); i++)   
    {
        cJSON * item = cJSON_GetArrayItem(root, i);
        if(cJSON_Object == item->type)      
            printJson(item);
        else                                
        {
            printf("%s->", item->string);
            printf("%s\n", cJSON_Print(item));
        }
    }
}
#endif

DWORD getsignalnumber()
{
  return signalNumberIndex;
}

SigDataType_t * getSignalfromJson(cJSON * root)
{
  int sigLen,i;
  cJSON *item;
  cJSON *sigobj;
  cJSON *sigContent;
  char *stringt;
  SigDataType_t *sptr_t;
  SigDataType_t *sptr;
  sptr_t=(SigDataType_t *)malloc(sizeof(SigDataType_t));
  sptr_t->SigDataNextPtr=NULL;
  item = cJSON_GetObjectItem(root, "signals");
  if (item==NULL)
  {
    nmsyslog("error: signal parser format in CAN Database is not correct");
    return NULL;
  }
  sigLen=cJSON_GetArraySize(item);
  if(sigLen>0)
  {
    for (i=0;i<sigLen;i++)
    {
      sigobj = cJSON_GetArrayItem(item,i);
      #ifdef print_debug
      printf("%s\n", cJSON_Print(sigobj));
      #endif
      if(sigobj==NULL)
      { 
        nmsyslog("error: signal content in CAN Database is not correct");
        return NULL;
      }
      sptr_t->SigId_t=signalNumberIndex;
      sigContent=cJSON_GetObjectItem(sigobj, "start_bit");
      if (sigContent->type != cJSON_Number)
      {
        nmsyslog("error: signal type 'start_bit' in CAN Database is not correct");
        return NULL;
      }
      sptr_t->SigPos_t=sigContent->valueint;


      sigContent=cJSON_GetObjectItem(sigobj, "name");
      if (sigContent->type != cJSON_String)
      {
        nmsyslog("error: signal type 'name' in CAN Database is not correct");
        return NULL;
      }
      sptr_t->SigName_t=sigContent->valuestring;


      sigContent=cJSON_GetObjectItem(sigobj, "bit_length");
      if (sigContent->type != cJSON_Number)
      {
        nmsyslog("error: signal type 'bit_length' in CAN Database is not correct");
        return NULL;
      }
      sptr_t->SigLen_t=sigContent->valueint;

      sigContent=cJSON_GetObjectItem(sigobj, "factor");
      if (sigContent->type != cJSON_Number)
      {
	if(sigContent->type != cJSON_String)
	{
        	nmsyslog("error: signal type 'factor' in CAN Database is not correct");
        	return NULL;
	}
	else
	{
		sptr_t->SigValRatio_t=atof(sigContent->valuestring);
	}
      }
      else
      {
      	sptr_t->SigValRatio_t=sigContent->valuedouble;
      }

      sigContent=cJSON_GetObjectItem(sigobj, "offset");
      if (sigContent->type != cJSON_Number)
      {
	if(sigContent->type != cJSON_String)
	{
        	nmsyslog("error: signal type 'offset' in CAN Database is not correct");
        	return NULL;
	}
	else
	{
		sptr_t->SigValOffset_t=atof(sigContent->valuestring);
	}
      }
      else
      {
	    sptr_t->SigValOffset_t=sigContent->valuedouble;
      }

      sigContent=cJSON_GetObjectItem(sigobj, "is_big_endian");
      if(sigContent->type==2)
      {
        sptr_t->SigFormat_t=1;  //motorola, is_big_endian
      }
      else
      {
        sptr_t->SigFormat_t=0;  //intel
      }
      sptr=sptr_t;
      sptr_t=(SigDataType_t *)malloc(sizeof(SigDataType_t));
      sptr_t->SigDataNextPtr=sptr;
      signalNumberIndex++;
    }
  }
  else
  {
    /* code */
    nmsyslog("error: no signal in database format");
    return NULL;
  }
  
  free(sptr_t);
  return sptr;
}

#ifdef function_main
#define cjsonfilepath "cfg/can.json"
int main()
#else
MsgDataType_t *getMessagefromJson(char *cjsonfilepath)
#endif
{
    FILE *f;
    int MsgLen,len;
    char *content;
    cJSON *json;
    char messagetemp[256];
    int i;
    MsgDataType_t *msgdataptr=NULL;
    SigDataType_t *signaldataptr;
    MsgDataType_t *msgdataptr_t=NULL;
    cJSON *item = NULL;
    cJSON *msgObj;
    cJSON *msgContent;
    signalNumberIndex=0;
    f=fopen(cjsonfilepath,"rb"); 
    if(f==NULL)
    {
      nmsyslog("error: CAN Database file can't be open");
      return NULL;
    }
    fseek(f,0,SEEK_END);
    len=ftell(f);
    fseek(f,0,SEEK_SET);
    content=(char *)malloc(len+1);
    fread(content,1,len,f);
    fclose(f);
    msgdataptr=(MsgDataType_t *)malloc(sizeof(MsgDataType_t));
    msgdataptr_t=(MsgDataType_t *)malloc(sizeof(MsgDataType_t));
    msgdataptr_t->MsgDataNextPtr=NULL;
    signaldataptr=(SigDataType_t *)malloc(sizeof(SigDataType_t));

    json=cJSON_Parse(content);
    if (!json) {
      nmsyslog("error: CAN Database content is invalid");
      return NULL;
    }

    item = cJSON_GetObjectItem(json, "messages");
    if(item==NULL)
    {
      nmsyslog("error: messages definition is invalid in CAN Database");
      return NULL;
    }
    MsgLen=cJSON_GetArraySize(item);
    if(MsgLen>0)
    {
      for (i=0;i<MsgLen;i++)
      {
        msgObj = cJSON_GetArrayItem(item,i);
        #ifdef print_main
        printf("%s\n", cJSON_Print(msgObj));
        #endif
        if (msgObj==NULL)
        {
          nmsyslog("error: message content in CAN Database is invalid");
          return NULL;
        }
        msgContent=cJSON_GetObjectItem(msgObj, "id");
        if (msgContent->type!=cJSON_Number)
        {
          nmsyslog("error: message type 'id' in CAN Database is invalid");
          return NULL;
        }
        msgdataptr_t->MsgId_t=msgContent->valueint;
        msgContent=cJSON_GetObjectItem(msgObj, "name");
        if (msgContent->type!=cJSON_String)
        {
          nmsyslog("error: message type 'name' in CAN Database is invalid");
          return NULL;
        }
        msgdataptr_t->MsgName_t=msgContent->valuestring;

        signaldataptr=getSignalfromJson(msgObj);
        if (signaldataptr==NULL)
        {
          sprintf(messagetemp, "error: the signals of message '%s' in CAN Database are invalid",msgdataptr_t->MsgName_t);
          nmsyslog(messagetemp);
          return NULL;
        }
        msgdataptr_t->SigDataPtr=signaldataptr;
        msgdataptr=msgdataptr_t;
        msgdataptr_t=(MsgDataType_t *)malloc(sizeof(MsgDataType_t));
        msgdataptr_t->MsgDataNextPtr=msgdataptr;

      }
    }
    else
    {
      /* code */
      nmsyslog("error: no message definition in CAN Database");
      return NULL;
    }
    

    free(msgdataptr_t);
    return msgdataptr;
}
