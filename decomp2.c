/////////////////////////////////////////////////////////////////////////////
// Decomp2.c
//                                                                        
// LZW (CAT!) file decompression routines for RasterPlus95 project                                    
// Copyright (C) 1996 Graphx, Inc.
//

#define _MSC_VER 1200
#define _WIN32_WINNT 0x0501
#define NOCRYPT 1
#define __STRALIGN_H_ 1
#include <c:\Program Files\Microsoft SDK\Include\Windows.h>
#include "wtsapi32.h"
 
#include<string.h>
#include"catdef.h"

#define EOF -1

#define BYTE unsigned char

/* boolean values */
#define TRUE 1
#define FALSE 0

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

/* local prototypes */
static short putc_ncr(BYTE c, short (*PutFunc)());
static short getcode(short (*GetFunc)());
static void ResetGetCode();

/* global vars */
static short n_bits;          /* number of bits/code */
static short clear_flg;
static BYTE stack[HSIZE];     /* local push/pop stack */

static short maxcode;         /* maximum code, given n_bits */
#define MAXCODE(n)      ((1<<(n)) - 1) /* maximum code calculation */
static short maxcodemax =  1 << BITS;    /* largest possible code (+1) */

#define TABSIZE  4096
static struct entry                  /* string table entry format */
   {
   char used;                       /* true when this entry is in use */
   unsigned short next;             /* ptr to next in collision list */
   unsigned short predecessor;      /* code for preceeding string */
   BYTE follower;                   /* char following string */
   } string_tab[TABSIZE];             /* the code string table */

static unsigned short codetab[HSIZE];     /* string code table (crunch) */
static unsigned short *prefix = codetab;  /* prefix code table (uncrunch) */
static BYTE *suffix = (BYTE *)string_tab; /* suffix table (uncrunch) */

static short free_ent;                   /* first unused entry */
static char buf[BITS];                 /* input/output buffer */

