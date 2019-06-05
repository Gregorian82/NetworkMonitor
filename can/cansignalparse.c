#include "candatabase.h"
#include "cansignalparse.h"
#include "../timer/nmtime.h"
#include "../cfg/cfg.h"
#include "../event/eventmonitor.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../log/nmsyslog.h"
#include "../log/nmsysdiag.h"
#ifdef nm_debug
#include <time.h>
#endif

#define msgsizeforsignalparse   64
#define candatabasestatus_idle  0
#define candatabasestatus_busy  1


const BYTE byteOrder[]={7,6,5,4,3,2,1,0};



typedef struct CanMsgBuffer4Sig{
    CanMsgType_t MsgBuffer[msgsizeforsignalparse];
    BYTE MsgBufferIndex;
    BYTE MsgBufferStatus;
}CanMsgBuffer4Sig_t;

typedef struct CanMsg4Sig{
    MsgDataType_t *candatabase;
    CanMsgBuffer4Sig_t candatabuffer;
    BYTE CanDaseBaseStatus;
    DWORD CanSigNum;
}CanMsg4Sig_t;

typedef struct CanSigGroupinMsg{
    SigValueType_t cansiginmsgbuffer[cansigNuminmsg];
    BYTE cansiglen;
}CanSigGroupinMsg_t;

static CanMsg4Sig_t CanDataBase4Sig[CanHwNum];

#if CanHwNum==1
static char *databasefilepath[CanHwNum]={cjsonfile0};
#endif
#if CanHwNum==2
static char *databasefilepath[CanHwNum]={cjsonfile0,cjsonfile1};
#endif
#if CanHwNum==3
static char *databasefilepath[CanHwNum]={cjsonfile0,cjsonfile1,cjsonfile2};
#endif
#if CanHwNum==4
static char *databasefilepath[CanHwNum]={cjsonfile0,cjsonfile1,cjsonfile2,cjsonfile3};
#endif
#if CanHwNum==5
static char *databasefilepath[CanHwNum]={cjsonfile0,cjsonfile1,cjsonfile2,cjsonfile3,cjsonfile4};
#endif


// get the value of signal from onemessage
static double cansignalparse(CanMsgType_t *CanMsgInput_temp, SigDataType_t *CanSigInput_temp)
{
    double valueDOU_temp;
    WORD64 valueINT_temp,valueINT_filter;
    WORD i,j,k;
    WORD st_temp,len_temp;
    WORD st_offset;
    st_temp=CanSigInput_temp->SigPos_t;
    len_temp=CanSigInput_temp->SigLen_t;
    valueINT_temp=0;
    if(CanSigInput_temp->SigFormat_t==MOTOROLA_FORMAT)
    {

        for(i=0;i<7;i++)
        {
            valueINT_temp |= (WORD64) CanMsgInput_temp->MsgData_t[i];
            valueINT_temp <<= 8;
        }
        valueINT_temp |= (WORD64) CanMsgInput_temp->MsgData_t[i];
        i=0;
        j=0;
 
        while((i<8)&&(st_temp>=((i+1)*8)))
        {
            i++;
        }
        st_offset=st_temp-i*8;
        j=8-(i+1);
        valueINT_temp >>=(j*8+st_offset);
        valueINT_filter=1;
        for(k=1;k<len_temp;k++)
        {
            valueINT_filter<<=1;
            valueINT_filter|=1;
        }
        valueINT_temp &= valueINT_filter;

    }
    else
    {   
       for(i=0;i<7;i++)
        {
            valueINT_temp |= (WORD64) CanMsgInput_temp->MsgData_t[7-i];
            valueINT_temp <<= 8;
        }
        valueINT_temp |= (WORD64) CanMsgInput_temp->MsgData_t[7-i];
        i=0;
        j=0;
 
        valueINT_temp >>=st_temp;
        valueINT_filter=1;
        for(k=1;k<len_temp;k++)
        {
            valueINT_filter<<=1;
            valueINT_filter|=1;
        }
        valueINT_temp &= valueINT_filter;
    }
    valueDOU_temp=((double)valueINT_temp) * CanSigInput_temp->SigValRatio_t + CanSigInput_temp->SigValOffset_t;
    return valueDOU_temp;
}


