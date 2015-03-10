/////////////////////////////////////////////////////////////////////////////
// GISRPCCMDs.h
//                                                                        
// Photogize.PrintServer RPC layer
// Copyright (C) 2005 Graphx, Inc.
//
// Created: April 6, 2005 by dje
//
// Mods:
//
//

#ifndef __GISRPCCMDS_H__
#define __GISRPCCMDS_H__

#define GISRPCCMD_GETSVRVERSION				0x01
#define GISRPCCMD_GETNUMDEVICES				0x02
#define GISRPCCMD_GETDEVINFO				0x03
#define GISRPCCMD_GETCANVASINFO				0x04
#define GISRPCCMD_CREATECUSTOMCANVAS		0x05
#define GISRPCCMD_OPENJOB					0x06
#define GISRPCCMD_GETLASTERRORSTR   		0x07
#define GISRPCCMD_ADDJOBSEPARATOR   		0x08
#define GISRPCCMD_ADDPHOTO         			0x09
#define GISRPCCMD_PRINTJOB					0x0A
#define GISRPCCMD_GETJOBSTATUS				0x0B
#define GISRPCCMD_CLOSEJOB					0x0C
#define GISRPCCMD_CLOSERPC					0x0D
#define GISRPCCMD_SERVERSYSMSG				0x0E
#define GISRPCCMD_GETDEVOPTIONNAME			0x0F
#define GISRPCCMD_GETDEVOPTIONSETTINGNAME	0x10
#define GISRPCCMD_GETDEVOPTION				0x11
#define GISRPCCMD_SETDEVOPTION				0x12
#define GISRPCCMD_GETPHOTOSTATUS			0x13
#define GISRPCCMD_PUTFILE         			0x14
#define GISRPCCMD_GETFILE         			0x15
#define GISRPCCMD_ADDPHOTO2         		0x16

#define GISRPCCMD_SND						0x01
#define GISRPCCMD_RCV						0x02

#endif