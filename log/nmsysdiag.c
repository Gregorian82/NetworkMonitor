#include "nmsysdiag.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "../timer/nmtime.h"
#include "nmsyslog.h"

#define nmdiagBEACONNUM     10
#define nmdiagBEACONUSED    4
#define nmdiagBEACONMAX     50

static unsigned int nmdiag_errorlist;
static unsigned int nmdiag_beaconvalue[nmdiagBEACONNUM];
static unsigned int nmdiag_beaconerrormap[nmdiagBEACONNUM]={BEACON_CANRXERROR,BEACON_SIGNALPARSERERROR,BEACON_EVENTMONITORERROR,BEACON_CANASCERROR,0,0,0,0,0,0};
void nmsysdiagset(unsigned int index)
{
    if(!(nmdiag_errorlist&index))
    {
        nmdiag_errorlist |= index;
    }
}
void nmsysdiagclear(unsigned int index)
{
    if(nmdiag_errorlist&index)
    {
        nmdiag_errorlist &= (~index);
    }
}

int nmsysdiagstatus(unsigned int index)
{
	return (nmdiag_errorlist & index);
}

void nmsysdiagbeacon(unsigned int index)
{
    if(index<nmdiagBEACONUSED)
    {
        nmdiag_beaconvalue[index]=nmdiagBEACONMAX;
    }
    nmsysdiagclear(nmdiag_beaconerrormap[index]);
}
static void * pthread_nmsysdiag(void *arg)
{
    int i;
    int j;
    char messagetemp[64];
    j=0;
    nmsyslog("ok: Diagnostic Thread starts");
    while(1)
    {
        for(i=0;i<nmdiagBEACONUSED;i++)
        {
            if(nmdiag_beaconvalue[i]>0)
            {
                nmdiag_beaconvalue[i]--;
            }
            else
            {
        
		if(!nmsysdiagstatus(nmdiag_beaconerrormap[i]))
		{
                	sprintf(messagetemp, "error: ID %d, the thread beacon is timeout",i);
                	nmsyslog(messagetemp);
                	nmsysdiagset(nmdiag_beaconerrormap[i]);
		}
            }
            
        }
	if(nmdiag_errorlist)
	{

		
		if(nmdiag_errorlist&CANHWERROR)
		{
			nmsyslog("error: Diagnostic Service Detected CAN Hardware Error!");
		}
		if(nmdiag_errorlist&CANDBCERROR)
		{
			nmsyslog("error: Diagnostic Service Detected CAN DBC Loading Error!");
		}

		if(nmdiag_errorlist&CANWRBUFFERERROR)
		{
			nmsyslog("error: Diagnostic Service Detected CAN WR Buffer Error!");
		}
		if(nmdiag_errorlist&EVENTDBERROR)
		{

			nmsyslog("error: Diagnostic Service Detected Event Database Loading Error!");
		}
		if(nmdiag_errorlist&EVENTWRBUFFERERROR)
		{

			nmsyslog("error: Diagnostic Service Detected Event Database WR Buffer Error!");
		}
		if(nmdiag_errorlist&BEACON_CANRXERROR)
		{
		}
		if(nmdiag_errorlist&BEACON_SIGNALPARSERERROR)
		{
		}
		if(nmdiag_errorlist&BEACON_EVENTMONITORERROR)
		{
		}
		if(nmdiag_errorlist&BEACON_CANASCERROR)
		{
		}
	}
        sleep_ms(1000);
    }
}

void nmsysdiaginit(void)
{
    int i;
    pthread_t tidp;
    nmdiag_errorlist=0;
    for(i=0;i<nmdiagBEACONNUM;i++)
    {
        nmdiag_beaconvalue[i]=nmdiagBEACONMAX;
    }
    if ((pthread_create(&tidp, NULL, pthread_nmsysdiag,NULL)) == -1)
	{
        nmsyslog("error: Diagnostic Thread Initialization Error");
		return;
	}
}