static BYTE rmask[9] =        /* right side masks */
{   0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

static BYTE state;            /* current packing state */

/***************************************************************************/
/* putc_ncr                                                                */
/*                                                                         */
/* put NCR coded bytes                                                     */
/***************************************************************************/

static short putc_ncr(BYTE c, short (*PutFunc)())
   {
   short ret;
   static short lastc;                 /* last character seen */

   ret = 0;

   switch(state)                       /* action depends on our state */
      {
      case NOHIST:                     /* no previous history */
           if(c==DLE)                  /* if starting a series */
              state = INREP;           /* then remember it next time */
           else
              ret = PutFunc(lastc=c);  /* else nothing unusual */
           return(ret);
      case INREP:                      /* in a repeat */
           if(c)                       /* if count is nonzero */
              while((--c) && !(ret))   /* then repeatedly ... */
                 ret = PutFunc(lastc); /* ... output the byte */
           else
              ret = PutFunc(DLE);      /* else output DLE as data */
           state = NOHIST;             /* back to no history */
           return(ret);

      default:
           return(-1);
      }
   }

/***************************************************************************/
/* Decomp                                                                  */
/*                                                                         */
/* decompress a file                                                       */
/***************************************************************************/

short Decomp(short (*GetFunc)(), short (*PutFunc)())
   {
   BYTE *stackp;
   short finchar;
   short code, oldcode, incode;
   short ret;

   state = NOHIST;
   ret = 0;

   if((code=GetFunc())!=BITS)
      {
      /* bad bit header */
      return(-1);
      }

   n_bits = INIT_BITS;                /* set starting code size */
   clear_flg = 0;

   /* As above, initialize the first 256 entries in the table. */
   maxcode = MAXCODE(n_bits=INIT_BITS);
   /* reset decode string table */
   memset((void *)prefix, 0, 256*sizeof(short));
   for(code = 255; code >= 0; code--)
      suffix[code] = (BYTE)code;

   free_ent = FIRST;

   finchar = oldcode = getcode(GetFunc);

   /* EOF already? Get out of here */
   if(oldcode == -1) return(-1);

   /* first code must be 8 bits=char */
   ret = putc_ncr((char)finchar, PutFunc);

   stackp = stack;

   while((code = getcode(GetFunc))> -1)
     {
     if (ret != 0) break;

     if(code==CLEAR)                  /* reset string table */
        {
        memset((void *)prefix,0, 256*sizeof(short));
        clear_flg = 1;
        free_ent = FIRST - 1;
        if((code=getcode(GetFunc))==-1)     /* O, untimely death! */
           break;
        }
     incode = code;

     /* Special case for KwKwK string. */
     if(code >= free_ent)
        {
        *stackp++ = (unsigned char)finchar;
        code = oldcode;
        }

     /* Generate output characters in reverse order */
     while(code >= 256)
        {
        *stackp++ = suffix[code];
        code = prefix[code];
        }
     *stackp++ = (unsigned char)finchar = suffix[code];

     /* And put them out in forward order */
     do
        ret = putc_ncr(*--stackp, PutFunc);
     while((stackp > stack) && (ret == 0));

     /* Generate the new entry. */
     if((code=free_ent) < maxcodemax)
        {
        prefix[code] = (unsigned short)oldcode;
        suffix[code] = (unsigned char)finchar;
        free_ent = code+1;
        }
     /* Remember previous code. */
     oldcode = incode;
     }
   return(ret);
   }

/***************************************************************************/
/* DecompDiscreet                                                          */
/*                                                                         */
/* decompress a file in chunks smaller than 256 bytes                      */
/***************************************************************************/

short DecompDiscreet(short Action, short (*GetFunc)(), short (*PutFunc)())
   {
   static BYTE *stackp;
   static short finchar;
   static short code, oldcode, incode;
   static short ret;
   static short FirstCode;

   switch(Action)
      {
      case LZW_INIT:
           ResetGetCode();
           state = NOHIST;
           FirstCode = FALSE;
           ret = 0;
           if((code=GetFunc())!=BITS)
              {
              /* bad bit header */
              ret = -1;
              break;
              }
           n_bits = INIT_BITS;                /* set starting code size */
           clear_flg = 0;
           /* As above, initialize the first 256 entries in the table. */
           maxcode = MAXCODE(n_bits=INIT_BITS);
           /* reset decode string table */
           memset((void *)prefix, 0, 256*sizeof(short));
           for(code = 255; code >= 0; code--)
              suffix[code] = (BYTE)code;
           free_ent = FIRST;
           finchar = oldcode = getcode(GetFunc);
           /* EOF already? Get out of here */
           if(oldcode == -1)
             ret = -1;
           else
             ret = 0;
           stackp = stack;
           break;
      case LZW_CONTINUE:
           /* special for first code */
           if (!FirstCode)
              {
              ret = putc_ncr((char)finchar, PutFunc);
              FirstCode = TRUE;
              break;
              }
           /* if there is stuff on the stack, put it out in reverse order */
           if (stackp > stack)
              {
              ret = putc_ncr(*--stackp, PutFunc);
              if ((stackp == stack) && (ret == 0))
                 {
                 /* Generate the new entry. */
                 if((code=free_ent) < maxcodemax)
                    {
                    prefix[code] = (unsigned short)oldcode;
                    suffix[code] = (unsigned char)finchar;
                    free_ent = code+1;
                    }
                 /* Remember previous code. */
                 oldcode = incode;
                 }
              }

           /* break if still stuff on stack */
           if (stackp > stack) break;

           if((code = getcode(GetFunc))> -1)
              {
              if(code==CLEAR)                  /* reset string table */
                 {
                 memset((void *)prefix,0, 256*sizeof(short));
                 clear_flg = 1;
                 free_ent = FIRST - 1;
                 if((code=getcode(GetFunc))==-1)     /* O, untimely death! */
                    {
                    ret = -1;
                    break;
                    }
                 }
              incode = code;
              /* Special case for KwKwK string. */
              if(code >= free_ent)
                 {
                 *stackp++ = (unsigned char)finchar;
                 code = oldcode;
                 }
              /* Generate output characters in reverse order */
              while(code >= 256)
                 {
                 *stackp++ = suffix[code];
                 code = prefix[code];
                 }
              *stackp++ = (unsigned char)finchar = suffix[code];
              }
           else
              ret = -1;
           break;
      }
   return(ret);
   }

/***************************************************************************/
/* getcode                                                                 */
/*                                                                         */
/* get a code                                                              */
/***************************************************************************/

static short GlobalOffset;
static short GlobalFileSize;

static void ResetGetCode()
   {
   GlobalOffset = 0;
   GlobalFileSize = 0;
   }

static short getcode(short (*GetFunc)())
   {
   register code;
   register r_off, bits;
   BYTE *bp = (BYTE *)buf;

   if(clear_flg > 0 || GlobalOffset >= GlobalFileSize || free_ent > maxcode)
      {
      /*
       * If the next entry will be too big for the current code
       * size, then we must increase the size.  This implies reading
       * a new buffer full, too.
       */
      if(free_ent > maxcode)
         {
         n_bits++;
         if(n_bits == BITS)
            maxcode = maxcodemax;    /* won't get any bigger now */
         else
            maxcode = MAXCODE(n_bits);
         }
      if(clear_flg > 0)
         {
         maxcode = MAXCODE(n_bits = INIT_BITS);
         clear_flg = 0;
         }

      for(GlobalFileSize=0; GlobalFileSize < n_bits; GlobalFileSize++)
         {
         if((code=GetFunc())==EOF)
            break;
         else
            buf[GlobalFileSize] = code;
         }
      if(GlobalFileSize <= 0)
         return -1;               /* end of file */

      GlobalOffset = 0;
      /* Round size down to integral number of codes */
      GlobalFileSize = (GlobalFileSize << 3)-(n_bits - 1);
      }
   r_off = GlobalOffset;
   bits = n_bits;

   /*
    * Get to the first byte.
    */
   bp +=(r_off >> 3);
   r_off &= 7;

   /* Get first part (low order bits) */
   code = (*bp++ >> r_off);
   bits -= 8 - r_off;
   r_off = 8 - r_off;                 /* now, offset into code word */

   /* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
   if(bits >= 8)
      {
      code |= *bp++ << r_off;
      r_off += 8;
      bits -= 8;
      }
   /* high order bits. */
   code |= (*bp & rmask[bits]) << r_off;
   GlobalOffset += n_bits;

   return (short)(code & MAXCODE(BITS));
   }
