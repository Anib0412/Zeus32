// RIPMSG.H
// msg struct for inter-app status passing

#ifndef __RIPMSG_H__
#define __RIPMSG_H__

#define RIPMSG_MAX_MSG_LEN  128

typedef struct tagRIPMSGSTRUCT
   {
   short MsgType;    
   short DoTime;
   short PercentComplete;
   long TotalComplete;
   short ColorOn;
   short PageOn;
   short PageTotal;
   long LineOn;
   long LineTotal;   
   short CopyOn;
   short CopyTotal; 
   char Info[RIPMSG_MAX_MSG_LEN];
   } RIPMSGSTRUCT;

typedef RIPMSGSTRUCT * PRIPMSGSTRUCT;

// MsgType defs
#define RIPMSG_READING         1
#define RIPMSG_SCANLINE        2
#define RIPMSG_PRINTING        3
#define RIPMSG_SPINONLY        4
#define RIPMSG_INFO            6
#define RIPMSG_INFO2           7
#define RIPMSG_RENDERTITLE     8
#define RIPMSG_READING_TOTAL   9
#define RIPMSG_PRINTING_TP     10
#define RIPMSG_EJECT_PAGE      11

// Color defs
#define RIPMSG_COLOR_BLACK    0
#define RIPMSG_COLOR_WHITE    1
#define RIPMSG_COLOR_RED      2
#define RIPMSG_COLOR_GREEN    3
#define RIPMSG_COLOR_BLUE     4
#define RIPMSG_COLOR_CYAN     5
#define RIPMSG_COLOR_MAGENTA  6
#define RIPMSG_COLOR_YELLOW   7
#define RIPMSG_COLOR_IMAGE    8

#endif                        

