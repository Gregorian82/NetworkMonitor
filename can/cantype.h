#ifndef cantype_H
#define cantype_H
#include <linux/types.h>
#include <linux/ioctl.h>
#include <pcan.h>

#ifndef NM_CAN_OK
#define NM_CAN_OK       0
#endif

#ifndef NM_CAN_ERROR
#define NM_CAN_ERROR    1
#endif


#if !defined(DWORD) || !defined(WORD) || !defined(BYTE)


#ifdef __KERNEL__
#define DWORD  u32
#define WORD   u16
#define BYTE   u8
#else
#define DWORD  __u32
#define WORD   __u16
#define BYTE   __u8
#endif
#endif

#ifndef WORD64
#define WORD64  unsigned long long
#endif

#define INTEL_FORMAT    0
#define MOTOROLA_FORMAT 1
/*Raw CAN Data Types */
typedef struct CanMsgType{
  DWORD MsgID_t;
  BYTE  MsgLen_t;
  BYTE  MsgType_t;		
  BYTE  MsgData_t[8];
  DWORD MsgStatus_t;
  DWORD MsgTimeS_t;           
  DWORD MsgTimeUs_t;          
}CanMsgType_t;
/* CAN Database Type */

typedef struct SigDataValTabType{
  char SigValTabName_t[64]; 
  struct SigDataValTabType *SigValTabNameNextPtr;
}SigDataValTabType_t;

typedef struct SigValueType {
  DWORD SigId_t;
  DWORD MsgId_t;
  BYTE  HwChId_t;
  char  *SigName_t;
  double SigValue;
  char *SigUnit_t;
  DWORD SigTimeS_t;           
  DWORD SigTimeUs_t;          
} SigValueType_t;

typedef struct SigDataType{
  DWORD SigId_t;
  char  *SigName_t;
  WORD  SigPos_t; 
  WORD  SigLen_t;
  BYTE  SigFormat_t;          
  BYTE  SigType_t;            
  double SigValRatio_t;
  double SigValOffset_t;
  char *SigUnit_t;
  SigDataValTabType_t SigValTab_t;
  struct SigDataType *SigDataNextPtr;
}SigDataType_t;

typedef struct MsgDataType{
  DWORD MsgId_t;
  char * MsgName_t;
  BYTE  MsgLen_t;
  BYTE  MsgType_t;
  WORD  MsgCyclicTime_t;
  SigDataType_t *SigDataPtr;
  struct MsgDataType *MsgDataNextPtr;
}MsgDataType_t;

typedef struct EcuDataType{
  DWORD EcuId_t;
  char *EcuName_t;
  struct EcuDataType *MsgDataPtr;
  struct EcuDataType *EcuNextPtr;
}EcuDataType_t;

typedef struct NetworkDataType{
  DWORD NetworkId_t;
  char *NetworkName_t;
  char *BraudrateStr_t;
  struct NetworkDataType *EcuPtr;
  struct NetworkDataType *NetworkNextPtr;
}NetworkDataType_t;
#endif
