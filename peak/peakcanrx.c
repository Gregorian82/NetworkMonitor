#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   // exit
#include <signal.h>
#include <string.h>
#include <stdlib.h>   // strtoul
#include <fcntl.h>    // O_RDWR
#include <unistd.h>

#include <libpcan.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/time.h>
#include "peakcanrx.h"
#include "../cfg/cfg.h"
#include "../log/nmsyslog.h"

static HANDLE h;
static struct timeval t1;
static CanMsgType_t rxtemp;
static __u16 baudrategroup[]={CAN_BAUD_100K,CAN_BAUD_125K,CAN_BAUD_250K,CAN_BAUD_500K,CAN_BAUD_1M};
static char* baudratestr[]={"CAN_BAUD_100K","CAN_BAUD_125K","CAN_BAUD_250K","CAN_BAUD_500K","CAN_BAUD_1M"};

BYTE PeakCanInit(int baudratetemp)
{
    __u32 dwPort = 0;
    __u16 wIrq = 0;
    __u16 wBTR0BTR1 = baudrategroup[baudratetemp];
    printf ("Peak Can Baudrate Setting as %s\n",baudratestr[baudratetemp]);
    h = LINUX_CAN_Open(DEFAULT_NODE0, O_RDWR);
	if(h==NULL)
	{
		//Log File
		nmsyslog("error: No CAN Device Found!");
		return NM_CAN_ERROR;
	}
    errno = CAN_Init(h, wBTR0BTR1, CAN_INIT_TYPE_ST);
	#ifdef nm_debug
	printf ("Peak Can Inital Status:%d",errno);
	printf("Peak Start\n");
	#endif
	return NM_CAN_OK;
}

CanMsgType_t *PeakCanRead(void)
{
    DWORD status;
    TPCANRdMsg Rx_msg;
    //status=LINUX_CAN_Read(h, &Rx_msg);
	status=LINUX_CAN_Read_Timeout(h, &Rx_msg,10000000);
	#ifdef nm_debug
	printf("peak can read status: %d\n",status);
	#endif
	gettimeofday(&t1,NULL);
    rxtemp.MsgID_t=Rx_msg.Msg.ID;
	rxtemp.MsgType_t=(BYTE)Rx_msg.Msg.MSGTYPE;
	rxtemp.MsgData_t[0]=Rx_msg.Msg.DATA[0];
	rxtemp.MsgData_t[1]=Rx_msg.Msg.DATA[1];
	rxtemp.MsgData_t[2]=Rx_msg.Msg.DATA[2];
	rxtemp.MsgData_t[3]=Rx_msg.Msg.DATA[3];
	rxtemp.MsgData_t[4]=Rx_msg.Msg.DATA[4];
	rxtemp.MsgData_t[5]=Rx_msg.Msg.DATA[5];
	rxtemp.MsgData_t[6]=Rx_msg.Msg.DATA[6];
	rxtemp.MsgData_t[7]=Rx_msg.Msg.DATA[7];
	rxtemp.MsgLen_t=Rx_msg.Msg.LEN;
    rxtemp.MsgStatus_t=status;
	rxtemp.MsgTimeUs_t=t1.tv_usec;
	rxtemp.MsgTimeS_t=t1.tv_sec;
    return &rxtemp;
}

