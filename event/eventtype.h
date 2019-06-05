#ifndef eventtype_H
#define eventtype_H
#include "../can/cantype.h"

#define eo_equal    0x01
#define eo_more     0x02
#define eo_less     0x03
#define eo_moreeq   0x04
#define eo_lesseq   0x05
#define eo_noteq    0x06
#define eo_null     0x00


#define el_and      0x01
#define el_or       0x02
#define el_null     0x00


#define es_TRUE     0x01
#define es_FAULSE   0x00

#define eventsts_inactive   0
#define eventsts_active     1


typedef struct EventSigType{
  char *SigName_t;
  DWORD SigId_t;
  BYTE SignalOperate;
  double SignalThreshold;
  BYTE SignalRelationShip;
  DWORD FrameId;
  char *FrameName;
  struct EventSigType *Next;
}EventSigType_t;

typedef struct EventDataType{
  WORD EventDataId;
  EventSigType_t *EventSigDataPtr;
  struct EventDataType *Next;
}EventDataType_t;

typedef struct EventDataStsType{
    DWORD EventTimeS_t;
    DWORD EventTimeUs_t;
    BYTE  EventStatus;
}EventDataStsType_t;




#endif
