#include "ascname.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
const char *filenamefix[]={"_raw.asc","_event.asc","_event.asc"};
const char *filenameprex[]={"report/raw/","report/event/summary/","report/event/raw/"};

char *getascname(BYTE nametype)
{
    time_t timenow;
    struct tm * timeinfo;
    char buffer0 [64];
    static char buffer1 [64];
    time(&timenow);
    timeinfo = localtime (&timenow);
    strftime (buffer0,sizeof(buffer0),"%Y-%m-%d_%H-%M-%S",timeinfo );
    sprintf(buffer1, "%s%s%s",filenameprex[nametype],buffer0,filenamefix[nametype]);
    return (char *)buffer1;
}
/*
void main(void)
{
    printf("%s\n",getascname(0));
}*/