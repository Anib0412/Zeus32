/////////////////////////////////////////////////////////////////////////////
// DECOMP.CPP
//                                                                        
// LZW decompressor class for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"decomp.h"
#include"catdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDecomp::CDecomp()
   {
   // alloc stack
   stack = new BYTE[HSIZE];   
   codetab = new unsigned short[HSIZE];
   string_tab = new STRINGTABLE[TABSIZE];
   prefix = codetab;
   suffix = (BYTE *)string_tab;    
   maxcodemax =  1 << BITS;    /* largest possible code (+1) */      
   GlobalOffset = GlobalSize = 0;
   LineCount = 0;
   // init the bit masks
   lmask[0] = 0xff; lmask[1] = 0xfe; lmask[2] = 0xfc; lmask[3] = 0xf8;
   lmask[4] = 0xf0; lmask[5] = 0xe0; lmask[6] = 0xc0; lmask[7] = 0x80;
   lmask[8] = 0x00;
   rmask[0] = 0x00; rmask[1] = 0x01; rmask[2] = 0x03; rmask[3] = 0x07;
   rmask[4] = 0x0f; rmask[5] = 0x1f; rmask[6] = 0x3f; rmask[7] = 0x7f;
   rmask[8] = 0xff;
   }
   
CDecomp::~CDecomp()
   {                            
   delete string_tab;
   delete codetab;
   delete stack;     
   }   

BOOL CDecomp::Finish()
   {
   // finish up
   if (GlobalOutCount > 0)
      {
      if (fwrite(WriteBuffer, 1, GlobalOutCount, GlobalOutFile) !=
         GlobalOutCount)
         {
         // error writing file
         return FALSE;
         }
	  }  
   return TRUE;
   }	  		 

/***************************************************************************/
/* putc_ncr                                                                */
/*                                                                         */
/* put NCR coded bytes                                                     */
/***************************************************************************/

short CDecomp::putc_ncr(BYTE c)
   {
   short ret;
   static BYTE lastc;                 /* last character seen */

   ret = 0;

   switch(state)                       /* action depends on our state */
      {
      case NOHIST:                     /* no previous history */
	       if(c==DLE)                  /* if starting a series */
	          state = INREP;           /* then remember it next time */
	       else
	          ret = PutFunction(lastc=c);  /* else nothing unusual */
	       return(ret);
      case INREP:                      /* in a repeat */
	       if(c)                       /* if count is nonzero */
	          while((--c) && !(ret))   /* then repeatedly ... */
		         ret = PutFunction(lastc); /* ... output the byte */
	        else
	           ret = PutFunction(DLE);      /* else output DLE as data */
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

BOOL CDecomp::Start(FILE* infile, FILE* outfile, int hn)
   {
   BYTE *stackp;
   short finchar;
   short code, oldcode, incode;
   short ret;    

   state = NOHIST;
   ret = 0;   
   GlobalOutCount = 0;
   GlobalInFile = infile;
   GlobalOutFile = outfile;
   HeaderNeeded = hn;

   if((code=getc(GlobalInFile))!=BITS)
      {
      /* bad bit header */
      return(FALSE);
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

   finchar = oldcode = getcode();

   /* EOF already? Get out of here */
   if(oldcode == -1) return(FALSE);

   /* first code must be 8 bits=char */
   ret = putc_ncr((char)finchar);

   stackp = stack;

   while((code = getcode())> -1)
     {
     if (ret != 0) break;

     if(code==CLEAR)                  /* reset string table */
	{
	memset((void *)prefix,0, 256*sizeof(short));
	clear_flg = 1;
	free_ent = FIRST - 1;
	if((code=getcode())==-1)     /* O, untimely death! */
	   break;
	}
     incode = code;

     /* Special case for KwKwK string. */
     if(code >= free_ent)
	{
	*stackp++ = (BYTE)finchar;
	code = oldcode;
	}

     /* Generate output characters in reverse order */
     while(code >= 256)
	{
	*stackp++ = suffix[code];
	code = prefix[code];
	}                                         
	finchar = suffix[code];
    *stackp++ = (BYTE)finchar;

     /* And put them out in forward order */
     do
	ret = putc_ncr(*--stackp);
     while((stackp > stack) && (ret == 0));

     /* Generate the new entry. */
     if((code=free_ent) < maxcodemax)
	{
	prefix[code] = (unsigned short)oldcode;
	suffix[code] = (BYTE)finchar;
	free_ent = code+1;
	}
     /* Remember previous code. */
     oldcode = incode;
     }
   return(ret);
   }
                  
/***************************************************************************/
/* getcode                                                                 */
/*                                                                         */
/* get a code                                                              */
/***************************************************************************/

short CDecomp::getcode()
   {
   register code;
   register r_off, bits;
   BYTE *bp = (BYTE *)buf;

   if(clear_flg > 0 || GlobalOffset >= GlobalSize || free_ent > maxcode)
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

      for(GlobalSize=0; GlobalSize < n_bits; GlobalSize++)
	 {
	 if((code=getc(GlobalInFile))==EOF)
	    break;
	 else
	    buf[GlobalSize] = code;
	 }
      if(GlobalSize <= 0)
	 return -1;               /* end of file */

      GlobalOffset = 0;
      /* Round size down to integral number of codes */
      GlobalSize = (GlobalSize << 3)-(n_bits - 1);
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

short CDecomp::PutFunction(BYTE c)
   {
   WriteBuffer[GlobalOutCount++] = (BYTE)c;

   /* do line by line until we find header spec */
   if ((LineCount < 8) && (GlobalOutCount < WRITEBUFFERSIZE))
      {
      /* found a line feed, incr line count */
      if (c == 0x0a)
	     {
	     if (fwrite(WriteBuffer, 1, GlobalOutCount, GlobalOutFile) !=
	        GlobalOutCount)
	        {
	        // error writing file
	        exit(1);
	        }
	     LineCount++;
	     GlobalOutCount = 0;
	     if (LineCount == 8)
	        {
	        if (HeaderNeeded == FULL_HEADER)
	           {
	           if (fwrite(PSHeaderDICT, sizeof(PSHeaderDICT), 1, GlobalOutFile) != 1)
		          {
		          // error writing file
		          exit(1);
		          }
	           }
	        else if (HeaderNeeded == EPS_HEADER)
	           {
	           if (fwrite(EPSHeaderDICT, sizeof(EPSHeaderDICT), 1, GlobalOutFile) != 1)
		          {
		          // error writing file
		          exit(1);
		          }
	           }
	        }
	     }
      }
   else
      {
      if (GlobalOutCount == WRITEBUFFERSIZE)
	     {
	     if (fwrite(WriteBuffer, 1, GlobalOutCount, GlobalOutFile) !=
	        GlobalOutCount)
	        {
	        // error writing file
	        exit(1);
	        }
	     GlobalOutCount = 0;
	     }
      }
   return(0);
   }

