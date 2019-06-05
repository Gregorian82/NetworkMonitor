#include "canrxthread.h"
#include "canasc.h"
#include "canrxbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../peak/peakcanrx.h"
#include "cansignalparse.h"
#include "../log/ascname.h"
#include "../log/nmsyslog.h"
#include "../log/nmsysdiag.h"
#include "../timer/nmtime.h"
#include "../event/eventbuffer.h"
#ifdef nm_debug
#include <time.h>
#endif
#define cyclenumMax 10
static BYTE can_networkstatus;
static BYTE cyclenum;
#ifdef CanHwNum
static int baudratecmd=3;
void writebaudrate(int valuetemp)
{
    baudratecmd=valuetemp;
}


static void * pthread_buffer0(void *arg)
{
    CanMsgType_t *messag_t;
    BYTE i;
    #ifdef nm_debug
    time_t t1;
    struct tm *tm_now;
    #endif
    threadnum--;
    while(threadnum);
    nmsyslog("ok: CAN Rx Thread starts");
    if(PeakCanInit(baudratecmd)==0)
    {   
        nmsyslog("ok: CAN HW Initialization is finished");
    }
    else
    {
        /* nmsysdiag */
        nmsysdiagset(CANHWERROR);
        
    }
    
    cyclenum=0;
    i=0;
    while(exitflag)
    {
        #ifdef peakcanHW
    
        messag_t=PeakCanRead();
        if(!messag_t->MsgStatus_t)
        {      
            cyclenum=0;
            if(i>0)
            {
                if(!can_networkstatus)
                {
                    can_networkstatus=1;
                }
                if(can_MsgBufferWrite(messag_t,peakcanHW))
                {
                    #ifdef nm_debug
                    printf("Can Message Buffer Write Status Error!\n");
                    #endif
                    nmsyslog("error: CAN Message Writing Buffer Error");
                    nmsysdiagset(CANWRBUFFERERROR);
                }
                else
                {
                cansignalparserWrite(messag_t,peakcanHW);
                #ifdef nm_debug
                    time(&t1);
                    tm_now = localtime(&t1);
                    printf("Time: %d:%d:%d",tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
                    printf("  Function: canrxthread\n Message ID:%d, Status:%d\n",messag_t->MsgID_t,messag_t->MsgStatus_t);
                #endif
                #else
                #error "No Define peakcanHW in cfg.h"
                #endif
                nmsysdiagbeacon(NMBEACON_CANRX);
		
                }
            }
        }
        i=1;
    }
    nmsyslog("ok: CAN Rx Thread is closed");
}
#else
#error "No Define CanHwNum in cfg.h"
#endif

static void * pthread_ascprint(void *arg)
{
    FILE *fp=NULL;
    DWORD i,j;

    char *filenametemp;
    CanMsgType_t messag_t[CanMessageBufferLength];
    threadnum--;
    while(threadnum);
    #ifdef nm_debug
    printf("ascprint function start!\n");
    #endif
    filenametemp=getascname(rawfile);
    fp=fopen(filenametemp,"w");
	fprintf(fp, "%s\n",asc_HeadPrint());
    setbuf(fp,NULL);
	fprintf(fp, "%s\n",asc_ConstPrint());
    fflush(fp);
    nmsyslog("ok: CAN RAW Data Record start");
    while(exitflag)
    {
        if(!can_MsgBufferGet(messag_t,peakcanHW))
        {
            for(i=0;i<CanMessageBufferLength;i++)
            {
                fprintf(fp, "%s\n",asc_MessagePrint(&messag_t[i],peakcanHW));
                fflush(fp);
            }
		#ifdef eventreport
            eventbuffer_write(messag_t,CanMessageBufferLength);	
		#endif
        }

        if(can_networkstatus)
        {
            cyclenum++;
            if(cyclenum>cyclenumMax)
            {
                if(!can_MsgBufferGetWork(messag_t,&j,peakcanHW))
                {
                    for(i=0;i<j;i++)
                    {
                        fprintf(fp, "%s\n",asc_MessagePrint(&messag_t[i],peakcanHW));
                        fflush(fp);
                    
                    }
                    #ifdef eventreport
                    eventbuffer_write(messag_t,j);
		    #endif
                }
                cyclenum=0;
                can_networkstatus=0;
            }
            
            
        }
        nmsysdiagbeacon(NMBEACON_CANASC);
        sleep_ms(10);
    }
    fclose(fp);
    nmsyslog("ok: CAN RAW Data Record is closed");
}


void pthread_bufferInit(void)
{
    #ifdef CanHwNum
    pthread_t tidp[CanHwNum];
    #endif
    pthread_t ascdp;
    can_networkstatus=0;//idle
    if ((pthread_create(&ascdp, NULL, pthread_ascprint,NULL)) == -1)
	{
        nmsyslog("error: CAN Raw Data Record Thread can't be created");
		return;
	}
    #ifdef CanHwNum
    if ((pthread_create(&tidp[0], NULL, pthread_buffer0,NULL)) == -1)
	{
        nmsyslog("error: CAN Rx Thread can't be created");
		return;
	}
    #endif
}

