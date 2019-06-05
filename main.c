#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>  
#include <signal.h>
#include <string.h>
#include <stdlib.h>   
#include <fcntl.h>    
#include <unistd.h>
#include "can/canasc.h"
#include "can/canrxthread.h"
#include "can/cansignalparse.h"
#include "event/eventmonitor.h"
#include "can/canrxbuffer.h"
#include "log/nmsyslog.h"
#include "log/nmsysdiag.h"

int main(int argc, char *argv[])
{   

    char key;
    char code[4];
    char messagetemp[64];
    int baudrateV;
    threadnum=4;
    exitflag=1;
    key=' ';
    
    if (argc > 0)
    {
        baudrateV=atoi(argv[1]);
        writebaudrate(baudrateV);
        sprintf(messagetemp, "ok: Setting Baudrate is %d",baudrateV);
        nmsyslog(messagetemp);
    }

    nmsysdiaginit();
    nmsyslog("ok: Diagnostic Service Initialization is finished");
    can_MsgBufferInit();
    nmsyslog("ok: CAN Message Buffer Initialization is finished");
    pthread_bufferInit();  
    nmsyslog("ok: CAN Message Receive Thread Initialization is finished");
    cansignalparseInit();  
    nmsyslog("ok: CAN Database Loading is finished");
    eventmonitor_init();   
    nmsyslog("ok: Event Monitor Initializtion is finished");
    while(threadnum);
    printf("functions start!\n");
    while(1)
    {
    }

    return 0;
}
