# Microsoft Developer Studio Project File - Name="zeus32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=zeus32 - Win32 Debug98
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zeus.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zeus.mak" CFG="zeus32 - Win32 Debug98"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zeus32 - Win32 DebugRP30" (based on "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 ReleaseRP30" (based on "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 ReleasePHZPrintServer" (based on "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 DebugPHZPrintServer" (based on "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 Release98" (based on "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 Debug98" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Source/Zeus32", XXAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zeus32___Win32_DebugRP30"
# PROP BASE Intermediate_Dir "zeus32___Win32_DebugRP30"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\DebugRP30"
# PROP Intermediate_Dir ".\DebugRP30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "LOADDEVFROMRIP" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /Fr /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\source\gdiplus" /I "..\..\com\xml\xerces-c1_7_0-win32\include" /I "..\CmnLibs\Current LeadTools\include" /D "_DEBUG" /D "LOADDEVFROMRIP" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib ltfil_n.lib ltimg_n.lib ltkrn_n.lib spromeps.lib /nologo /stack:0x10240 /subsystem:windows /profile /map /debug /machine:IX86 /out:".\WinDebug\rp95.exe"
# ADD LINK32 ws2_32.lib lttwn_u.lib ltfil_u.lib ltdis_u.lib ltimgclr_u.lib ltimgcor_u.lib ltimgefx_u.lib ltkrn_u.lib psapi.lib tbarcode3.lib oldnames.lib spromeps.lib gdiplus.lib /nologo /subsystem:windows /debug /machine:IX86 /force /out:".\DebugRP30\rasterplus.exe" /libpath:"..\CmnLibs\CURRENT LEADTOOLS\lib" /libpath:"..\Expat-1.95.8\StaticLibs" /libpath:"..\CmnLibs\Common Libs"
# SUBTRACT LINK32 /profile /map

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zeus32___Win32_ReleaseRP30"
# PROP BASE Intermediate_Dir "zeus32___Win32_ReleaseRP30"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseRP30"
# PROP Intermediate_Dir "ReleaseRP30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Gr /MT /W3 /GX /D "USEDONGLE" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "\source\gdiplus" /I "..\..\com\xml\xerces-c1_7_0-win32\include" /I "..\CmnLibs\Current LeadTools\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ltdis_n.lib oldnames.lib ltfil_n.lib ltimg_n.lib ltkrn_n.lib spromeps.lib /nologo /stack:0x32000 /subsystem:windows /map /machine:IX86 /out:".\dongle\rp95.exe"
# SUBTRACT BASE LINK32 /profile /pdb:none
# ADD LINK32 ws2_32.lib lttwn_u.lib ltfil_u.lib ltdis_u.lib ltimgclr_u.lib ltimgcor_u.lib ltimgefx_u.lib ltkrn_u.lib psapi.lib tbarcode3.lib oldnames.lib spromeps.lib gdiplus.lib /nologo /subsystem:windows /map /machine:IX86 /force /out:".\ReleaseRP30\rasterplus.exe" /libpath:"..\CmnLibs\CURRENT LEADTOOLS\lib" /libpath:"..\Expat-1.95.8\StaticLibs" /libpath:"..\CmnLibs\Common Libs"
# SUBTRACT LINK32 /profile /pdb:none /debug

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zeus32___Win32_ReleasePHZPrintServer0"
# PROP BASE Intermediate_Dir "zeus32___Win32_ReleasePHZPrintServer0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleasePHZPrintServer"
# PROP Intermediate_Dir "ReleasePHZPrintServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GR /GX /O2 /I "..\..\com\xml\xerces-c1_7_0-win32\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /D "LITE_VERSION" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GR /GX /O2 /I "..\..\com\xml\xerces-c1_7_0-win32\include" /I "\source\gdiplus" /D "NDEBUG" /D "LITE_VERSION" /D "_PHZ_PRINT_SERVER" /D "LEADTOOLS_13" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 psapi.lib tbarcode3.lib ltwnd_n.lib ltdis_n.lib oldnames.lib ltfil_n.lib ltimg_n.lib ltkrn_n.lib spromeps.lib lttwn_n.lib gdiplus.lib /nologo /subsystem:windows /map /machine:IX86 /force /out:".\ReleaseRP30_Lite\rasterplus.exe"
# SUBTRACT BASE LINK32 /profile /pdb:none /debug
# ADD LINK32 ltwnd_n.lib ltwnd_n lttwn_u.lib ltfil_u.lib ltdis_u.lib ltimgclr_u.lib ltimgcor_u.lib ltimgefx_u.lib ltkrn_u.lib psapi.lib tbarcode3.lib oldnames.lib spromeps.lib gdiplus.lib /nologo /subsystem:windows /map /machine:IX86 /force /out:".\ReleasePHZPrintServer\PhotogizePrintServer.exe" /libpath:"..\Expat-1.95.8\StaticLibs"
# SUBTRACT LINK32 /profile /pdb:none /debug

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zeus32___Win32_DebugPHZPrintServer0"
# PROP BASE Intermediate_Dir "zeus32___Win32_DebugPHZPrintServer0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugPHZPrintServer"
# PROP Intermediate_Dir "DebugPHZPrintServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\..\com\xml\xerces-c1_7_0-win32\include" /D "_DEBUG" /D "LOADDEVFROMRIP" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /D "LITE_VERSION" /Fr /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\..\com\xml\xerces-c1_7_0-win32\include" /I "\source\gdiplus" /D "_DEBUG" /D "LOADDEVFROMRIP" /D "LITE_VERSION" /D "_PHZ_PRINT_SERVER" /D "LEADTOOLS_13" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 psapi.lib tbarcode3.lib ltwnd_n.lib ltdis_n.lib oldnames.lib ltfil_n.lib ltimg_n.lib ltkrn_n.lib spromeps.lib lttwn_n.lib gdiplus.lib /nologo /subsystem:windows /debug /machine:IX86 /force /out:".\DebugRP30_Lite\PhotogizePrintServer.exe"
# SUBTRACT BASE LINK32 /profile /map
# ADD LINK32 ltwnd_n.lib ltwnd_n lttwn_u.lib ltfil_u.lib ltdis_u.lib ltimgclr_u.lib ltimgcor_u.lib ltimgefx_u.lib ltkrn_u.lib psapi.lib tbarcode3.lib oldnames.lib spromeps.lib gdiplus.lib /nologo /subsystem:windows /debug /machine:IX86 /force /out:".\DebugPHZPrintServer\PhotogizePrintServer.exe" /libpath:"..\Expat-1.95.8\StaticLibs"
# SUBTRACT LINK32 /profile /map

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zeus32___Win32_Release98"
# PROP BASE Intermediate_Dir "zeus32___Win32_Release98"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release98"
# PROP Intermediate_Dir "Release98"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "\source\gdiplus" /I "..\..\com\xml\xerces-c1_7_0-win32\include" /I "..\CmnLibs\Current LeadTools\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "\source\gdiplus" /I "..\..\com\xml\xerces-c1_7_0-win32\include" /I "..\CmnLibs\Current LeadTools\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /D "_NOT_NT" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ws2_32.lib lttwn_u.lib ltfil_u.lib ltdis_u.lib ltimgclr_u.lib ltimgcor_u.lib ltimgefx_u.lib ltkrn_u.lib psapi.lib tbarcode3.lib oldnames.lib spromeps.lib gdiplus.lib /nologo /subsystem:windows /map /machine:IX86 /force /out:".\ReleaseRP30\rasterplus.exe" /libpath:"..\CmnLibs\CURRENT LEADTOOLS\lib" /libpath:"..\Expat-1.95.8\StaticLibs" /libpath:"..\CmnLibs\Common Libs"
# SUBTRACT BASE LINK32 /profile /pdb:none /debug
# ADD LINK32 ws2_32.lib lttwn_u.lib ltfil_u.lib ltdis_u.lib ltimgclr_u.lib ltimgcor_u.lib ltimgefx_u.lib ltkrn_u.lib psapi.lib tbarcode3.lib oldnames.lib spromeps.lib gdiplus.lib /nologo /subsystem:windows /map /machine:IX86 /force /out:".\Release98\rasterplus.exe" /libpath:"..\CmnLibs\CURRENT LEADTOOLS\lib" /libpath:"..\Expat-1.95.8\StaticLibs" /libpath:"..\CmnLibs\Common Libs"
# SUBTRACT LINK32 /profile /pdb:none /debug

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zeus32___Win32_Debug98"
# PROP BASE Intermediate_Dir "zeus32___Win32_Debug98"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug98"
# PROP Intermediate_Dir "Debug98"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\source\gdiplus" /I "..\..\com\xml\xerces-c1_7_0-win32\include" /I "..\CmnLibs\Current LeadTools\include" /D "_DEBUG" /D "LOADDEVFROMRIP" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /Fr /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\source\gdiplus" /I "..\..\com\xml\xerces-c1_7_0-win32\include" /I "..\CmnLibs\Current LeadTools\include" /D "_DEBUG" /D "LOADDEVFROMRIP" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SECNOMSG" /D "RP30" /D "_WIN32_DCOM" /D "_NOT_NT" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ws2_32.lib lttwn_u.lib ltfil_u.lib ltdis_u.lib ltimgclr_u.lib ltimgcor_u.lib ltimgefx_u.lib ltkrn_u.lib psapi.lib tbarcode3.lib oldnames.lib spromeps.lib gdiplus.lib /nologo /subsystem:windows /debug /machine:IX86 /force /out:".\DebugRP30\rasterplus.exe" /libpath:"..\CmnLibs\CURRENT LEADTOOLS\lib" /libpath:"..\Expat-1.95.8\StaticLibs" /libpath:"..\CmnLibs\Common Libs"
# SUBTRACT BASE LINK32 /profile /map
# ADD LINK32 ws2_32.lib lttwn_u.lib ltfil_u.lib ltdis_u.lib ltimgclr_u.lib ltimgcor_u.lib ltimgefx_u.lib ltkrn_u.lib psapi.lib tbarcode3.lib oldnames.lib spromeps.lib gdiplus.lib /nologo /subsystem:windows /debug /machine:IX86 /force /out:".\Debug98\rasterplus.exe" /libpath:"..\CmnLibs\CURRENT LEADTOOLS\lib" /libpath:"..\Expat-1.95.8\StaticLibs" /libpath:"..\CmnLibs\Common Libs"
# SUBTRACT LINK32 /profile /map

!ENDIF 

# Begin Target

# Name "zeus32 - Win32 DebugRP30"
# Name "zeus32 - Win32 ReleaseRP30"
# Name "zeus32 - Win32 ReleasePHZPrintServer"
# Name "zeus32 - Win32 DebugPHZPrintServer"
# Name "zeus32 - Win32 Release98"
# Name "zeus32 - Win32 Debug98"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\ICC\ColorManage\AdjustDialog.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# SUBTRACT BASE CPP /YX
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICC\ColorManage\AdjustGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\AdvancedDevice.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AntiAlias.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ArchiveTree.cpp
# End Source File
# Begin Source File

SOURCE=.\BHEADER.CPP
# End Source File
# Begin Source File

SOURCE=..\..\Com\PhotogizeLab\blowfish.cpp
# End Source File
# Begin Source File

SOURCE=.\cabinstall.cpp
# End Source File
# Begin Source File

SOURCE=.\CANVAS.CPP

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# ADD BASE CPP /O2
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# ADD BASE CPP /O2
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\ICC\ColorManage\CMMDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\CQUEUE.CPP
# End Source File
# Begin Source File

SOURCE="..\Font Manager\CResourceList.cpp"
# End Source File
# Begin Source File

SOURCE=.\CRIP.CPP
# End Source File
# Begin Source File

SOURCE=.\CTM2RECT.CPP

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# ADD BASE CPP /Od
# ADD CPP /Od

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# ADD BASE CPP /Od
# ADD CPP /Od

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# ADD BASE CPP /Od
# ADD CPP /Od

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CustomFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomPage.cpp
# End Source File
# Begin Source File

SOURCE=.\DECOMP.CPP

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\decomp2.c

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DemoDlg.cpp
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\devconnect.cpp
# End Source File
# Begin Source File

SOURCE=.\devdlg.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\device.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# ADD BASE CPP /Od
# ADD CPP /Od
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# ADD BASE CPP /Od
# ADD CPP /Od

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# ADD BASE CPP /Od
# SUBTRACT BASE CPP /YX
# ADD CPP /Od
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DeviceList.cpp
# End Source File
# Begin Source File

SOURCE=.\DISP.CPP
# End Source File
# Begin Source File

SOURCE=.\dlgwait.cpp
# End Source File
# Begin Source File

SOURCE=..\dongle\DongleClass.cpp
# End Source File
# Begin Source File

SOURCE=.\DownloadStatusDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Icc\Monaco\fileio.cpp
# End Source File
# Begin Source File

SOURCE=.\FontManagerPage.cpp
# End Source File
# Begin Source File

SOURCE=.\Frect.cpp
# End Source File
# Begin Source File

SOURCE=.\general.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\generalprefspage.cpp
# End Source File
# Begin Source File

SOURCE=.\GISServer.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\HoldTree.cpp
# End Source File
# Begin Source File

SOURCE=.\icon\icon.rc
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ImagePause.cpp
# End Source File
# Begin Source File

SOURCE=.\InstallPrinter.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# ADD CPP /I "C:\Program Files\Microsoft SDK\Include"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# ADD CPP /I "C:\Program Files\Microsoft SDK\Include"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# ADD BASE CPP /I "C:\Program Files\Microsoft SDK\Include"
# ADD CPP /I "C:\Program Files\Microsoft SDK\Include"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# ADD BASE CPP /I "C:\Program Files\Microsoft SDK\Include"
# ADD CPP /I "C:\Program Files\Microsoft SDK\Include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\JobProps.cpp
# End Source File
# Begin Source File

SOURCE=.\JobPropsSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\LicenseManager.cpp
# End Source File
# Begin Source File

SOURCE=..\ICC\ColorManage\LineGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\ListeningSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\loadtransform.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\LogWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=..\Icc\Monaco\MonacoAPI.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICC\MonacoMPL\MonacoMPL.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICC\MonacoMPL\Mpl_i.c

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MyInternetSession.cpp
# End Source File
# Begin Source File

SOURCE=.\newformv.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NewPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\NewQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\nformat.cpp
# End Source File
# Begin Source File

SOURCE=.\objects.cpp
# End Source File
# Begin Source File

SOURCE=.\OurPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\OurSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\OurWorkspace.cpp
# End Source File
# Begin Source File

SOURCE=.\PhotoPane.cpp
# End Source File
# Begin Source File

SOURCE=.\plist.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pptooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\PREVIEW.CPP

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PrinterNameChange.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PrintIndexDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintIndexPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintPaneState.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintTree.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessManager.cpp
# End Source File
# Begin Source File

SOURCE=.\profile.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyPane.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueChild.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueCount.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\QueuePane.cpp
# End Source File
# Begin Source File

SOURCE=.\help\rasterplus.hpj

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# Begin Custom Build - Making help file...
OutDir=.\DebugRP30
ProjDir=.
TargetName=rasterplus
InputPath=.\help\rasterplus.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# Begin Custom Build - Making help file...
OutDir=.\ReleaseRP30
ProjDir=.
TargetName=rasterplus
InputPath=.\help\rasterplus.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# Begin Custom Build - Making help file...
OutDir=.\ReleasePHZPrintServer
ProjDir=.
TargetName=PhotogizePrintServer
InputPath=.\help\rasterplus.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# Begin Custom Build - Making help file...
OutDir=.\DebugPHZPrintServer
ProjDir=.
TargetName=PhotogizePrintServer
InputPath=.\help\rasterplus.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# Begin Custom Build - Making help file...
OutDir=.\Release98
ProjDir=.
TargetName=rasterplus
InputPath=.\help\rasterplus.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# Begin Custom Build - Making help file...
OutDir=.\Debug98
ProjDir=.
TargetName=rasterplus
InputPath=.\help\rasterplus.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(ProjDir)\makehelp.bat"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RasterPlusView.cpp
# End Source File
# Begin Source File

SOURCE=.\report.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\REXDIRDI.CPP
# End Source File
# Begin Source File

SOURCE=.\ripitb.cpp
# End Source File
# Begin Source File

SOURCE=.\ripitj.cpp
# End Source File
# Begin Source File

SOURCE=.\rippane.cpp
# End Source File
# Begin Source File

SOURCE=.\ripprocess.cpp
# End Source File
# Begin Source File

SOURCE=.\ripthrd.cpp
# End Source File
# Begin Source File

SOURCE=.\roundit.cpp
# End Source File
# Begin Source File

SOURCE=.\help\rp95.hpj

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RPNode.cpp
# End Source File
# Begin Source File

SOURCE=.\RPResource.cpp
# End Source File
# Begin Source File

SOURCE=.\RPTreeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Ruler.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanDialog.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CmnSrc\scsi\Scsi.cpp
# End Source File
# Begin Source File

SOURCE=.\SCSIExplorer.cpp
# End Source File
# Begin Source File

SOURCE=.\server.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ServerCfg.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ShellAssociate.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SpecialTree.cpp
# End Source File
# Begin Source File

SOURCE=.\SPLITDLG.CPP

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\STDAFX.CPP

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SYSMEM.CPP
# End Source File
# Begin Source File

SOURCE=.\TimeCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\TRANSDLG.CPP
# End Source File
# Begin Source File

SOURCE=..\Icc\ColorManage\TransformColor.cpp
# End Source File
# Begin Source File

SOURCE=..\ICC\ColorManage\TransformGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\transformlist.cpp
# End Source File
# Begin Source File

SOURCE=.\TransformMgrPage.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TRANSFRM.CPP
# End Source File
# Begin Source File

SOURCE=.\USERFILE.CPP

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\visualxf.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# ADD BASE CPP /O2
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# ADD BASE CPP /O2
# ADD CPP /O2

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\webbrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\WebServicesASP.cpp
# ADD CPP /D "XML_STATIC"
# End Source File
# Begin Source File

SOURCE=..\ICC\Windows\WindowsCMM.cpp
# End Source File
# Begin Source File

SOURCE=.\WINSPLIT.CPP

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XformSave.cpp
# End Source File
# Begin Source File

SOURCE=..\..\com\PhotogizeLab\XHarvest.cpp

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ZEUS.CPP

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# ADD CPP /Gm
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# ADD BASE CPP /Gm
# SUBTRACT BASE CPP /YX
# ADD CPP /Gm
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# ADD BASE CPP /Gm
# SUBTRACT BASE CPP /YX
# ADD CPP /Gm
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\zeus.rc
# End Source File
# Begin Source File

SOURCE=.\zeusdoc.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\ICC\ColorManage\AdjustDialog.h
# End Source File
# Begin Source File

SOURCE=..\ICC\ColorManage\AdjustGraph.h
# End Source File
# Begin Source File

SOURCE=.\AdvancedDevice.h
# End Source File
# Begin Source File

SOURCE=.\AntiAlias.h
# End Source File
# Begin Source File

SOURCE=.\ArchiveTree.h
# End Source File
# Begin Source File

SOURCE=.\Batch.h
# End Source File
# Begin Source File

SOURCE=.\Bheader.h
# End Source File
# Begin Source File

SOURCE=.\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Com\PhotogizeLab\blowfish.h
# End Source File
# Begin Source File

SOURCE=.\cabinstall.h
# End Source File
# Begin Source File

SOURCE=.\Canvas.h
# End Source File
# Begin Source File

SOURCE=.\Catdef.h
# End Source File
# Begin Source File

SOURCE=.\ClientSocket.h
# End Source File
# Begin Source File

SOURCE=..\Icc\Monaco\cmiccprofile.h
# End Source File
# Begin Source File

SOURCE=..\ICC\ColorManage\CMMDialog.h
# End Source File
# Begin Source File

SOURCE=.\colorc.h
# End Source File
# Begin Source File

SOURCE=..\Icc\ColorProfile.h
# End Source File
# Begin Source File

SOURCE=.\ColorTransform.h
# End Source File
# Begin Source File

SOURCE=.\control.hm
# End Source File
# Begin Source File

SOURCE=.\Cqueue.h
# End Source File
# Begin Source File

SOURCE="..\Font Manager\CResourceList.h"
# End Source File
# Begin Source File

SOURCE=.\Crip.h
# End Source File
# Begin Source File

SOURCE=.\Ctm2rect.h
# End Source File
# Begin Source File

SOURCE=.\CustomFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\CustomPage.h
# End Source File
# Begin Source File

SOURCE=.\Decomp.h
# End Source File
# Begin Source File

SOURCE=.\DemoDlg.h
# End Source File
# Begin Source File

SOURCE=.\devconnect.h
# End Source File
# Begin Source File

SOURCE=.\devdlg.h
# End Source File
# Begin Source File

SOURCE=.\devface.h
# End Source File
# Begin Source File

SOURCE=.\device.h
# End Source File
# Begin Source File

SOURCE=.\DeviceList.h
# End Source File
# Begin Source File

SOURCE=.\Disp.h
# End Source File
# Begin Source File

SOURCE=.\dlgwait.h
# End Source File
# Begin Source File

SOURCE=.\DownloadStatusDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditJobDescription.h
# End Source File
# Begin Source File

SOURCE=..\Icc\Monaco\fileio.h
# End Source File
# Begin Source File

SOURCE=.\FontManagerPage.h
# End Source File
# Begin Source File

SOURCE=.\Frect.h
# End Source File
# Begin Source File

SOURCE=.\Ftypes.h
# End Source File
# Begin Source File

SOURCE=.\general.h
# End Source File
# Begin Source File

SOURCE=.\generalprefspage.h
# End Source File
# Begin Source File

SOURCE=..\gis\GImageClient.h
# End Source File
# Begin Source File

SOURCE=..\gis\giscmds.h
# End Source File
# Begin Source File

SOURCE=..\gis\gisretval.h
# End Source File
# Begin Source File

SOURCE=.\GISServer.h
# End Source File
# Begin Source File

SOURCE=..\gis\GisSocket.h
# End Source File
# Begin Source File

SOURCE=.\GlobalProperties.h
# End Source File
# Begin Source File

SOURCE=.\HoldTree.h
# End Source File
# Begin Source File

SOURCE=..\Icc\ICCProfile.h
# End Source File
# Begin Source File

SOURCE=.\ImagePause.h
# End Source File
# Begin Source File

SOURCE=.\Inidefs.h
# End Source File
# Begin Source File

SOURCE=.\InstallPrinter.h
# End Source File
# Begin Source File

SOURCE=.\ipcdata.h
# End Source File
# Begin Source File

SOURCE=.\ipcripdata.h
# End Source File
# Begin Source File

SOURCE=.\JobProps.h
# End Source File
# Begin Source File

SOURCE=.\JobPropsSheet.h
# End Source File
# Begin Source File

SOURCE=.\jobseparator.h
# End Source File
# Begin Source File

SOURCE=.\LicenseManager.h
# End Source File
# Begin Source File

SOURCE=..\ICC\ColorManage\LineGraph.h
# End Source File
# Begin Source File

SOURCE=.\ListeningSocket.h
# End Source File
# Begin Source File

SOURCE=.\loadtransform.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\LogWnd.h
# End Source File
# Begin Source File

SOURCE=.\Mainfrm.h
# End Source File
# Begin Source File

SOURCE=..\Icc\Monaco\MonacoAPI.h

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Icc\Monaco\MonacoCMMDLL.h

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICC\MonacoMPL\MonacoMPL.h

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ICC\MonacoMPL\Mpl.h

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MyInternetSession.h
# End Source File
# Begin Source File

SOURCE=.\newformv.h
# End Source File
# Begin Source File

SOURCE=.\NewPreview.h
# End Source File
# Begin Source File

SOURCE=.\NewQueue.h
# End Source File
# Begin Source File

SOURCE=.\nformat.h
# End Source File
# Begin Source File

SOURCE=.\objects.h
# End Source File
# Begin Source File

SOURCE=..\Icc\Monaco\OEMCmmDLL.h
# End Source File
# Begin Source File

SOURCE=.\OurPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\OurTreeBase.h
# End Source File
# Begin Source File

SOURCE=.\OurWorkspace.h
# End Source File
# Begin Source File

SOURCE=.\PhotoPane.h
# End Source File
# Begin Source File

SOURCE=.\plist.h
# End Source File
# Begin Source File

SOURCE=.\pptooltip.h
# End Source File
# Begin Source File

SOURCE=.\Prefsmsg.h
# End Source File
# Begin Source File

SOURCE=.\Prev.h
# End Source File
# Begin Source File

SOURCE=.\Preview.h
# End Source File
# Begin Source File

SOURCE=.\PreviewPrefsPage.h
# End Source File
# Begin Source File

SOURCE=.\PrinterNameChange.h
# End Source File
# Begin Source File

SOURCE=.\PrintIndexDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrintIndexPage.h
# End Source File
# Begin Source File

SOURCE=.\PrintPaneState.h
# End Source File
# Begin Source File

SOURCE=.\PrintTree.h
# End Source File
# Begin Source File

SOURCE=.\ProcessManager.h
# End Source File
# Begin Source File

SOURCE=.\profile.h
# End Source File
# Begin Source File

SOURCE=.\PropertyFrame.h
# End Source File
# Begin Source File

SOURCE=.\PropertyPane.h
# End Source File
# Begin Source File

SOURCE=.\ps_lookup.h
# End Source File
# Begin Source File

SOURCE=.\psapi.h
# End Source File
# Begin Source File

SOURCE=.\QueueChild.h
# End Source File
# Begin Source File

SOURCE=.\QueueCount.h
# End Source File
# Begin Source File

SOURCE=.\QueueMemory.h
# End Source File
# Begin Source File

SOURCE=.\QueuePane.h
# End Source File
# Begin Source File

SOURCE=.\RasterPlusView.h
# End Source File
# Begin Source File

SOURCE=.\report.h
# End Source File
# Begin Source File

SOURCE=.\Rexdirdi.h
# End Source File
# Begin Source File

SOURCE=.\Rip.h
# End Source File
# Begin Source File

SOURCE=.\ripitb.h
# End Source File
# Begin Source File

SOURCE=.\ripitj.h
# End Source File
# Begin Source File

SOURCE=.\Ripmsg.h
# End Source File
# Begin Source File

SOURCE=.\rippane.h
# End Source File
# Begin Source File

SOURCE=.\ripproc.h
# End Source File
# Begin Source File

SOURCE=.\ripprocess.h
# End Source File
# Begin Source File

SOURCE=.\RipStatus.h
# End Source File
# Begin Source File

SOURCE=.\RipStruct.h
# End Source File
# Begin Source File

SOURCE=.\ripthrd.h
# End Source File
# Begin Source File

SOURCE=.\roundit.h
# End Source File
# Begin Source File

SOURCE=.\RP95PORT.H
# End Source File
# Begin Source File

SOURCE=..\device\RP_Version.h
# End Source File
# Begin Source File

SOURCE=.\RPNode.h
# End Source File
# Begin Source File

SOURCE=.\RPResource.h
# End Source File
# Begin Source File

SOURCE=.\RPTreeBase.h
# End Source File
# Begin Source File

SOURCE=.\Ruler.h
# End Source File
# Begin Source File

SOURCE=.\ScanDialog.h

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SCSIExplorer.h
# End Source File
# Begin Source File

SOURCE=.\Servdefs.h
# End Source File
# Begin Source File

SOURCE=.\server.h
# End Source File
# Begin Source File

SOURCE=.\ServerCfg.h
# End Source File
# Begin Source File

SOURCE=.\ShellAssociate.h
# End Source File
# Begin Source File

SOURCE=.\SpecialTree.h
# End Source File
# Begin Source File

SOURCE=.\Splitdlg.h
# End Source File
# Begin Source File

SOURCE=.\Spromeps.h
# End Source File
# Begin Source File

SOURCE=.\Stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Sysmem.h
# End Source File
# Begin Source File

SOURCE=..\barcode\TECBarCode.h
# End Source File
# Begin Source File

SOURCE=.\TimeCheck.h
# End Source File
# Begin Source File

SOURCE=.\Transdlg.h
# End Source File
# Begin Source File

SOURCE=..\Icc\ColorManage\TransformColor.h
# End Source File
# Begin Source File

SOURCE=..\ICC\ColorManage\TransFormGraph.h
# End Source File
# Begin Source File

SOURCE=.\transformlist.h
# End Source File
# Begin Source File

SOURCE=.\TransformMgrPage.h
# End Source File
# Begin Source File

SOURCE=.\Transfrm.h
# End Source File
# Begin Source File

SOURCE=.\USERFILE.H
# End Source File
# Begin Source File

SOURCE=.\visualxf.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser.h
# End Source File
# Begin Source File

SOURCE=.\WebServicesASP.h
# End Source File
# Begin Source File

SOURCE=.\WebUpdate.h
# End Source File
# Begin Source File

SOURCE=..\ICC\Windows\WindowsCMM.h
# End Source File
# Begin Source File

SOURCE=.\Winsplit.h
# End Source File
# Begin Source File

SOURCE=.\XformSave.h
# End Source File
# Begin Source File

SOURCE=.\Xfrmdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\com\PhotogizeLab\XHarvest.h
# End Source File
# Begin Source File

SOURCE=.\Zeus.h
# End Source File
# Begin Source File

SOURCE=.\zeusdoc.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\bitmap5.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00005.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bmp00007.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\coldtool.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\Res\HiColorToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\HiColorToolbarDisabled.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\HiColorToolbarHot.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\icon\Res\Ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00010.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00011.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00012.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00013.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00014.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00015.ico
# End Source File
# Begin Source File

SOURCE=.\Res\ico00016.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icon6.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icon_fil.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icon_pre.ico
# End Source File
# Begin Source File

SOURCE=.\icon\Res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\Res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Idr_zeus.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Landscap.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Portrait.ico
# End Source File
# Begin Source File

SOURCE=.\Res\prevctrl.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\printtra.ico
# End Source File
# Begin Source File

SOURCE=.\rp6.bmp
# End Source File
# Begin Source File

SOURCE=.\rpicon.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_16.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_16_demo.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_new_24.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_new_24_fuji.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_new_2_24.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_new_2_24_fuji.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_new_2_8.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_new_2_8_fuji.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_new_8.bmp
# End Source File
# Begin Source File

SOURCE=.\rpsp_new_8_fuji.bmp
# End Source File
# Begin Source File

SOURCE=.\Rpsplash.bmp
# End Source File
# Begin Source File

SOURCE=.\Rpsplash_demo.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Status_o.ico
# End Source File
# Begin Source File

SOURCE=.\Res\status_p.ico
# End Source File
# Begin Source File

SOURCE=.\Res\tb256.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Toolbar1PHZ.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\toolbar_.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\toolbarPHZ.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\zoom_in.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\zoom_out.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Manual\ChangeLog.Txt
# End Source File
# Begin Source File

SOURCE=..\..\gis
# End Source File
# Begin Source File

SOURCE=.\Header.dat
# End Source File
# Begin Source File

SOURCE=..\ICC\MonacoMPL\Mpl.tlb

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Icc\Monaco\OEMCmmDLL.lib

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Icc\Monaco\MonacoCmm.lib

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\com\xml\xerces-c1_7_0-win32\lib\xerces-c_1.lib"
# End Source File
# Begin Source File

SOURCE="..\..\com\xml\xerces-c1_7_0-win32\lib\xerces-c_1D.lib"
# End Source File
# Begin Source File

SOURCE=..\PCGI\PCGINT.lib

!IF  "$(CFG)" == "zeus32 - Win32 DebugRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleaseRP30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 ReleasePHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 DebugPHZPrintServer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Release98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug98"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\Expat-1.95.8\StaticLibs\libexpatMT.lib"
# End Source File
# Begin Source File

SOURCE=..\PCGI_V5\PCGINT.lib
# End Source File
# End Target
# End Project
