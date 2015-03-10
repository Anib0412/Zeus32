/////////////////////////////////////////////////////////////////////////////
// catdef.h
//                                                                        
// LZW (CAT!) file decompression defines for RasterPlus95 project                                    
// Copyright (C) 1996 Graphx, Inc.
//

#define LZW_INIT      0
#define LZW_CONTINUE  1

#define FULL_HEADER   1
#define EPS_HEADER    8

short Decomp(short (*GetFunc)(), short (*PutFunc)());
short DecompDiscreet(short Action, short (*GetFunc)(), short (*PutFunc)());




