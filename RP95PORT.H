/////////////////////////////////////////////////////////////////////////////
// RP95PORT.H
//                                                                        
// port definitions for RasterPlus95
// Copyright (C) 1996 Graphx, Inc.
//

#ifdef UNICODE
#define RP95_PORT_SECTION      L"Ports"

#ifdef RP30
#define RP95_PORT_KEY          L"Software\\Graphx\\RasterPlus\\Ports"
#define RP95_PORT_NAME         L"RasterPlus Port"
#define RP_ADDED_PORT          L"Software\\Graphx\\RasterPlus\\NewPorts"
#else
#define RP95_PORT_KEY          L"Software\\Graphx\\RasterPlus95\\Ports"
#define RP95_PORT_NAME         L"RasterPlus95 Port"
#endif
#else
#define RP95_PORT_SECTION      "Ports"

#ifdef RP30
#define RP95_PORT_KEY          "Software\\Graphx\\RasterPlus\\Ports"
#define RP95_PORT_NAME         "RasterPlus Port"
#define RP_ADDED_PORT          "Software\\Graphx\\RasterPlus\\NewPorts"
#else
#define RP95_PORT_KEY          "Software\\Graphx\\RasterPlus95\\Ports"
#define RP95_PORT_NAME         "RasterPlus95 Port"
#endif

#endif

