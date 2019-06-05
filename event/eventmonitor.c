#include "eventmonitor.h"
#include "eventtype.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../timer/nmtime.h"
#include "eventdatabase.h"
#include "../cfg/cfg.h"
#include "../can/cansignalparse.h"
#include "../log/ascname.h"
#include "../log/nmsyslog.h"
#include "../log/nmsysdiag.h"
#include "eventbuffer.h"
#include "../can/canasc.h"
#include "eventbuffer.h"

static FILE *eventfp=NULL;
static EventDataStsType_t *EventDataStsPtr=NULL;
static EventDataType_t *EventDataPtr=NULL;
static WORD EventDataNum=0;

static void eventCanMsgPrint_Head(BYTE ChIndex)
{
    BYTE eventstsindextemp;
    char *filenametemp;
    filenametemp=getascname(eventfile);
    eventfp=fopen(filenametemp,"w");
	fprintf(eventfp, "%s\n","This is the event log file to record all events");
    setbuf(eventfp,NULL);
}



static BYTE eventcompare(double V1, double V2, BYTE operateIndex)
{
    BYTE resulttemp;
    resulttemp=0;
    switch (operateIndex){
        case eo_equal: 
            if(V1==V2) resulttemp=1;
            break;
        case eo_more: 
            if(V1>V2) resulttemp=1;
            break;
        case eo_less: 
            if(V1<V2) resulttemp=1;
            break;
        case eo_moreeq: 
            if(V1>=V2) resulttemp=1;
            break;
        case eo_lesseq: 
            if(V1<=V2) resulttemp=1;
            break;
        case eo_noteq: 
            if(V1!=V2) resulttemp=1;
            break;
        default:
            resulttemp=0;
            break;
    }
    return resulttemp;
}

void eventcansigcheck(SigValueType_t *sigvalue_temp)
{
    
    EventDataType_t *EvDaPtr_l=NULL;
    EventSigType_t *EvSigPtr_l=NULL;
    WORD EventNumtemp;
    double actV,thrV;
    DWORD sigIdtemp;
    DWORD EventTimeS_temp,EventTimeUs_temp;
    DWORD EventTimeS_Result,EventTimeUs_Result;
    BYTE ResultNum;
    BYTE LastResult;
    BYTE LastRL;
    EvDaPtr_l=EventDataPtr;
    while(EvDaPtr_l!=NULL)
    {
        
        EventNumtemp=EvDaPtr_l->EventDataId;
        EvSigPtr_l=EvDaPtr_l->EventSigDataPtr;
        EventTimeS_Result=0;
        EventTimeUs_Result=0;
        ResultNum=0;
        LastResult=0;
        while(EvSigPtr_l!=NULL)
        {   
            sigIdtemp=EvSigPtr_l->SigId_t;
            actV=(sigvalue_temp+sigIdtemp)->SigValue;

            EventTimeS_temp=(sigvalue_temp+sigIdtemp)->SigTimeS_t;
            EventTimeUs_temp=(sigvalue_temp+sigIdtemp)->SigTimeUs_t;
            thrV=EvSigPtr_l->SignalThreshold;
            #ifdef nm_debug
            printf("eventmonitor-signal name: %s\n",(sigvalue_temp+sigIdtemp)->SigName_t);
            printf("acutal value: %f, threshold value: %f, operator: %d\n",actV,thrV,EvSigPtr_l->SignalOperate);
	    printf("signal receive time: %s\n",print_localtime(EventTimeS_temp,EventTimeUs_temp));
            printf("Before result:%d\n",LastResult);
            #endif
            if(!ResultNum){
                LastResult=eventcompare(actV,thrV,EvSigPtr_l->SignalOperate);
                LastRL=EvSigPtr_l->SignalRelationShip;
            }
            else
            {
                if(LastRL==el_and)
                {
                    LastResult&=eventcompare(actV,thrV,EvSigPtr_l->SignalOperate);
                }
                else
                {
                    LastResult|=eventcompare(actV,thrV,EvSigPtr_l->SignalOperate);
                }
                if(EvSigPtr_l->SignalRelationShip!=el_null)
                {
                    LastRL=EvSigPtr_l->SignalRelationShip;
                }
            }
            
            if((EventTimeS_temp > EventTimeS_Result) && (EventTimeUs_temp>EventTimeUs_Result))
            {
                EventTimeS_Result=EventTimeS_temp;
                EventTimeUs_Result=EventTimeUs_temp;
            }
            #ifdef nm_debug
            printf(", after result:%d\n",LastResult);
            #endif
            EvSigPtr_l=EvSigPtr_l->Next;
            ResultNum++;
        }
        if(LastResult)
        {
            #ifdef nm_debug
            printf("EventID: %d\n",EvDaPtr_l->EventDataId);
            #endif
            if((EventDataStsPtr+EvDaPtr_l->EventDataId-1)->EventStatus!=es_TRUE)
            {
                (EventDataStsPtr+EvDaPtr_l->EventDataId-1)->EventStatus=es_TRUE;
                (EventDataStsPtr+EvDaPtr_l->EventDataId-1)->EventTimeS_t=EventTimeS_Result;
                (EventDataStsPtr+EvDaPtr_l->EventDataId-1)->EventTimeUs_t=EventTimeUs_Result;
                fprintf(eventfp, "%s: Event ID %d is active\n",print_localtime(EventTimeS_Result,EventTimeUs_Result),EvDaPtr_l->EventDataId);
                fflush(eventfp);
            }

        }
        else
        {
            if((EventDataStsPtr+EvDaPtr_l->EventDataId-1)->EventStatus==es_TRUE)
            {
                (EventDataStsPtr+EvDaPtr_l->EventDataId-1)->EventStatus=es_FAULSE;
                (EventDataStsPtr+EvDaPtr_l->EventDataId-1)->EventTimeS_t=0;
                (EventDataStsPtr+EvDaPtr_l->EventDataId-1)->EventTimeUs_t=0;
                fprintf(eventfp, "%s: Event ID %d is inactive\n",print_localtime(EventTimeS_Result,EventTimeUs_Result),EvDaPtr_l->EventDataId);
                fflush(eventfp);
            }

        }
        EvDaPtr_l=EvDaPtr_l->Next;
    }
}

