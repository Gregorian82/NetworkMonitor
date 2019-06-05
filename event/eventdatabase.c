#include <stdio.h>
#include <stdlib.h>
#include "../cjson/cJSON.h"
#include "../can/cantype.h"
#include "../cfg/cfg.h"
#include "eventdatabase.h"
#include "../log/nmsyslog.h"

EventDataType_t * geteventfromJson(cJSON * root)
{
  int sigLen,i;
  cJSON *item;
  cJSON *sigobj;
  cJSON *sigContent;
  char *stringt;
  WORD EventIdTemp;
  WORD EventIdCheck;
  char *operatortemp;
  EventDataType_t *eptr_t;
  EventDataType_t *eptr;
  EventSigType_t  *sptr_t;
  EventSigType_t  *sptr;
  item = cJSON_GetObjectItem(root, "Sheet1");
  if (item==NULL)
  {
    nmsyslog("error: event element format in Event Configuration File is not correct");
    return NULL;
  }
  sigLen=cJSON_GetArraySize(item);
  eptr_t=(EventDataType_t *)malloc(sizeof(EventDataType_t));
  sptr_t=(EventSigType_t *)malloc(sizeof(EventSigType_t));
  sptr_t->Next=NULL;
  eptr_t->Next=NULL;
  EventIdTemp=0;
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
        nmsyslog("error: event element content in Event Configuration File is not correct");
        return NULL;
      }
      sigContent=cJSON_GetObjectItem(sigobj, "EventID");
      if(sigContent->type != cJSON_Number)
      { 
        nmsyslog("error: event element type 'EventID' in Event Configuration File is not correct");
        return NULL;
      }

      EventIdCheck=sigContent->valueint;



      sigContent=cJSON_GetObjectItem(sigobj, "SignalName");
      if (sigContent->type != cJSON_String)
      {
        nmsyslog("error: event element type 'SignalName' in Event Configuration File is not correct");
        return NULL;
      }
      sptr_t->SigName_t=sigContent->valuestring;
      #ifdef print_debug
      printf("%s\n", sptr_t->SigName_t);
      #endif
      sigContent=cJSON_GetObjectItem(sigobj, "Operator");
      if (sigContent->type != cJSON_String)
      {
        nmsyslog("error: event element type 'Operator' in Event Configuration File is not correct");
        return NULL;
      }
      operatortemp=sigContent->valuestring;
      if ((*(operatortemp)=='=')&&(*(operatortemp+1)=='=')) {
          sptr_t->SignalOperate=eo_equal;
      }
      else if ((*(operatortemp)=='<')&&(*(operatortemp+1)=='=')){
        sptr_t->SignalOperate=eo_lesseq;
      }
      else if ((*(operatortemp)=='<')&&(*(operatortemp+1)!='=')){
        sptr_t->SignalOperate=eo_less;
      }
      else if ((*(operatortemp)=='>')&&(*(operatortemp+1)!='=')){
        sptr_t->SignalOperate=eo_more;
      }
      else if ((*(operatortemp)=='>')&&(*(operatortemp+1)=='=')){
        sptr_t->SignalOperate=eo_moreeq;
      }
      else if ((*(operatortemp)=='!')&&(*(operatortemp+1)=='=')){
        sptr_t->SignalOperate=eo_noteq;
      }
      else {
        sptr_t->SignalOperate=eo_null;
      }
      #ifdef print_debug
      printf("Operator Code: %d\n", sptr_t->SignalOperate);
      #endif
      sigContent=cJSON_GetObjectItem(sigobj, "Threathold");
      if (sigContent->type != cJSON_Number)
      {
        nmsyslog("error: event element type 'Threathold' in Event Configuration File is not correct");
        return NULL;
      }
      sptr_t->SignalThreshold=sigContent->valuedouble;
      #ifdef print_debug
      printf("Threshold Value: %f\n", sptr_t->SignalThreshold);
      #endif
      sigContent=cJSON_GetObjectItem(sigobj, "Relationship");
      if (sigContent->type != cJSON_String)
      {
        nmsyslog("error: event element type 'Relationship' in Event Configuration File is not correct");
        return NULL;
      }
      operatortemp=sigContent->valuestring;

      if ((*(operatortemp)=='A') && (*(operatortemp+1)=='N')){
        sptr_t->SignalRelationShip=el_and;
      }
      else if((*(operatortemp)=='O')&&(*(operatortemp+1)=='R')){
        sptr_t->SignalRelationShip=el_or;
      }
      else{
        sptr_t->SignalRelationShip=el_null;
      }

      #ifdef print_debug
      printf("Relationship Definition: %d\n", sptr_t->SignalRelationShip);
      #endif

      sigContent=cJSON_GetObjectItem(sigobj, "MessageName");
      if (sigContent->type != cJSON_String)
      {
        nmsyslog("error: event element type 'MessageName' in Event Configuration File is not correct");
        return NULL;
      }
      sptr_t->FrameName=sigContent->valuestring;  //motorola, is_big_endian
      #ifdef print_debug
      printf("Frame Name: %s\n", sptr_t->FrameName);
      #endif
      sigContent=cJSON_GetObjectItem(sigobj, "MessageID");
      if (sigContent->type != cJSON_String)
      {
        nmsyslog("error: event element type 'MessageID' in Event Configuration File is not correct");
        return NULL;
      }
      sptr_t->FrameId=strtol(sigContent->valuestring,NULL,16);  //motorola, is_big_endian
      #ifdef print_debug
      printf("Frame ID: %d\n", sptr_t->FrameId);
      #endif
      if((EventIdCheck!=EventIdTemp)&&(EventIdCheck>0))
      {
        if(EventIdCheck>1){
        eptr_t->EventDataId=EventIdTemp;
        eptr_t->EventSigDataPtr=sptr;
        eptr=eptr_t;
        eptr_t=(EventDataType_t *)malloc(sizeof(EventDataType_t));
        eptr_t->EventSigDataPtr=sptr;
        eptr_t->Next=eptr;
        sptr_t->Next=NULL;
        }
        EventIdTemp=EventIdCheck;

        #ifdef print_debug
        printf("Event ID: %d\n", eptr_t->EventDataId);
        #endif
      }
        sptr=sptr_t;
        sptr_t=(EventSigType_t *)malloc(sizeof(EventSigType_t));
        sptr_t->Next=sptr;
        if(i==(sigLen-1))
        {
          eptr_t->EventDataId=EventIdTemp;
          eptr_t->EventSigDataPtr=sptr;
        }

    }
  }
  else
  {
    nmsyslog("error: no event element in Event Configuration File");
    return NULL;
  }
  
    free(sptr_t);
    return eptr_t;
}


EventDataType_t *getEventfromJson(char *cjsonfilepath)
{
    FILE *f;
    int MsgLen,len;
    char *content;
    cJSON *json;
    int i;
    static EventDataType_t *msgdataptr=NULL;
    EventDataType_t *msgdataptr_t;
    EventSigType_t *signaldataptr;
    f=fopen(cjsonfilepath,"rb");
    if(f==NULL)
    {
      nmsyslog("error: Event Configuration file can't be open");
      return NULL;
    }
    fseek(f,0,SEEK_END);
    len=ftell(f);
    fseek(f,0,SEEK_SET);
    content=(char *)malloc(len+1);
    fread(content,1,len,f);
    fclose(f);


    json=cJSON_Parse(content);
    if (!json) {
      nmsyslog("error: Event Configuration file is invalid");
      return NULL;
    }
    msgdataptr=geteventfromJson(json);
    return msgdataptr;

}
