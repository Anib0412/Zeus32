/////////////////////////////////////////////////////////////////////////////
// QueueCount.h
//                                                                        
// global data holder for queue counting for RasterPlus
// Copyright (C) 2000 Graphx, Inc.
//

#ifndef __QUEUECOUNT_H__
#define __QUEUECOUNT_H__

#define MAX_OPEN_QUEUES   16

typedef struct tagQUEUECOUNT
   {
   int iQueueCount;   
   } QUEUECOUNT, *PQUEUECOUNT;

class CQueueCount : public CObject
   {
public:
   CQueueCount();
   ~CQueueCount();      
   void SetData();
   void UnSetData();
   void IncrQueueCount(int iUpTick = 1);
   void DecrQueueCount(int iDownTick = 1);   
   int GetQueueCount();   
   void SetQueueCount(int iCount);   

protected:   
   HANDLE hFileMap;
   PQUEUECOUNT pData;
   };
#endif