/*return all siganl physical value in one message*/
static CanSigGroupinMsg_t *cansignalProcess(CanMsgType_t *CanMsgInput_temp,MsgDataType_t *CanMsgDataInput_temp,BYTE HWchID_temp)
{
    MsgDataType_t *CanMsgData_temp;
    CanMsgType_t *CanMsg_temp;
    SigDataType_t *CanSig_temp;
    static CanSigGroupinMsg_t CanSigGroup_temp;
    #ifdef nm_debug
    time_t t1;
    struct tm *tm_now;
    #endif
    CanMsgData_temp =   CanMsgDataInput_temp;
    CanMsg_temp     =   CanMsgInput_temp;
    CanSigGroup_temp.cansiglen=0;
    while(CanMsgData_temp!=NULL)
    {   
	if(CanMsgData_temp->MsgId_t==CanMsg_temp->MsgID_t)
        {	
            CanSig_temp=(SigDataType_t *)CanMsgData_temp->SigDataPtr;
            while(CanSig_temp!=NULL)
            {
                CanSigGroup_temp.cansiginmsgbuffer[CanSigGroup_temp.cansiglen].SigValue=cansignalparse(CanMsg_temp,CanSig_temp);
                CanSigGroup_temp.cansiginmsgbuffer[CanSigGroup_temp.cansiglen].SigName_t=CanSig_temp->SigName_t;
                CanSigGroup_temp.cansiginmsgbuffer[CanSigGroup_temp.cansiglen].SigUnit_t=CanSig_temp->SigUnit_t;
                CanSigGroup_temp.cansiginmsgbuffer[CanSigGroup_temp.cansiglen].SigId_t  =CanSig_temp->SigId_t;
                CanSigGroup_temp.cansiginmsgbuffer[CanSigGroup_temp.cansiglen].HwChId_t =HWchID_temp;
                CanSigGroup_temp.cansiginmsgbuffer[CanSigGroup_temp.cansiglen].MsgId_t  =CanMsg_temp->MsgID_t;
                CanSigGroup_temp.cansiginmsgbuffer[CanSigGroup_temp.cansiglen].SigTimeS_t =CanMsg_temp->MsgTimeS_t;
                CanSigGroup_temp.cansiginmsgbuffer[CanSigGroup_temp.cansiglen].SigTimeUs_t=CanMsg_temp->MsgTimeUs_t;
                #ifdef nm_debug
                            time(&t1);
                            tm_now = localtime(&t1);
                            printf("Time: %d:%d:%d",tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
                            printf("  Function: cansignalparse\n Signal Name:%s, Signal Value:%f\n",CanSig_temp->SigName_t,CanSigGroup_temp.cansiginmsgbuffer[CanSigGroup_temp.cansiglen].SigValue);
               	#endif
                
                
                CanSig_temp = CanSig_temp->SigDataNextPtr;
                CanSigGroup_temp.cansiglen++;
            }
        }

        CanMsgData_temp=CanMsgData_temp->MsgDataNextPtr;
    }
    return &CanSigGroup_temp;
}

static void * pthread_signalparse(void *arg)
{
    BYTE i,j,k;

    DWORD CanSigNum_temp;
    CanMsg4Sig_t CanData4Sig_temp;
    SigValueType_t *CanSigValueBuffer=NULL;
    CanSigGroupinMsg_t *CanSigGroup_temp;
    CanMsgType_t *CanMsgPrt_temp;
    MsgDataType_t *CanMsgDatabase_temp;
    DWORD CansigIndex_temp;
    #ifdef nm_debug
    time_t t1;
    struct tm *tm_now;
    #endif
    threadnum--;
    while(threadnum);
    CanSigNum_temp=0;
    CanMsgDatabase_temp=CanDataBase4Sig[peakcanHW].candatabase;
    CanSigNum_temp=CanDataBase4Sig[peakcanHW].CanSigNum;
    
    if(CanSigNum_temp>0)
    {
        CanSigValueBuffer=(SigValueType_t *)malloc(sizeof(SigValueType_t)*CanSigNum_temp);

        nmsyslog("ok: Signal monitor thread starts");
        while(exitflag)
        {
            for(i=0;i<CanHwNum;i++)
            {
                if(CanDataBase4Sig[i].CanDaseBaseStatus!=candatabasestatus_busy)
                {
                    CanDataBase4Sig[i].CanDaseBaseStatus=candatabasestatus_busy;
                    CanData4Sig_temp=CanDataBase4Sig[i];
                    CanDataBase4Sig[i].candatabuffer.MsgBufferIndex=0;
                    if(CanDataBase4Sig[i].candatabuffer.MsgBufferStatus==candatabuffer4sigFull)
                    {
                        CanDataBase4Sig[i].candatabuffer.MsgBufferStatus=candatabuffer4sigNotFull;
                    }
                    CanDataBase4Sig[i].CanDaseBaseStatus=candatabasestatus_idle;
                    //signal process
                    for(j=0;j<CanData4Sig_temp.candatabuffer.MsgBufferIndex;j++)
                    {
                        CanMsgPrt_temp=&(CanData4Sig_temp.candatabuffer.MsgBuffer[j]);
                        #ifdef nm_debug
                            time(&t1);
                            tm_now = localtime(&t1);
                            printf("Time: %d:%d:%d",tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
                            printf("  Function: cansignalparse\n Message ID:%d, Status:%d\n",CanMsgPrt_temp->MsgID_t,CanMsgPrt_temp->MsgStatus_t);
                        #endif
                        CanSigGroup_temp=cansignalProcess(CanMsgPrt_temp,CanMsgDatabase_temp,i);
                        for(k=0;k<CanSigGroup_temp->cansiglen;k++)
                        {
                            CansigIndex_temp=CanSigGroup_temp->cansiginmsgbuffer[k].SigId_t;
                            *(CanSigValueBuffer+CansigIndex_temp)=CanSigGroup_temp->cansiginmsgbuffer[k];
                            //signal value table updated

                        }
          
                        eventcansigcheck(CanSigValueBuffer);
                    
                    }

                }
                

            }
            nmsysdiagbeacon(NMBEACON_SIGNALPARSER);
            sleep_ms(10);
        }
    }
    else
    {
        nmsyslog("error: No Signal by event configuration to be monitored");
    }
    nmsyslog("ok: Signal monitor thread is closed");
}

void cansignalparserWrite(CanMsgType_t *msgdata, BYTE ChIndex)
{
    if(CanDataBase4Sig[ChIndex].CanDaseBaseStatus!=candatabasestatus_busy)
    {
        CanDataBase4Sig[ChIndex].CanDaseBaseStatus=candatabasestatus_busy;
        if(CanDataBase4Sig[ChIndex].candatabuffer.MsgBufferStatus!=candatabuffer4sigFull)
        {
            CanDataBase4Sig[ChIndex].candatabuffer.MsgBuffer[CanDataBase4Sig[ChIndex].candatabuffer.MsgBufferIndex]=*(msgdata);
            CanDataBase4Sig[ChIndex].candatabuffer.MsgBufferIndex++;
        }
        if(CanDataBase4Sig[ChIndex].candatabuffer.MsgBufferIndex>=msgsizeforsignalparse)
        {
            CanDataBase4Sig[ChIndex].candatabuffer.MsgBufferStatus=candatabuffer4sigFull;
        }
        CanDataBase4Sig[ChIndex].CanDaseBaseStatus=candatabasestatus_idle;
    }
}

DWORD cansignalidget(char *signalnametemp,DWORD MsgIDtemp, BYTE hwIndex)
{
    DWORD indextemp;
    indextemp=0;
    MsgDataType_t *CanMsgData_temp;
    SigDataType_t *CanSig_temp;
    if(CanDataBase4Sig[hwIndex].CanSigNum>0)
    {
        CanMsgData_temp=CanDataBase4Sig[hwIndex].candatabase;
        while(CanMsgData_temp!=NULL)
        {
            if(CanMsgData_temp->MsgId_t==MsgIDtemp)
            {
                CanSig_temp=(SigDataType_t *)CanMsgData_temp->SigDataPtr;
                while(CanSig_temp!=NULL)
                {
                    if(strcmp(CanSig_temp->SigName_t,signalnametemp)==0)
                    {
                        indextemp=CanSig_temp->SigId_t;
                        return indextemp;
                    }
                    CanSig_temp = CanSig_temp->SigDataNextPtr;
                }
            }

            CanMsgData_temp=CanMsgData_temp->MsgDataNextPtr;

        }
    }
    else
    {
        indextemp=0xffffffff;
    }
    return indextemp;
}

void cansignalparseInit()
{
    BYTE i;
    pthread_t tidp;
    for(i=0;i<CanHwNum;i++)
    {
        CanDataBase4Sig[i].candatabase = getMessagefromJson(databasefilepath[i]);
        if(CanDataBase4Sig[i].candatabase==NULL)
        {
            nmsyslog("error: CAN Database Initialization Error");
            /* nmsysdiag */
            nmsysdiagset(CANDBCERROR);
		    return;
        }
        CanDataBase4Sig[i].candatabuffer.MsgBufferIndex=0;
        CanDataBase4Sig[i].candatabuffer.MsgBufferStatus=candatabuffer4sigNotFull;
        CanDataBase4Sig[i].CanDaseBaseStatus=candatabasestatus_idle;
        CanDataBase4Sig[i].CanSigNum=getsignalnumber();
    }
    
    if ((pthread_create(&tidp, NULL, pthread_signalparse,NULL)) == -1)
	{
        nmsyslog("error: CAN Signal Parser Initialization Error");
		return;
	}
}

