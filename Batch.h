/////////////////////////////////////////////////////////////////////////////
// batch.h
//                                                                        
// PostScript data for BatchHeader
// Copyright (C) 1996-98 Graphx, Inc.
//
// mods:
//
// 11-23-98  Version 2.0.5 - if background is black, use white text and lines

static char *BatchHeaderText[] =
   {
   "%!PS-Adobe-2.0 EPSF-2.0",
   "%%Title: RasterPlus Batch Header File",
   "%%Creator: RasterPlus",
   "%%BoundingBox: 0 0 792 528",
   "%%EndComments",
   "%%BeginSetup",
   "%%EndSetup",
   "gsave",
   "/BD {bind def} bind def",
   "/LD {load def} BD",
   "/N /newpath LD",
   "/M /moveto LD",
   "/L /lineto LD",
   "/S /show LD",
   "/SL /setlinewidth LD",
   "/FF {gsave eofill grestore} def",
   "/EO /eoclip LD",
   "/SF {gsave stroke grestore} def",
   "/GS /gsave LD",
   "/GR /grestore LD",
   "/cstring",
   "   { dup stringwidth pop 2 div currentpoint",
   "     3 -2 roll exch sub exch moveto } def",
   "/Helvetica-Bold findfont [50 0 0 50 0 0] makefont setfont",
   "N",                         // line 24
   "0 0 M",
   "792 0 L",
   "792 528 L",
   "0 528 L",
   ".999 .999 .999 setrgbcolor", // line 29
   "FF",                        // line 30
   "N",                         // line 31
   "0 50 M",
   "0 0 L",
   "50 0 L",
   //".999 .0 .0 setrgbcolor",
   ".999 .999 .999 setrgbcolor", // line 35
   "5 SL",
   "SF",
   "N",
   "742 0 M",
   "792 0 L",
   "792 50 L",
   //".999 .0 .0 setrgbcolor",
   " ",
   "5 SL",
   "SF",
   "N",
   "792 478 M",
   "792 528 L",
   "742 528 L",
   //".999 .0 .0 setrgbcolor",
   " ",
   "5 SL",
   "SF",
   "N",
   "0 478 M",
   "0 528 L",
   "50 528 L",
   //".999 .0 .0 setrgbcolor",
   " ",
   "5 SL",
   "SF",                        // line 58
   "GS",
   "396 400 M",
   //".0 .0 .999 setrgbcolor",
   " ",
   "(SLIDES ARE US) cstring",   // line 62
   "(SLIDES ARE US) S",         // line 63
   "GR",
   "GS",
   "396 350 M",
   //".0 .0 .999 setrgbcolor",
   " ",
   "(Batch #411) cstring",      // line 68
   "(Batch #411) S",            // line 69
   "GR",
   "GS",
   "396 300 M",
   //".0 .0 .999 setrgbcolor",
   " ",
   "(400 Slides in batch) cstring", // line 74
   "(400 Slides in batch) S",       // line 75
   "GR",
   "GS",
   "396 250 M",
   //".0 .0 .999 setrgbcolor",
   " ",
   "(November 11, 1994) cstring",   // line 80
   "(November 11, 1994) S",         // line 81
   "GR",
   "GS",
   "396 200 M",
   //".0 .0 .999 setrgbcolor",
   " ",
   "(Time) cstring",   // line 86
   "(Time) S",         // line 87
   "GR",
   "GR",
   "gsave showpage grestore"
   //"showpage"
   };

#define BATCHHEADER_NUMLINES     91
#define BATCHHEADER_LINE1        62
#define BATCHHEADER_LINE2        68
#define BATCHHEADER_LINE3        74
#define BATCHHEADER_LINE4        80
#define BATCHHEADER_LINE5        86
#define BATCHHEADER_STARTCORNER  31
#define BATCHHEADER_ENDCORNER    58
#define BATCHHEADER_LINE5        86
#define BATCHHEADER_STARTBACK    24
#define BATCHHEADER_ENDBACK      30
#define BATCHHEADER_BACKCOLOR    29
#define BATCHHEADER_TEXTCOLOR    35

static char *BatchHeaderString1 = " cstring";
static char *BatchHeaderString2 = " S";
static char *BatchHeaderBackWhite = ".999 .999 .999 setrgbcolor";
static char *BatchHeaderBackBlack = ".0 .0 .0 setrgbcolor";
static char *BatchHeaderTextWhite = ".999 .999 .999 setrgbcolor";
static char *BatchHeaderTextRed = ".999 .0 .0 setrgbcolor";

