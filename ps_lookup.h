/////////////////////////////////////////////////////////////////////////////
// PS_LOOKUP.H
//                                                                        
// look-up strings for PostScript file analysis
// Copyright (C) 1995-99 Graphx, Inc.
//

#ifndef __PS_LOOKUP_H__
#define __PS_LOOKUP_H__

static char* PS_Token[] = 
   {
// WinSlide-related stuff
   "%%BoundingBox:",                    // PSLOOKUP_BOUNDINGBOX
   "%%PageBoundingBox:",				// PSLOOKUP_PAGEBOUNDINGBOX
   "%ADO_ImageableArea:",				// PSLOOKUP_ADOIAREA
   "? ? ? ?",							// PSLOOKUP_QUESTION
   "(atend)",							// PSLOOKUP_ATEND
   "%%Title:",   						// PSLOOKUP_TITLE
   "%WinSplit Dict Needed:",   			// PSLOOKUP_WINSPLIT_DICT
   "EPSF",								// PSLOOKUP_EPSF
   "%%Creator: WinSlide Driver",		// PSLOOKUP_WINSLIDE_CREATOR
// Zeus Server related stuff
   "%%BeginFeature:",		            // PSLOOKUP_RP95_BEGIN_FEATURE
   "%%EndFeature", 						// PSLOOKUP_RP95_END_FEATURE
   "%%RasterPlus95 Server File Version 1.00",    // PSLOOKUP_RP95_SERVER_VER100      
   "%%Device:", 					    // PSLOOKUP_RP95_DEVICE
   "%!PS-Adobe-3.0",		  			// PSLOOKUP_RP95_ADOBE_30
   "%!PS-Adobe",		     			// PSLOOKUP_RP95_ADOBE_ANY
   "*JobPatchFile 98",                  // PSLOOKUP_RP95_SERVER_VERSION
   "*JobPatchFile 99",                  // PSLOOKUP_RP95_SERVER_DEVNAME
   "%%BeginSetup",						// PSLOOKUP_RP95_BEGIN_SETUP
   "%%EndSetup",						// PSLOOKUP_RP95_END_SETUP
   "*Resolution",                       // PSLOOKUP_RP95_SERVER_RESOLUTION
   "*PageSize",							// PSLOOKUP_RP95_SERVER_PAGESIZE
   "%!PS-Adobe-2.0",					// PSLOOKUP_RP95_ADOBE_20
   "end %DMM",                          // PSLOOKUP_RP95_MACFILM_HACK
// mac orientation
   "%%Orientation: Landscape",          // PSLOOKUP_RP95_MAC_ORIENT_LANDSCAPE
   "%%Orientation: Portrait",           // PSLOOKUP_RP95_MAC_ORIENT_PORTRAIT   
										// PSLOOKUP_RP95_95_ORIENT_PORTRAIT   
   "[ 1.000 0 0 1.000 0 0 ] false Pscript_Win_Driver_Incr",
										// PSLOOKUP_RP95_95_ORIENT_LANDSCAPE
   "[ 0 1.000 -1.000 0 0 0 ] false Pscript_Win_Driver_Incr",
   "%%Creator: Windows NT 4",			// PSLOOKUP_WINNT_CREATOR      
   "/mysetup [ 1.000 0 0 1.000",		// PSLOOKUP_COREL_ORIENT_PORTRAIT
   "/mysetup [ 0 1.000 -1.000 0",		// PSLOOKUP_COREL_ORIENT_LANDSCAPE
   "[statusdict begin{%%",				// PSLOOKUP_QUARK_PAGE_SIZE

// pdf stuff
   "CropBox",							// PSLOOKUP_PDF_CROP_BOX
   "MediaBox",							// PSLOOKUP_PDF_MEDIA_BOX
   "Title",								// PSLOOKUP_PDF_TITLE
   "*PrintToHold",						// PSLOOKUP_PRINTTOHOLD
   "*Print To Hold",		            // PSLOOKUP_PRINTTOHOLD_2
   "%%Creator: Pscript.dll Version 5",	// PSLOOKUP_WIN2000_CREATOR
   
// Addition PS Stuff
   "%%HiResBoundingBox",				// PSLOOKUP_HIRES_BOUNDINGBOX
   "Rotate"								// PSLOOKUP_PDF_ROTATE
   };
	
#define PSLOOKUP_BOUNDINGBOX    			 0
#define PSLOOKUP_PAGEBOUNDINGBOX			 1
#define PSLOOKUP_ADOIAREA					 2
#define PSLOOKUP_QUESTION					 3
#define PSLOOKUP_ATEND						 4
#define PSLOOKUP_TITLE						 5
#define PSLOOKUP_WINSPLIT_DICT				 6
#define PSLOOKUP_EPSF						 7
#define PSLOOKUP_WINSLIDE_CREATOR   		 8
#define PSLOOKUP_RP95_BEGIN_FEATURE			 9
#define PSLOOKUP_RP95_END_FEATURE			 10
#define PSLOOKUP_RP95_SERVER_VER100			 11
#define PSLOOKUP_RP95_DEVICE				 12
#define PSLOOKUP_RP95_ADOBE_30				 13
#define PSLOOKUP_RP95_ADOBE_ANY				 14
#define PSLOOKUP_RP95_SERVER_VERSION		 15
#define PSLOOKUP_RP95_SERVER_DEVNAME		 16
#define PSLOOKUP_RP95_BEGIN_SETUP			 17   
#define PSLOOKUP_RP95_END_SETUP				 18
#define PSLOOKUP_RP95_SERVER_RESOLUTION		 19
#define PSLOOKUP_RP95_SERVER_PAGESIZE		 20   
#define PSLOOKUP_RP95_ADOBE_20				 21
#define PSLOOKUP_RP95_MACFILM_HACK			 22
#define PSLOOKUP_RP95_MAC_ORIENT_LANDSCAPE   23
#define PSLOOKUP_RP95_MAC_ORIENT_PORTRAIT    24
#define PSLOOKUP_RP95_95_ORIENT_PORTRAIT     25
#define PSLOOKUP_RP95_95_ORIENT_LANDSCAPE    26
#define PSLOOKUP_WINNT_CREATOR               27 
#define PSLOOKUP_COREL_ORIENT_PORTRAIT       28
#define PSLOOKUP_COREL_ORIENT_LANDSCAPE      29
#define PSLOOKUP_QUARK_PAGE_SIZE             30
#define PSLOOKUP_PDF_CROP_BOX                31
#define PSLOOKUP_PDF_MEDIA_BOX               32
#define PSLOOKUP_PDF_TITLE					 33
#define PSLOOKUP_PRINTTOHOLD                 34
#define PSLOOKUP_PRINTTOHOLD_2				 35
#define PSLOOKUP_WIN2000_CREATOR             36
#define PSLOOKUP_HIRES_BOUNDINGBOX			 37
#define PSLOOKUP_PDF_ROTATE					 38
#endif
