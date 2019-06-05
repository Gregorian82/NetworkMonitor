#ifndef nmsysdiag_H
#define nmsysdiag_H

#define EMPTYERROR                  0x0000
#define CANHWERROR                  0x0001
#define CANDBCERROR                 0x0002
#define CANWRBUFFERERROR            0x0004
#define EVENTDBERROR                0x0008
#define EVENTWRBUFFERERROR          0x0010
#define EVENTRDBUFFERERROR          0x0020

#define BEACON_CANRXERROR           0x0100
#define BEACON_SIGNALPARSERERROR    0x0200
#define BEACON_EVENTMONITORERROR    0x0400
#define BEACON_CANASCERROR          0x0800

#define NMBEACON_CANRX          0x0000
#define NMBEACON_SIGNALPARSER   0x0001
#define NMBEACON_EVENTMONITOR   0x0002
#define NMBEACON_CANASC         0x0003

void nmsysdiagset(unsigned int index);
void nmsysdiagbeacon(unsigned int index);  
int nmsysdiagcheck(void);
void nmsysdiaginit(void);
#endif
