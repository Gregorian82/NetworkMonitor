#ifndef nmtime_H
#define nmtime_H
#include "../can/cantype.h"
void sleep_ms(int timedelay_ms);
char *print_localtime(DWORD TimeS, DWORD TimeuS);
#endif