static WORD eventcansiginitial(EventDataType_t *EvDaPtr_t)
{
    EventDataType_t *EvDaPtr_l;
    EventSigType_t *EvSigPtr_l;
    WORD EventNumtemp;
    EventNumtemp=0;
    EvDaPtr_l=EvDaPtr_t;
    while(EvDaPtr_l!=NULL)
    {

        EventNumtemp++;
        EvSigPtr_l=EvDaPtr_l->EventSigDataPtr;
        while(EvSigPtr_l!=NULL)
        {   
            EvSigPtr_l->SigId_t=cansignalidget(EvSigPtr_l->SigName_t,EvSigPtr_l->FrameId,peakcanHW);
            EvSigPtr_l=EvSigPtr_l->Next;
        }
        EvDaPtr_l=EvDaPtr_l->Next;
    }
    return EventNumtemp;
}

static void * pthread_eventmonitor(void *arg)
{
    BYTE eventstsindextemp;
    DWORD ts_temp,tsus_temp;
    BYTE eventstatus;
    BYTE eventnum;
    DWORD i;
    DWORD closedelay;
    FILE *fp1=NULL;
    char *filenametemp;
    eventquenodetype_t eventnodetemp;
    eventstatus=0;
    closedelay=0;
    threadnum--;
    while(threadnum);
 #ifdef eventreport
            filenametemp=getascname(eventrawfile);
            fp1=fopen(filenametemp,"w");
	        fprintf(fp1, "%s\n",asc_EventHeadPrint());
            setbuf(fp1,NULL);
	        fprintf(fp1, "%s\n",asc_ConstPrint());
            fflush(fp1);
#endif
    eventCanMsgPrint_Head(0);
    nmsyslog("ok: Event Monitor Thread starts");
    
    while(exitflag)
    {
        eventnum=0;
        for(eventstsindextemp=0;eventstsindextemp<EventDataNum;eventstsindextemp++)
        {
            if((EventDataStsPtr+eventstsindextemp)->EventStatus==es_TRUE)
            {
                eventnum++;
                ts_temp=(EventDataStsPtr+eventstsindextemp)->EventTimeS_t;
                tsus_temp=(EventDataStsPtr+eventstsindextemp)->EventTimeUs_t;
 
            }
        }
        
        #ifdef eventreport
        if((eventstatus==0)&&(eventnum>0))
        {
            eventstatus=1;

            while(eventbuffer_read(&eventnodetemp)==0)
            {
                for(i=0;i<eventnodetemp.CanMessageBufferUsedLen;i++)
                {
                    fprintf(fp1, "%s\n",asc_EventMessagePrint(&eventnodetemp.CanMessageBuffer[i],peakcanHW));
                    fflush(fp1);
                    
                }
            }
        }
        else if((eventstatus==1)&&(eventnum>0))
        {
            while(eventbuffer_read(&eventnodetemp)==0)
            {
                for(i=0;i<eventnodetemp.CanMessageBufferUsedLen;i++)
                {
                    fprintf(fp1, "%s\n",asc_EventMessagePrint(&eventnodetemp.CanMessageBuffer[i],peakcanHW));
                    fflush(fp1);
                   
                }
            }

        }
        else if((eventstatus==1)&&(eventnum==0))
        {
            closedelay++;
            while(eventbuffer_read(&eventnodetemp)==0)
            {
                for(i=0;i<eventnodetemp.CanMessageBufferUsedLen;i++)
                {
                    fprintf(fp1, "%s\n",asc_MessagePrint(&eventnodetemp.CanMessageBuffer[i],peakcanHW));
                    fflush(fp1);
                    
                }
            }
            if(closedelay>100)
            {
                closedelay=0;
                eventstatus=0;
                
            }
        }
	#endif
	nmsysdiagbeacon(NMBEACON_EVENTMONITOR);
        if(eventstatus==0)
        {
            sleep_ms(1000);
            
        }
        else
        {
            sleep_ms(10);
        }
       

    }
    
    fclose(eventfp);
#ifdef eventreport
    fclose(fp1);
#endif
    nmsyslog("ok: Event Monitor Thread is closed");
}
void eventmonitor_init()
{
    BYTE j;
    WORD i;
    pthread_t tidp;
    EventDataPtr=getEventfromJson(eventcjsonfile0);
    if(EventDataPtr==NULL)
    {
        nmsyslog("error: Event Database Loading Error");
        nmsysdiagset(EVENTDBERROR);
		return;
    }
    EventDataNum=eventcansiginitial(EventDataPtr);
    EventDataStsPtr=(EventDataStsType_t *)malloc(sizeof(EventDataStsType_t)*EventDataNum);
    for(i=0;i<EventDataNum;i++)
    {
        (EventDataStsPtr+i)->EventStatus=es_FAULSE;
        (EventDataStsPtr+i)->EventTimeS_t=0;
        (EventDataStsPtr+i)->EventTimeUs_t=0;
    }

	#ifdef eventreport
    if(eventbuffer_init()<0)
    {
        nmsyslog("error: Event Buffer Initialization Error");
        return;
    }
	#endif
    if ((pthread_create(&tidp, NULL, pthread_eventmonitor,NULL)) == -1)
	{
        nmsyslog("error: Event Monitor Thread can't be created");
		return;
	}
}
