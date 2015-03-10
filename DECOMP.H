#ifndef _DECOMP_H_
#define _DECOMP_H_

#include"header.dat"
#include <afxext.h>

/* lzw defs */
#define INREP     1                   /* sending a repeated value */
#define NOHIST    0                   /* don't consider previous input*/
#define SENTCHAR  1                   /* lastchar set, no lookahead yet */
#define SENDNEWC  2                   /* run over, send new char next */
#define SENDCNT   3                   /* newchar set, send count next */
#define DLE       0x90                /* repeat sequence marker */
#define FIRST     257                 /* first free entry */
#define CLEAR     256                 /* table clear output code */
#define BITS      12                  /* maximum bits per code */
#define HSIZE     5003                /* 80% occupancy */
#define INIT_BITS 9                   /* initial number of bits/code */

#define MAXCODE(n)      ((1<<(n)) - 1) /* maximum code calculation */

#define TABSIZE  4096
typedef struct tagSTRINGTABLE
   {
   char used;                       /* true when this entry is in use */
   unsigned short next;             /* ptr to next in collision list */
   unsigned short predecessor;      /* code for preceeding string */
   BYTE follower;                   /* char following string */   
   } STRINGTABLE;

#define WRITEBUFFERSIZE 256

class CDecomp
   {         
public:   
   CDecomp();
   ~CDecomp();           
   BOOL Start(FILE*, FILE*, int);
   short getcode();    
   short putc_ncr(BYTE);
   short PutFunction(BYTE);
   BOOL Finish();
protected:
   short n_bits;          
   short clear_flg;
   BYTE *stack;
   short maxcode;
   short maxcodemax;      
   unsigned short *codetab;     /* string code table (crunch) */
   unsigned short *prefix;
   BYTE *suffix; 
   STRINGTABLE* string_tab;  
   BYTE state;            /* current packing state */
   short free_ent;                   /* first unused entry */
   char buf[BITS];                 /* input/output buffer */
   short GlobalOffset;
   short GlobalSize;               
   FILE* GlobalInFile;
   FILE* GlobalOutFile;
   BYTE lmask[9];        /* left side masks */      
   BYTE rmask[9];        /* right side masks */   
   BYTE WriteBuffer[WRITEBUFFERSIZE];
   size_t GlobalOutCount;
   int HeaderNeeded;
   long LineCount;
   };   
#endif
