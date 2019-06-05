#include "nmtime.h"
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void sleep_ms(int timedelay_ms)
{
	struct timeval timeout;
	timeout.tv_sec=0;
	timeout.tv_usec=1000*timedelay_ms;
	select(1,NULL,NULL,NULL,&timeout);
	return;
}

char *print_localtime(DWORD TimeS, DWORD TimeuS)
{
	time_t mytime;
	struct tm TimeNowtemp;
	char strT[32];
	static char buffer1[64];
	mytime=(time_t)TimeS;
	strftime(strT, sizeof(strT), "%Y/%m/%d %H:%M:%S.", localtime(&mytime));
  	sprintf(buffer1, "%s%dms",strT,TimeuS/1000);
	return buffer1;
}
