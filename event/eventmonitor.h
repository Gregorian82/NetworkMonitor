#ifndef eventmonitor_H
#define eventmonitor_H
#include "../cfg/event_cfg.h"
#include "../can/cantype.h"

void eventcansigcheck(SigValueType_t *sigvalue_temp);
void eventmonitor_init();
#endif