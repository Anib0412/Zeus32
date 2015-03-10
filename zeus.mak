# Microsoft Developer Studio Generated NMAKE File, Based on zeus.dsp
!IF "$(CFG)" == ""
CFG=zeus32 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to zeus32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "zeus32 - Win32 Debug" && "$(CFG)" !=\
 "zeus32 - Win32 Fastcall Release" && "$(CFG)" !=\
 "zeus32 - Win32 Fastcall Release Kanji" && "$(CFG)" !=\
 "zeus32 - Win32 Fastcall Release Dongle" && "$(CFG)" != "zeus32 - Win32 Demo"\
 && "$(CFG)" != "zeus32 - Win32 Debug Kanji"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zeus.mak" CFG="zeus32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zeus32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 Fastcall Release" (based on "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 Fastcall Release Kanji" (based on\
 "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 Fastcall Release Dongle" (based on\
 "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 Demo" (based on "Win32 (x86) Application")
!MESSAGE "zeus32 - Win32 Debug Kanji" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

OUTDIR=.\WinDebug
INTDIR=.\WinDebug
# Begin Custom Macros
OutDir=.\.\WinDebug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\rp95.exe" "$(OUTDIR)\zeus.bsc"

!ELSE 

ALL : "$(OUTDIR)\rp95.exe" "$(OUTDIR)\zeus.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AdvancedDevice.obj"
	-@erase "$(INTDIR)\AdvancedDevice.sbr"
	-@erase "$(INTDIR)\BHEADER.OBJ"
	-@erase "$(INTDIR)\BHEADER.SBR"
	-@erase "$(INTDIR)\cabinstall.obj"
	-@erase "$(INTDIR)\cabinstall.sbr"
	-@erase "$(INTDIR)\CANVAS.OBJ"
	-@erase "$(INTDIR)\CANVAS.SBR"
	-@erase "$(INTDIR)\CQUEUE.OBJ"
	-@erase "$(INTDIR)\CQUEUE.SBR"
	-@erase "$(INTDIR)\CRIP.OBJ"
	-@erase "$(INTDIR)\CRIP.SBR"
	-@erase "$(INTDIR)\CTM2RECT.OBJ"
	-@erase "$(INTDIR)\CTM2RECT.SBR"
	-@erase "$(INTDIR)\DECOMP.OBJ"
	-@erase "$(INTDIR)\DECOMP.SBR"
	-@erase "$(INTDIR)\devconnect.obj"
	-@erase "$(INTDIR)\devconnect.sbr"
	-@erase "$(INTDIR)\devdlg.obj"
	-@erase "$(INTDIR)\devdlg.sbr"
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\device.sbr"
	-@erase "$(INTDIR)\DeviceList.obj"
	-@erase "$(INTDIR)\DeviceList.sbr"
	-@erase "$(INTDIR)\DevListPage.obj"
	-@erase "$(INTDIR)\DevListPage.sbr"
	-@erase "$(INTDIR)\DISP.OBJ"
	-@erase "$(INTDIR)\DISP.SBR"
	-@erase "$(INTDIR)\dlgwait.obj"
	-@erase "$(INTDIR)\dlgwait.sbr"
	-@erase "$(INTDIR)\DongleClass.obj"
	-@erase "$(INTDIR)\DongleClass.sbr"
	-@erase "$(INTDIR)\fileinfo.obj"
	-@erase "$(INTDIR)\fileinfo.sbr"
	-@erase "$(INTDIR)\Frect.obj"
	-@erase "$(INTDIR)\Frect.sbr"
	-@erase "$(INTDIR)\general.obj"
	-@erase "$(INTDIR)\general.sbr"
	-@erase "$(INTDIR)\GlobalProperties.obj"
	-@erase "$(INTDIR)\GlobalProperties.sbr"
	-@erase "$(INTDIR)\ImagePause.obj"
	-@erase "$(INTDIR)\ImagePause.sbr"
	-@erase "$(INTDIR)\loadtransform.obj"
	-@erase "$(INTDIR)\loadtransform.sbr"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\mainfrm.sbr"
	-@erase "$(INTDIR)\newformv.obj"
	-@erase "$(INTDIR)\newformv.sbr"
	-@erase "$(INTDIR)\nformat.obj"
	-@erase "$(INTDIR)\nformat.sbr"
	-@erase "$(INTDIR)\OurPropertySheet.obj"
	-@erase "$(INTDIR)\OurPropertySheet.sbr"
	-@erase "$(INTDIR)\plist.obj"
	-@erase "$(INTDIR)\plist.sbr"
	-@erase "$(INTDIR)\PREVIEW.OBJ"
	-@erase "$(INTDIR)\PREVIEW.SBR"
	-@erase "$(INTDIR)\PreviewPrefsPage.obj"
	-@erase "$(INTDIR)\PreviewPrefsPage.sbr"
	-@erase "$(INTDIR)\PrinterNameChange.obj"
	-@erase "$(INTDIR)\PrinterNameChange.sbr"
	-@erase "$(INTDIR)\ProcessManager.obj"
	-@erase "$(INTDIR)\ProcessManager.sbr"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\profile.sbr"
	-@erase "$(INTDIR)\qprefs.obj"
	-@erase "$(INTDIR)\qprefs.sbr"
	-@erase "$(INTDIR)\QueueChild.obj"
	-@erase "$(INTDIR)\QueueChild.sbr"
	-@erase "$(INTDIR)\report.obj"
	-@erase "$(INTDIR)\report.sbr"
	-@erase "$(INTDIR)\REXDIRDI.OBJ"
	-@erase "$(INTDIR)\REXDIRDI.SBR"
	-@erase "$(INTDIR)\ripitb.obj"
	-@erase "$(INTDIR)\ripitb.sbr"
	-@erase "$(INTDIR)\ripitj.obj"
	-@erase "$(INTDIR)\ripitj.sbr"
	-@erase "$(INTDIR)\ripprocess.obj"
	-@erase "$(INTDIR)\ripprocess.sbr"
	-@erase "$(INTDIR)\ripthrd.obj"
	-@erase "$(INTDIR)\ripthrd.sbr"
	-@erase "$(INTDIR)\roundit.obj"
	-@erase "$(INTDIR)\roundit.sbr"
	-@erase "$(INTDIR)\Ruler.obj"
	-@erase "$(INTDIR)\Ruler.sbr"
	-@erase "$(INTDIR)\Scsi.obj"
	-@erase "$(INTDIR)\Scsi.sbr"
	-@erase "$(INTDIR)\SCSIExplorer.obj"
	-@erase "$(INTDIR)\SCSIExplorer.sbr"
	-@erase "$(INTDIR)\server.obj"
	-@erase "$(INTDIR)\server.sbr"
	-@erase "$(INTDIR)\ServerCfg.obj"
	-@erase "$(INTDIR)\ServerCfg.sbr"
	-@erase "$(INTDIR)\ShellAssociate.obj"
	-@erase "$(INTDIR)\ShellAssociate.sbr"
	-@erase "$(INTDIR)\SPLITDLG.OBJ"
	-@erase "$(INTDIR)\SPLITDLG.SBR"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\STDAFX.SBR"
	-@erase "$(INTDIR)\SYSMEM.OBJ"
	-@erase "$(INTDIR)\SYSMEM.SBR"
	-@erase "$(INTDIR)\TRANSDLG.OBJ"
	-@erase "$(INTDIR)\TRANSDLG.SBR"
	-@erase "$(INTDIR)\transformlist.obj"
	-@erase "$(INTDIR)\transformlist.sbr"
	-@erase "$(INTDIR)\TransformMgrPage.obj"
	-@erase "$(INTDIR)\TransformMgrPage.sbr"
	-@erase "$(INTDIR)\TRANSFRM.OBJ"
	-@erase "$(INTDIR)\TRANSFRM.SBR"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\visualxf.obj"
	-@erase "$(INTDIR)\visualxf.sbr"
	-@erase "$(INTDIR)\WINSPLIT.OBJ"
	-@erase "$(INTDIR)\WINSPLIT.SBR"
	-@erase "$(INTDIR)\XformSave.obj"
	-@erase "$(INTDIR)\XformSave.sbr"
	-@erase "$(INTDIR)\ZEUS.OBJ"
	-@erase "$(INTDIR)\zeus.res"
	-@erase "$(INTDIR)\ZEUS.SBR"
	-@erase "$(INTDIR)\zeusdoc.obj"
	-@erase "$(INTDIR)\zeusdoc.sbr"
	-@erase "$(OUTDIR)\rp95.exe"
	-@erase "$(OUTDIR)\rp95.map"
	-@erase "$(OUTDIR)\zeus.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "NEWDEVICE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\WinDebug/

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\zeus.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zeus.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AdvancedDevice.sbr" \
	"$(INTDIR)\BHEADER.SBR" \
	"$(INTDIR)\cabinstall.sbr" \
	"$(INTDIR)\CANVAS.SBR" \
	"$(INTDIR)\CQUEUE.SBR" \
	"$(INTDIR)\CRIP.SBR" \
	"$(INTDIR)\CTM2RECT.SBR" \
	"$(INTDIR)\DECOMP.SBR" \
	"$(INTDIR)\devconnect.sbr" \
	"$(INTDIR)\devdlg.sbr" \
	"$(INTDIR)\device.sbr" \
	"$(INTDIR)\DeviceList.sbr" \
	"$(INTDIR)\DevListPage.sbr" \
	"$(INTDIR)\DISP.SBR" \
	"$(INTDIR)\dlgwait.sbr" \
	"$(INTDIR)\DongleClass.sbr" \
	"$(INTDIR)\fileinfo.sbr" \
	"$(INTDIR)\Frect.sbr" \
	"$(INTDIR)\general.sbr" \
	"$(INTDIR)\GlobalProperties.sbr" \
	"$(INTDIR)\ImagePause.sbr" \
	"$(INTDIR)\loadtransform.sbr" \
	"$(INTDIR)\mainfrm.sbr" \
	"$(INTDIR)\newformv.sbr" \
	"$(INTDIR)\nformat.sbr" \
	"$(INTDIR)\OurPropertySheet.sbr" \
	"$(INTDIR)\plist.sbr" \
	"$(INTDIR)\PREVIEW.SBR" \
	"$(INTDIR)\PreviewPrefsPage.sbr" \
	"$(INTDIR)\PrinterNameChange.sbr" \
	"$(INTDIR)\ProcessManager.sbr" \
	"$(INTDIR)\profile.sbr" \
	"$(INTDIR)\qprefs.sbr" \
	"$(INTDIR)\QueueChild.sbr" \
	"$(INTDIR)\report.sbr" \
	"$(INTDIR)\REXDIRDI.SBR" \
	"$(INTDIR)\ripitb.sbr" \
	"$(INTDIR)\ripitj.sbr" \
	"$(INTDIR)\ripprocess.sbr" \
	"$(INTDIR)\ripthrd.sbr" \
	"$(INTDIR)\roundit.sbr" \
	"$(INTDIR)\Ruler.sbr" \
	"$(INTDIR)\Scsi.sbr" \
	"$(INTDIR)\SCSIExplorer.sbr" \
	"$(INTDIR)\server.sbr" \
	"$(INTDIR)\ServerCfg.sbr" \
	"$(INTDIR)\ShellAssociate.sbr" \
	"$(INTDIR)\SPLITDLG.SBR" \
	"$(INTDIR)\STDAFX.SBR" \
	"$(INTDIR)\SYSMEM.SBR" \
	"$(INTDIR)\TRANSDLG.SBR" \
	"$(INTDIR)\transformlist.sbr" \
	"$(INTDIR)\TransformMgrPage.sbr" \
	"$(INTDIR)\TRANSFRM.SBR" \
	"$(INTDIR)\visualxf.sbr" \
	"$(INTDIR)\WINSPLIT.SBR" \
	"$(INTDIR)\XformSave.sbr" \
	"$(INTDIR)\ZEUS.SBR" \
	"$(INTDIR)\zeusdoc.sbr"

"$(OUTDIR)\zeus.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=oldnames.lib /nologo /stack:0x10240 /subsystem:windows /profile\
 /map:"$(INTDIR)\rp95.map" /debug /machine:IX86 /out:"$(OUTDIR)\rp95.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AdvancedDevice.obj" \
	"$(INTDIR)\BHEADER.OBJ" \
	"$(INTDIR)\cabinstall.obj" \
	"$(INTDIR)\CANVAS.OBJ" \
	"$(INTDIR)\CQUEUE.OBJ" \
	"$(INTDIR)\CRIP.OBJ" \
	"$(INTDIR)\CTM2RECT.OBJ" \
	"$(INTDIR)\DECOMP.OBJ" \
	"$(INTDIR)\devconnect.obj" \
	"$(INTDIR)\devdlg.obj" \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\DeviceList.obj" \
	"$(INTDIR)\DevListPage.obj" \
	"$(INTDIR)\DISP.OBJ" \
	"$(INTDIR)\dlgwait.obj" \
	"$(INTDIR)\DongleClass.obj" \
	"$(INTDIR)\fileinfo.obj" \
	"$(INTDIR)\Frect.obj" \
	"$(INTDIR)\general.obj" \
	"$(INTDIR)\GlobalProperties.obj" \
	"$(INTDIR)\ImagePause.obj" \
	"$(INTDIR)\loadtransform.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\newformv.obj" \
	"$(INTDIR)\nformat.obj" \
	"$(INTDIR)\OurPropertySheet.obj" \
	"$(INTDIR)\plist.obj" \
	"$(INTDIR)\PREVIEW.OBJ" \
	"$(INTDIR)\PreviewPrefsPage.obj" \
	"$(INTDIR)\PrinterNameChange.obj" \
	"$(INTDIR)\ProcessManager.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\qprefs.obj" \
	"$(INTDIR)\QueueChild.obj" \
	"$(INTDIR)\report.obj" \
	"$(INTDIR)\REXDIRDI.OBJ" \
	"$(INTDIR)\ripitb.obj" \
	"$(INTDIR)\ripitj.obj" \
	"$(INTDIR)\ripprocess.obj" \
	"$(INTDIR)\ripthrd.obj" \
	"$(INTDIR)\roundit.obj" \
	"$(INTDIR)\Ruler.obj" \
	"$(INTDIR)\Scsi.obj" \
	"$(INTDIR)\SCSIExplorer.obj" \
	"$(INTDIR)\server.obj" \
	"$(INTDIR)\ServerCfg.obj" \
	"$(INTDIR)\ShellAssociate.obj" \
	"$(INTDIR)\SPLITDLG.OBJ" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\SYSMEM.OBJ" \
	"$(INTDIR)\TRANSDLG.OBJ" \
	"$(INTDIR)\transformlist.obj" \
	"$(INTDIR)\TransformMgrPage.obj" \
	"$(INTDIR)\TRANSFRM.OBJ" \
	"$(INTDIR)\visualxf.obj" \
	"$(INTDIR)\WINSPLIT.OBJ" \
	"$(INTDIR)\XformSave.obj" \
	"$(INTDIR)\ZEUS.OBJ" \
	"$(INTDIR)\zeus.res" \
	"$(INTDIR)\zeusdoc.obj" \
	".\lead80\lib\ltfil_n.lib" \
	".\lead80\lib\ltimg_n.lib" \
	".\lead80\lib\ltkrn_n.lib" \
	".\Spromeps.lib"

"$(OUTDIR)\rp95.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

OUTDIR=.\fastcall
INTDIR=.\fastcall
# Begin Custom Macros
OutDir=.\.\fastcall
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\rp95.exe"

!ELSE 

ALL : "$(OUTDIR)\rp95.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AdvancedDevice.obj"
	-@erase "$(INTDIR)\BHEADER.OBJ"
	-@erase "$(INTDIR)\cabinstall.obj"
	-@erase "$(INTDIR)\CANVAS.OBJ"
	-@erase "$(INTDIR)\CQUEUE.OBJ"
	-@erase "$(INTDIR)\CRIP.OBJ"
	-@erase "$(INTDIR)\CTM2RECT.OBJ"
	-@erase "$(INTDIR)\DECOMP.OBJ"
	-@erase "$(INTDIR)\devconnect.obj"
	-@erase "$(INTDIR)\devdlg.obj"
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\DeviceList.obj"
	-@erase "$(INTDIR)\DevListPage.obj"
	-@erase "$(INTDIR)\DISP.OBJ"
	-@erase "$(INTDIR)\dlgwait.obj"
	-@erase "$(INTDIR)\DongleClass.obj"
	-@erase "$(INTDIR)\fileinfo.obj"
	-@erase "$(INTDIR)\Frect.obj"
	-@erase "$(INTDIR)\general.obj"
	-@erase "$(INTDIR)\GlobalProperties.obj"
	-@erase "$(INTDIR)\ImagePause.obj"
	-@erase "$(INTDIR)\loadtransform.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\newformv.obj"
	-@erase "$(INTDIR)\nformat.obj"
	-@erase "$(INTDIR)\OurPropertySheet.obj"
	-@erase "$(INTDIR)\plist.obj"
	-@erase "$(INTDIR)\PREVIEW.OBJ"
	-@erase "$(INTDIR)\PreviewPrefsPage.obj"
	-@erase "$(INTDIR)\PrinterNameChange.obj"
	-@erase "$(INTDIR)\ProcessManager.obj"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\qprefs.obj"
	-@erase "$(INTDIR)\QueueChild.obj"
	-@erase "$(INTDIR)\report.obj"
	-@erase "$(INTDIR)\REXDIRDI.OBJ"
	-@erase "$(INTDIR)\ripitb.obj"
	-@erase "$(INTDIR)\ripitj.obj"
	-@erase "$(INTDIR)\ripprocess.obj"
	-@erase "$(INTDIR)\ripthrd.obj"
	-@erase "$(INTDIR)\roundit.obj"
	-@erase "$(INTDIR)\Ruler.obj"
	-@erase "$(INTDIR)\Scsi.obj"
	-@erase "$(INTDIR)\SCSIExplorer.obj"
	-@erase "$(INTDIR)\server.obj"
	-@erase "$(INTDIR)\ServerCfg.obj"
	-@erase "$(INTDIR)\ShellAssociate.obj"
	-@erase "$(INTDIR)\SPLITDLG.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\SYSMEM.OBJ"
	-@erase "$(INTDIR)\TRANSDLG.OBJ"
	-@erase "$(INTDIR)\transformlist.obj"
	-@erase "$(INTDIR)\TransformMgrPage.obj"
	-@erase "$(INTDIR)\TRANSFRM.OBJ"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\visualxf.obj"
	-@erase "$(INTDIR)\WINSPLIT.OBJ"
	-@erase "$(INTDIR)\XformSave.obj"
	-@erase "$(INTDIR)\ZEUS.OBJ"
	-@erase "$(INTDIR)\zeus.res"
	-@erase "$(INTDIR)\zeusdoc.obj"
	-@erase "$(OUTDIR)\rp95.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Gr /MT /W3 /GX /Zi /D "NDEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "NEWDEVICE" /D "_MBCS" /Fp"$(INTDIR)\zeus.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\fastcall/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\zeus.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zeus.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=oldnames.lib /nologo /stack:0x32000 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\rp95.pdb" /machine:IX86\
 /out:"$(OUTDIR)\rp95.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AdvancedDevice.obj" \
	"$(INTDIR)\BHEADER.OBJ" \
	"$(INTDIR)\cabinstall.obj" \
	"$(INTDIR)\CANVAS.OBJ" \
	"$(INTDIR)\CQUEUE.OBJ" \
	"$(INTDIR)\CRIP.OBJ" \
	"$(INTDIR)\CTM2RECT.OBJ" \
	"$(INTDIR)\DECOMP.OBJ" \
	"$(INTDIR)\devconnect.obj" \
	"$(INTDIR)\devdlg.obj" \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\DeviceList.obj" \
	"$(INTDIR)\DevListPage.obj" \
	"$(INTDIR)\DISP.OBJ" \
	"$(INTDIR)\dlgwait.obj" \
	"$(INTDIR)\DongleClass.obj" \
	"$(INTDIR)\fileinfo.obj" \
	"$(INTDIR)\Frect.obj" \
	"$(INTDIR)\general.obj" \
	"$(INTDIR)\GlobalProperties.obj" \
	"$(INTDIR)\ImagePause.obj" \
	"$(INTDIR)\loadtransform.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\newformv.obj" \
	"$(INTDIR)\nformat.obj" \
	"$(INTDIR)\OurPropertySheet.obj" \
	"$(INTDIR)\plist.obj" \
	"$(INTDIR)\PREVIEW.OBJ" \
	"$(INTDIR)\PreviewPrefsPage.obj" \
	"$(INTDIR)\PrinterNameChange.obj" \
	"$(INTDIR)\ProcessManager.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\qprefs.obj" \
	"$(INTDIR)\QueueChild.obj" \
	"$(INTDIR)\report.obj" \
	"$(INTDIR)\REXDIRDI.OBJ" \
	"$(INTDIR)\ripitb.obj" \
	"$(INTDIR)\ripitj.obj" \
	"$(INTDIR)\ripprocess.obj" \
	"$(INTDIR)\ripthrd.obj" \
	"$(INTDIR)\roundit.obj" \
	"$(INTDIR)\Ruler.obj" \
	"$(INTDIR)\Scsi.obj" \
	"$(INTDIR)\SCSIExplorer.obj" \
	"$(INTDIR)\server.obj" \
	"$(INTDIR)\ServerCfg.obj" \
	"$(INTDIR)\ShellAssociate.obj" \
	"$(INTDIR)\SPLITDLG.OBJ" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\SYSMEM.OBJ" \
	"$(INTDIR)\TRANSDLG.OBJ" \
	"$(INTDIR)\transformlist.obj" \
	"$(INTDIR)\TransformMgrPage.obj" \
	"$(INTDIR)\TRANSFRM.OBJ" \
	"$(INTDIR)\visualxf.obj" \
	"$(INTDIR)\WINSPLIT.OBJ" \
	"$(INTDIR)\XformSave.obj" \
	"$(INTDIR)\ZEUS.OBJ" \
	"$(INTDIR)\zeus.res" \
	"$(INTDIR)\zeusdoc.obj" \
	".\lead80\lib\ltfil_n.lib" \
	".\lead80\lib\ltimg_n.lib" \
	".\lead80\lib\ltkrn_n.lib" \
	".\Spromeps.lib"

"$(OUTDIR)\rp95.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

OUTDIR=.\rp_kanji
INTDIR=.\rp_kanji
# Begin Custom Macros
OutDir=.\.\rp_kanji
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\rp95.exe"

!ELSE 

ALL : "$(OUTDIR)\rp95.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AdvancedDevice.obj"
	-@erase "$(INTDIR)\BHEADER.OBJ"
	-@erase "$(INTDIR)\cabinstall.obj"
	-@erase "$(INTDIR)\CANVAS.OBJ"
	-@erase "$(INTDIR)\CQUEUE.OBJ"
	-@erase "$(INTDIR)\CRIP.OBJ"
	-@erase "$(INTDIR)\CTM2RECT.OBJ"
	-@erase "$(INTDIR)\DECOMP.OBJ"
	-@erase "$(INTDIR)\devconnect.obj"
	-@erase "$(INTDIR)\devdlg.obj"
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\DeviceList.obj"
	-@erase "$(INTDIR)\DevListPage.obj"
	-@erase "$(INTDIR)\DISP.OBJ"
	-@erase "$(INTDIR)\dlgwait.obj"
	-@erase "$(INTDIR)\DongleClass.obj"
	-@erase "$(INTDIR)\fileinfo.obj"
	-@erase "$(INTDIR)\Frect.obj"
	-@erase "$(INTDIR)\general.obj"
	-@erase "$(INTDIR)\GlobalProperties.obj"
	-@erase "$(INTDIR)\icon.res"
	-@erase "$(INTDIR)\ImagePause.obj"
	-@erase "$(INTDIR)\loadtransform.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\newformv.obj"
	-@erase "$(INTDIR)\nformat.obj"
	-@erase "$(INTDIR)\OurPropertySheet.obj"
	-@erase "$(INTDIR)\plist.obj"
	-@erase "$(INTDIR)\PREVIEW.OBJ"
	-@erase "$(INTDIR)\PreviewPrefsPage.obj"
	-@erase "$(INTDIR)\PrinterNameChange.obj"
	-@erase "$(INTDIR)\ProcessManager.obj"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\qprefs.obj"
	-@erase "$(INTDIR)\QueueChild.obj"
	-@erase "$(INTDIR)\report.obj"
	-@erase "$(INTDIR)\REXDIRDI.OBJ"
	-@erase "$(INTDIR)\ripitb.obj"
	-@erase "$(INTDIR)\ripitj.obj"
	-@erase "$(INTDIR)\ripprocess.obj"
	-@erase "$(INTDIR)\ripthrd.obj"
	-@erase "$(INTDIR)\roundit.obj"
	-@erase "$(INTDIR)\Ruler.obj"
	-@erase "$(INTDIR)\Scsi.obj"
	-@erase "$(INTDIR)\SCSIExplorer.obj"
	-@erase "$(INTDIR)\server.obj"
	-@erase "$(INTDIR)\ServerCfg.obj"
	-@erase "$(INTDIR)\ShellAssociate.obj"
	-@erase "$(INTDIR)\SPLITDLG.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\SYSMEM.OBJ"
	-@erase "$(INTDIR)\TRANSDLG.OBJ"
	-@erase "$(INTDIR)\transformlist.obj"
	-@erase "$(INTDIR)\TransformMgrPage.obj"
	-@erase "$(INTDIR)\TRANSFRM.OBJ"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\visualxf.obj"
	-@erase "$(INTDIR)\WINSPLIT.OBJ"
	-@erase "$(INTDIR)\XformSave.obj"
	-@erase "$(INTDIR)\ZEUS.OBJ"
	-@erase "$(INTDIR)\zeusdoc.obj"
	-@erase "$(OUTDIR)\rp95.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Gr /MT /W3 /GX /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "NEWDEVICE" /D "_MBCS" /D "USEKANJI" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\rp_kanji/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\icon.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zeus.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=oldnames.lib /nologo /stack:0x32000 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\rp95.pdb" /machine:IX86\
 /out:"$(OUTDIR)\rp95.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AdvancedDevice.obj" \
	"$(INTDIR)\BHEADER.OBJ" \
	"$(INTDIR)\cabinstall.obj" \
	"$(INTDIR)\CANVAS.OBJ" \
	"$(INTDIR)\CQUEUE.OBJ" \
	"$(INTDIR)\CRIP.OBJ" \
	"$(INTDIR)\CTM2RECT.OBJ" \
	"$(INTDIR)\DECOMP.OBJ" \
	"$(INTDIR)\devconnect.obj" \
	"$(INTDIR)\devdlg.obj" \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\DeviceList.obj" \
	"$(INTDIR)\DevListPage.obj" \
	"$(INTDIR)\DISP.OBJ" \
	"$(INTDIR)\dlgwait.obj" \
	"$(INTDIR)\DongleClass.obj" \
	"$(INTDIR)\fileinfo.obj" \
	"$(INTDIR)\Frect.obj" \
	"$(INTDIR)\general.obj" \
	"$(INTDIR)\GlobalProperties.obj" \
	"$(INTDIR)\icon.res" \
	"$(INTDIR)\ImagePause.obj" \
	"$(INTDIR)\loadtransform.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\newformv.obj" \
	"$(INTDIR)\nformat.obj" \
	"$(INTDIR)\OurPropertySheet.obj" \
	"$(INTDIR)\plist.obj" \
	"$(INTDIR)\PREVIEW.OBJ" \
	"$(INTDIR)\PreviewPrefsPage.obj" \
	"$(INTDIR)\PrinterNameChange.obj" \
	"$(INTDIR)\ProcessManager.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\qprefs.obj" \
	"$(INTDIR)\QueueChild.obj" \
	"$(INTDIR)\report.obj" \
	"$(INTDIR)\REXDIRDI.OBJ" \
	"$(INTDIR)\ripitb.obj" \
	"$(INTDIR)\ripitj.obj" \
	"$(INTDIR)\ripprocess.obj" \
	"$(INTDIR)\ripthrd.obj" \
	"$(INTDIR)\roundit.obj" \
	"$(INTDIR)\Ruler.obj" \
	"$(INTDIR)\Scsi.obj" \
	"$(INTDIR)\SCSIExplorer.obj" \
	"$(INTDIR)\server.obj" \
	"$(INTDIR)\ServerCfg.obj" \
	"$(INTDIR)\ShellAssociate.obj" \
	"$(INTDIR)\SPLITDLG.OBJ" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\SYSMEM.OBJ" \
	"$(INTDIR)\TRANSDLG.OBJ" \
	"$(INTDIR)\transformlist.obj" \
	"$(INTDIR)\TransformMgrPage.obj" \
	"$(INTDIR)\TRANSFRM.OBJ" \
	"$(INTDIR)\visualxf.obj" \
	"$(INTDIR)\WINSPLIT.OBJ" \
	"$(INTDIR)\XformSave.obj" \
	"$(INTDIR)\ZEUS.OBJ" \
	"$(INTDIR)\zeusdoc.obj" \
	".\lead80\lib\ltfil_n.lib" \
	".\lead80\lib\ltimg_n.lib" \
	".\lead80\lib\ltkrn_n.lib" \
	".\Spromeps.lib"

"$(OUTDIR)\rp95.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

OUTDIR=.\dongle
INTDIR=.\dongle
# Begin Custom Macros
OutDir=.\.\dongle
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\rp95.exe"

!ELSE 

ALL : "$(OUTDIR)\rp95.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AdvancedDevice.obj"
	-@erase "$(INTDIR)\BHEADER.OBJ"
	-@erase "$(INTDIR)\cabinstall.obj"
	-@erase "$(INTDIR)\CANVAS.OBJ"
	-@erase "$(INTDIR)\CQUEUE.OBJ"
	-@erase "$(INTDIR)\CRIP.OBJ"
	-@erase "$(INTDIR)\CTM2RECT.OBJ"
	-@erase "$(INTDIR)\DECOMP.OBJ"
	-@erase "$(INTDIR)\devconnect.obj"
	-@erase "$(INTDIR)\devdlg.obj"
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\DeviceList.obj"
	-@erase "$(INTDIR)\DevListPage.obj"
	-@erase "$(INTDIR)\DISP.OBJ"
	-@erase "$(INTDIR)\dlgwait.obj"
	-@erase "$(INTDIR)\DongleClass.obj"
	-@erase "$(INTDIR)\fileinfo.obj"
	-@erase "$(INTDIR)\Frect.obj"
	-@erase "$(INTDIR)\general.obj"
	-@erase "$(INTDIR)\GlobalProperties.obj"
	-@erase "$(INTDIR)\ImagePause.obj"
	-@erase "$(INTDIR)\loadtransform.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\newformv.obj"
	-@erase "$(INTDIR)\nformat.obj"
	-@erase "$(INTDIR)\OurPropertySheet.obj"
	-@erase "$(INTDIR)\plist.obj"
	-@erase "$(INTDIR)\PREVIEW.OBJ"
	-@erase "$(INTDIR)\PreviewPrefsPage.obj"
	-@erase "$(INTDIR)\PrinterNameChange.obj"
	-@erase "$(INTDIR)\ProcessManager.obj"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\qprefs.obj"
	-@erase "$(INTDIR)\QueueChild.obj"
	-@erase "$(INTDIR)\report.obj"
	-@erase "$(INTDIR)\REXDIRDI.OBJ"
	-@erase "$(INTDIR)\ripitb.obj"
	-@erase "$(INTDIR)\ripitj.obj"
	-@erase "$(INTDIR)\ripprocess.obj"
	-@erase "$(INTDIR)\ripthrd.obj"
	-@erase "$(INTDIR)\roundit.obj"
	-@erase "$(INTDIR)\Ruler.obj"
	-@erase "$(INTDIR)\Scsi.obj"
	-@erase "$(INTDIR)\SCSIExplorer.obj"
	-@erase "$(INTDIR)\server.obj"
	-@erase "$(INTDIR)\ServerCfg.obj"
	-@erase "$(INTDIR)\ShellAssociate.obj"
	-@erase "$(INTDIR)\SPLITDLG.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\SYSMEM.OBJ"
	-@erase "$(INTDIR)\TRANSDLG.OBJ"
	-@erase "$(INTDIR)\transformlist.obj"
	-@erase "$(INTDIR)\TransformMgrPage.obj"
	-@erase "$(INTDIR)\TRANSFRM.OBJ"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\visualxf.obj"
	-@erase "$(INTDIR)\WINSPLIT.OBJ"
	-@erase "$(INTDIR)\XformSave.obj"
	-@erase "$(INTDIR)\ZEUS.OBJ"
	-@erase "$(INTDIR)\zeus.res"
	-@erase "$(INTDIR)\zeusdoc.obj"
	-@erase "$(OUTDIR)\rp95.exe"
	-@erase "$(OUTDIR)\rp95.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Gr /MT /W3 /GX /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "NEWDEVICE" /D "USEDONGLE" /D "_MBCS" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\dongle/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\zeus.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zeus.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=oldnames.lib /nologo /stack:0x32000 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\rp95.pdb" /map:"$(INTDIR)\rp95.map"\
 /machine:IX86 /out:"$(OUTDIR)\rp95.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AdvancedDevice.obj" \
	"$(INTDIR)\BHEADER.OBJ" \
	"$(INTDIR)\cabinstall.obj" \
	"$(INTDIR)\CANVAS.OBJ" \
	"$(INTDIR)\CQUEUE.OBJ" \
	"$(INTDIR)\CRIP.OBJ" \
	"$(INTDIR)\CTM2RECT.OBJ" \
	"$(INTDIR)\DECOMP.OBJ" \
	"$(INTDIR)\devconnect.obj" \
	"$(INTDIR)\devdlg.obj" \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\DeviceList.obj" \
	"$(INTDIR)\DevListPage.obj" \
	"$(INTDIR)\DISP.OBJ" \
	"$(INTDIR)\dlgwait.obj" \
	"$(INTDIR)\DongleClass.obj" \
	"$(INTDIR)\fileinfo.obj" \
	"$(INTDIR)\Frect.obj" \
	"$(INTDIR)\general.obj" \
	"$(INTDIR)\GlobalProperties.obj" \
	"$(INTDIR)\ImagePause.obj" \
	"$(INTDIR)\loadtransform.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\newformv.obj" \
	"$(INTDIR)\nformat.obj" \
	"$(INTDIR)\OurPropertySheet.obj" \
	"$(INTDIR)\plist.obj" \
	"$(INTDIR)\PREVIEW.OBJ" \
	"$(INTDIR)\PreviewPrefsPage.obj" \
	"$(INTDIR)\PrinterNameChange.obj" \
	"$(INTDIR)\ProcessManager.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\qprefs.obj" \
	"$(INTDIR)\QueueChild.obj" \
	"$(INTDIR)\report.obj" \
	"$(INTDIR)\REXDIRDI.OBJ" \
	"$(INTDIR)\ripitb.obj" \
	"$(INTDIR)\ripitj.obj" \
	"$(INTDIR)\ripprocess.obj" \
	"$(INTDIR)\ripthrd.obj" \
	"$(INTDIR)\roundit.obj" \
	"$(INTDIR)\Ruler.obj" \
	"$(INTDIR)\Scsi.obj" \
	"$(INTDIR)\SCSIExplorer.obj" \
	"$(INTDIR)\server.obj" \
	"$(INTDIR)\ServerCfg.obj" \
	"$(INTDIR)\ShellAssociate.obj" \
	"$(INTDIR)\SPLITDLG.OBJ" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\SYSMEM.OBJ" \
	"$(INTDIR)\TRANSDLG.OBJ" \
	"$(INTDIR)\transformlist.obj" \
	"$(INTDIR)\TransformMgrPage.obj" \
	"$(INTDIR)\TRANSFRM.OBJ" \
	"$(INTDIR)\visualxf.obj" \
	"$(INTDIR)\WINSPLIT.OBJ" \
	"$(INTDIR)\XformSave.obj" \
	"$(INTDIR)\ZEUS.OBJ" \
	"$(INTDIR)\zeus.res" \
	"$(INTDIR)\zeusdoc.obj" \
	".\lead80\lib\ltfil_n.lib" \
	".\lead80\lib\ltimg_n.lib" \
	".\lead80\lib\ltkrn_n.lib" \
	".\Spromeps.lib"

"$(OUTDIR)\rp95.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

OUTDIR=.\Demo
INTDIR=.\Demo
# Begin Custom Macros
OutDir=.\.\Demo
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\rp95.exe"

!ELSE 

ALL : "$(OUTDIR)\rp95.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AdvancedDevice.obj"
	-@erase "$(INTDIR)\BHEADER.OBJ"
	-@erase "$(INTDIR)\cabinstall.obj"
	-@erase "$(INTDIR)\CANVAS.OBJ"
	-@erase "$(INTDIR)\CQUEUE.OBJ"
	-@erase "$(INTDIR)\CRIP.OBJ"
	-@erase "$(INTDIR)\CTM2RECT.OBJ"
	-@erase "$(INTDIR)\DECOMP.OBJ"
	-@erase "$(INTDIR)\devconnect.obj"
	-@erase "$(INTDIR)\devdlg.obj"
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\DeviceList.obj"
	-@erase "$(INTDIR)\DevListPage.obj"
	-@erase "$(INTDIR)\DISP.OBJ"
	-@erase "$(INTDIR)\dlgwait.obj"
	-@erase "$(INTDIR)\DongleClass.obj"
	-@erase "$(INTDIR)\fileinfo.obj"
	-@erase "$(INTDIR)\Frect.obj"
	-@erase "$(INTDIR)\general.obj"
	-@erase "$(INTDIR)\GlobalProperties.obj"
	-@erase "$(INTDIR)\ImagePause.obj"
	-@erase "$(INTDIR)\loadtransform.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\newformv.obj"
	-@erase "$(INTDIR)\nformat.obj"
	-@erase "$(INTDIR)\OurPropertySheet.obj"
	-@erase "$(INTDIR)\plist.obj"
	-@erase "$(INTDIR)\PREVIEW.OBJ"
	-@erase "$(INTDIR)\PreviewPrefsPage.obj"
	-@erase "$(INTDIR)\PrinterNameChange.obj"
	-@erase "$(INTDIR)\ProcessManager.obj"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\qprefs.obj"
	-@erase "$(INTDIR)\QueueChild.obj"
	-@erase "$(INTDIR)\report.obj"
	-@erase "$(INTDIR)\REXDIRDI.OBJ"
	-@erase "$(INTDIR)\ripitb.obj"
	-@erase "$(INTDIR)\ripitj.obj"
	-@erase "$(INTDIR)\ripprocess.obj"
	-@erase "$(INTDIR)\ripthrd.obj"
	-@erase "$(INTDIR)\roundit.obj"
	-@erase "$(INTDIR)\Ruler.obj"
	-@erase "$(INTDIR)\Scsi.obj"
	-@erase "$(INTDIR)\SCSIExplorer.obj"
	-@erase "$(INTDIR)\server.obj"
	-@erase "$(INTDIR)\ServerCfg.obj"
	-@erase "$(INTDIR)\ShellAssociate.obj"
	-@erase "$(INTDIR)\SPLITDLG.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\SYSMEM.OBJ"
	-@erase "$(INTDIR)\TRANSDLG.OBJ"
	-@erase "$(INTDIR)\transformlist.obj"
	-@erase "$(INTDIR)\TransformMgrPage.obj"
	-@erase "$(INTDIR)\TRANSFRM.OBJ"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\visualxf.obj"
	-@erase "$(INTDIR)\WINSPLIT.OBJ"
	-@erase "$(INTDIR)\XformSave.obj"
	-@erase "$(INTDIR)\ZEUS.OBJ"
	-@erase "$(INTDIR)\zeus.res"
	-@erase "$(INTDIR)\zeusdoc.obj"
	-@erase "$(OUTDIR)\rp95.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Gr /MT /W3 /GX /Ox /Ot /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /D "DEMO" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Demo/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\zeus.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zeus.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=oldnames.lib /nologo /stack:0x32000 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\rp95.pdb" /machine:IX86\
 /out:"$(OUTDIR)\rp95.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AdvancedDevice.obj" \
	"$(INTDIR)\BHEADER.OBJ" \
	"$(INTDIR)\cabinstall.obj" \
	"$(INTDIR)\CANVAS.OBJ" \
	"$(INTDIR)\CQUEUE.OBJ" \
	"$(INTDIR)\CRIP.OBJ" \
	"$(INTDIR)\CTM2RECT.OBJ" \
	"$(INTDIR)\DECOMP.OBJ" \
	"$(INTDIR)\devconnect.obj" \
	"$(INTDIR)\devdlg.obj" \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\DeviceList.obj" \
	"$(INTDIR)\DevListPage.obj" \
	"$(INTDIR)\DISP.OBJ" \
	"$(INTDIR)\dlgwait.obj" \
	"$(INTDIR)\DongleClass.obj" \
	"$(INTDIR)\fileinfo.obj" \
	"$(INTDIR)\Frect.obj" \
	"$(INTDIR)\general.obj" \
	"$(INTDIR)\GlobalProperties.obj" \
	"$(INTDIR)\ImagePause.obj" \
	"$(INTDIR)\loadtransform.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\newformv.obj" \
	"$(INTDIR)\nformat.obj" \
	"$(INTDIR)\OurPropertySheet.obj" \
	"$(INTDIR)\plist.obj" \
	"$(INTDIR)\PREVIEW.OBJ" \
	"$(INTDIR)\PreviewPrefsPage.obj" \
	"$(INTDIR)\PrinterNameChange.obj" \
	"$(INTDIR)\ProcessManager.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\qprefs.obj" \
	"$(INTDIR)\QueueChild.obj" \
	"$(INTDIR)\report.obj" \
	"$(INTDIR)\REXDIRDI.OBJ" \
	"$(INTDIR)\ripitb.obj" \
	"$(INTDIR)\ripitj.obj" \
	"$(INTDIR)\ripprocess.obj" \
	"$(INTDIR)\ripthrd.obj" \
	"$(INTDIR)\roundit.obj" \
	"$(INTDIR)\Ruler.obj" \
	"$(INTDIR)\Scsi.obj" \
	"$(INTDIR)\SCSIExplorer.obj" \
	"$(INTDIR)\server.obj" \
	"$(INTDIR)\ServerCfg.obj" \
	"$(INTDIR)\ShellAssociate.obj" \
	"$(INTDIR)\SPLITDLG.OBJ" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\SYSMEM.OBJ" \
	"$(INTDIR)\TRANSDLG.OBJ" \
	"$(INTDIR)\transformlist.obj" \
	"$(INTDIR)\TransformMgrPage.obj" \
	"$(INTDIR)\TRANSFRM.OBJ" \
	"$(INTDIR)\visualxf.obj" \
	"$(INTDIR)\WINSPLIT.OBJ" \
	"$(INTDIR)\XformSave.obj" \
	"$(INTDIR)\ZEUS.OBJ" \
	"$(INTDIR)\zeus.res" \
	"$(INTDIR)\zeusdoc.obj" \
	".\lead80\lib\ltfil_n.lib" \
	".\lead80\lib\ltimg_n.lib" \
	".\lead80\lib\ltkrn_n.lib" \
	".\Spromeps.lib"

"$(OUTDIR)\rp95.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

OUTDIR=.\rp_kanji_d
INTDIR=.\rp_kanji_d
# Begin Custom Macros
OutDir=.\.\rp_kanji_d
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\rp95.exe" "$(OUTDIR)\zeus.bsc"

!ELSE 

ALL : "$(OUTDIR)\rp95.exe" "$(OUTDIR)\zeus.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AdvancedDevice.obj"
	-@erase "$(INTDIR)\AdvancedDevice.sbr"
	-@erase "$(INTDIR)\BHEADER.OBJ"
	-@erase "$(INTDIR)\BHEADER.SBR"
	-@erase "$(INTDIR)\cabinstall.obj"
	-@erase "$(INTDIR)\cabinstall.sbr"
	-@erase "$(INTDIR)\CANVAS.OBJ"
	-@erase "$(INTDIR)\CANVAS.SBR"
	-@erase "$(INTDIR)\CQUEUE.OBJ"
	-@erase "$(INTDIR)\CQUEUE.SBR"
	-@erase "$(INTDIR)\CRIP.OBJ"
	-@erase "$(INTDIR)\CRIP.SBR"
	-@erase "$(INTDIR)\CTM2RECT.OBJ"
	-@erase "$(INTDIR)\CTM2RECT.SBR"
	-@erase "$(INTDIR)\DECOMP.OBJ"
	-@erase "$(INTDIR)\DECOMP.SBR"
	-@erase "$(INTDIR)\devconnect.obj"
	-@erase "$(INTDIR)\devconnect.sbr"
	-@erase "$(INTDIR)\devdlg.obj"
	-@erase "$(INTDIR)\devdlg.sbr"
	-@erase "$(INTDIR)\device.obj"
	-@erase "$(INTDIR)\device.sbr"
	-@erase "$(INTDIR)\DeviceList.obj"
	-@erase "$(INTDIR)\DeviceList.sbr"
	-@erase "$(INTDIR)\DevListPage.obj"
	-@erase "$(INTDIR)\DevListPage.sbr"
	-@erase "$(INTDIR)\DISP.OBJ"
	-@erase "$(INTDIR)\DISP.SBR"
	-@erase "$(INTDIR)\dlgwait.obj"
	-@erase "$(INTDIR)\dlgwait.sbr"
	-@erase "$(INTDIR)\DongleClass.obj"
	-@erase "$(INTDIR)\DongleClass.sbr"
	-@erase "$(INTDIR)\fileinfo.obj"
	-@erase "$(INTDIR)\fileinfo.sbr"
	-@erase "$(INTDIR)\Frect.obj"
	-@erase "$(INTDIR)\Frect.sbr"
	-@erase "$(INTDIR)\general.obj"
	-@erase "$(INTDIR)\general.sbr"
	-@erase "$(INTDIR)\GlobalProperties.obj"
	-@erase "$(INTDIR)\GlobalProperties.sbr"
	-@erase "$(INTDIR)\icon.res"
	-@erase "$(INTDIR)\ImagePause.obj"
	-@erase "$(INTDIR)\ImagePause.sbr"
	-@erase "$(INTDIR)\loadtransform.obj"
	-@erase "$(INTDIR)\loadtransform.sbr"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\mainfrm.sbr"
	-@erase "$(INTDIR)\newformv.obj"
	-@erase "$(INTDIR)\newformv.sbr"
	-@erase "$(INTDIR)\nformat.obj"
	-@erase "$(INTDIR)\nformat.sbr"
	-@erase "$(INTDIR)\OurPropertySheet.obj"
	-@erase "$(INTDIR)\OurPropertySheet.sbr"
	-@erase "$(INTDIR)\plist.obj"
	-@erase "$(INTDIR)\plist.sbr"
	-@erase "$(INTDIR)\PREVIEW.OBJ"
	-@erase "$(INTDIR)\PREVIEW.SBR"
	-@erase "$(INTDIR)\PreviewPrefsPage.obj"
	-@erase "$(INTDIR)\PreviewPrefsPage.sbr"
	-@erase "$(INTDIR)\PrinterNameChange.obj"
	-@erase "$(INTDIR)\PrinterNameChange.sbr"
	-@erase "$(INTDIR)\ProcessManager.obj"
	-@erase "$(INTDIR)\ProcessManager.sbr"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\profile.sbr"
	-@erase "$(INTDIR)\qprefs.obj"
	-@erase "$(INTDIR)\qprefs.sbr"
	-@erase "$(INTDIR)\QueueChild.obj"
	-@erase "$(INTDIR)\QueueChild.sbr"
	-@erase "$(INTDIR)\report.obj"
	-@erase "$(INTDIR)\report.sbr"
	-@erase "$(INTDIR)\REXDIRDI.OBJ"
	-@erase "$(INTDIR)\REXDIRDI.SBR"
	-@erase "$(INTDIR)\ripitb.obj"
	-@erase "$(INTDIR)\ripitb.sbr"
	-@erase "$(INTDIR)\ripitj.obj"
	-@erase "$(INTDIR)\ripitj.sbr"
	-@erase "$(INTDIR)\ripprocess.obj"
	-@erase "$(INTDIR)\ripprocess.sbr"
	-@erase "$(INTDIR)\ripthrd.obj"
	-@erase "$(INTDIR)\ripthrd.sbr"
	-@erase "$(INTDIR)\roundit.obj"
	-@erase "$(INTDIR)\roundit.sbr"
	-@erase "$(INTDIR)\Ruler.obj"
	-@erase "$(INTDIR)\Ruler.sbr"
	-@erase "$(INTDIR)\Scsi.obj"
	-@erase "$(INTDIR)\Scsi.sbr"
	-@erase "$(INTDIR)\SCSIExplorer.obj"
	-@erase "$(INTDIR)\SCSIExplorer.sbr"
	-@erase "$(INTDIR)\server.obj"
	-@erase "$(INTDIR)\server.sbr"
	-@erase "$(INTDIR)\ServerCfg.obj"
	-@erase "$(INTDIR)\ServerCfg.sbr"
	-@erase "$(INTDIR)\ShellAssociate.obj"
	-@erase "$(INTDIR)\ShellAssociate.sbr"
	-@erase "$(INTDIR)\SPLITDLG.OBJ"
	-@erase "$(INTDIR)\SPLITDLG.SBR"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\STDAFX.SBR"
	-@erase "$(INTDIR)\SYSMEM.OBJ"
	-@erase "$(INTDIR)\SYSMEM.SBR"
	-@erase "$(INTDIR)\TRANSDLG.OBJ"
	-@erase "$(INTDIR)\TRANSDLG.SBR"
	-@erase "$(INTDIR)\transformlist.obj"
	-@erase "$(INTDIR)\transformlist.sbr"
	-@erase "$(INTDIR)\TransformMgrPage.obj"
	-@erase "$(INTDIR)\TransformMgrPage.sbr"
	-@erase "$(INTDIR)\TRANSFRM.OBJ"
	-@erase "$(INTDIR)\TRANSFRM.SBR"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\visualxf.obj"
	-@erase "$(INTDIR)\visualxf.sbr"
	-@erase "$(INTDIR)\WINSPLIT.OBJ"
	-@erase "$(INTDIR)\WINSPLIT.SBR"
	-@erase "$(INTDIR)\XformSave.obj"
	-@erase "$(INTDIR)\XformSave.sbr"
	-@erase "$(INTDIR)\ZEUS.OBJ"
	-@erase "$(INTDIR)\ZEUS.SBR"
	-@erase "$(INTDIR)\zeusdoc.obj"
	-@erase "$(INTDIR)\zeusdoc.sbr"
	-@erase "$(OUTDIR)\rp95.exe"
	-@erase "$(OUTDIR)\rp95.map"
	-@erase "$(OUTDIR)\rp95.pdb"
	-@erase "$(OUTDIR)\zeus.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "NEWDEVICE" /D "_MBCS" /D "USEKANJI" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\zeus.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\rp_kanji_d/
CPP_SBRS=.\rp_kanji_d/

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\icon.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zeus.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AdvancedDevice.sbr" \
	"$(INTDIR)\BHEADER.SBR" \
	"$(INTDIR)\cabinstall.sbr" \
	"$(INTDIR)\CANVAS.SBR" \
	"$(INTDIR)\CQUEUE.SBR" \
	"$(INTDIR)\CRIP.SBR" \
	"$(INTDIR)\CTM2RECT.SBR" \
	"$(INTDIR)\DECOMP.SBR" \
	"$(INTDIR)\devconnect.sbr" \
	"$(INTDIR)\devdlg.sbr" \
	"$(INTDIR)\device.sbr" \
	"$(INTDIR)\DeviceList.sbr" \
	"$(INTDIR)\DevListPage.sbr" \
	"$(INTDIR)\DISP.SBR" \
	"$(INTDIR)\dlgwait.sbr" \
	"$(INTDIR)\DongleClass.sbr" \
	"$(INTDIR)\fileinfo.sbr" \
	"$(INTDIR)\Frect.sbr" \
	"$(INTDIR)\general.sbr" \
	"$(INTDIR)\GlobalProperties.sbr" \
	"$(INTDIR)\ImagePause.sbr" \
	"$(INTDIR)\loadtransform.sbr" \
	"$(INTDIR)\mainfrm.sbr" \
	"$(INTDIR)\newformv.sbr" \
	"$(INTDIR)\nformat.sbr" \
	"$(INTDIR)\OurPropertySheet.sbr" \
	"$(INTDIR)\plist.sbr" \
	"$(INTDIR)\PREVIEW.SBR" \
	"$(INTDIR)\PreviewPrefsPage.sbr" \
	"$(INTDIR)\PrinterNameChange.sbr" \
	"$(INTDIR)\ProcessManager.sbr" \
	"$(INTDIR)\profile.sbr" \
	"$(INTDIR)\qprefs.sbr" \
	"$(INTDIR)\QueueChild.sbr" \
	"$(INTDIR)\report.sbr" \
	"$(INTDIR)\REXDIRDI.SBR" \
	"$(INTDIR)\ripitb.sbr" \
	"$(INTDIR)\ripitj.sbr" \
	"$(INTDIR)\ripprocess.sbr" \
	"$(INTDIR)\ripthrd.sbr" \
	"$(INTDIR)\roundit.sbr" \
	"$(INTDIR)\Ruler.sbr" \
	"$(INTDIR)\Scsi.sbr" \
	"$(INTDIR)\SCSIExplorer.sbr" \
	"$(INTDIR)\server.sbr" \
	"$(INTDIR)\ServerCfg.sbr" \
	"$(INTDIR)\ShellAssociate.sbr" \
	"$(INTDIR)\SPLITDLG.SBR" \
	"$(INTDIR)\STDAFX.SBR" \
	"$(INTDIR)\SYSMEM.SBR" \
	"$(INTDIR)\TRANSDLG.SBR" \
	"$(INTDIR)\transformlist.sbr" \
	"$(INTDIR)\TransformMgrPage.sbr" \
	"$(INTDIR)\TRANSFRM.SBR" \
	"$(INTDIR)\visualxf.sbr" \
	"$(INTDIR)\WINSPLIT.SBR" \
	"$(INTDIR)\XformSave.sbr" \
	"$(INTDIR)\ZEUS.SBR" \
	"$(INTDIR)\zeusdoc.sbr"

"$(OUTDIR)\zeus.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=oldnames.lib /nologo /stack:0x32000 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\rp95.pdb" /map:"$(INTDIR)\rp95.map" /debug\
 /machine:IX86 /out:"$(OUTDIR)\rp95.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AdvancedDevice.obj" \
	"$(INTDIR)\BHEADER.OBJ" \
	"$(INTDIR)\cabinstall.obj" \
	"$(INTDIR)\CANVAS.OBJ" \
	"$(INTDIR)\CQUEUE.OBJ" \
	"$(INTDIR)\CRIP.OBJ" \
	"$(INTDIR)\CTM2RECT.OBJ" \
	"$(INTDIR)\DECOMP.OBJ" \
	"$(INTDIR)\devconnect.obj" \
	"$(INTDIR)\devdlg.obj" \
	"$(INTDIR)\device.obj" \
	"$(INTDIR)\DeviceList.obj" \
	"$(INTDIR)\DevListPage.obj" \
	"$(INTDIR)\DISP.OBJ" \
	"$(INTDIR)\dlgwait.obj" \
	"$(INTDIR)\DongleClass.obj" \
	"$(INTDIR)\fileinfo.obj" \
	"$(INTDIR)\Frect.obj" \
	"$(INTDIR)\general.obj" \
	"$(INTDIR)\GlobalProperties.obj" \
	"$(INTDIR)\icon.res" \
	"$(INTDIR)\ImagePause.obj" \
	"$(INTDIR)\loadtransform.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\newformv.obj" \
	"$(INTDIR)\nformat.obj" \
	"$(INTDIR)\OurPropertySheet.obj" \
	"$(INTDIR)\plist.obj" \
	"$(INTDIR)\PREVIEW.OBJ" \
	"$(INTDIR)\PreviewPrefsPage.obj" \
	"$(INTDIR)\PrinterNameChange.obj" \
	"$(INTDIR)\ProcessManager.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\qprefs.obj" \
	"$(INTDIR)\QueueChild.obj" \
	"$(INTDIR)\report.obj" \
	"$(INTDIR)\REXDIRDI.OBJ" \
	"$(INTDIR)\ripitb.obj" \
	"$(INTDIR)\ripitj.obj" \
	"$(INTDIR)\ripprocess.obj" \
	"$(INTDIR)\ripthrd.obj" \
	"$(INTDIR)\roundit.obj" \
	"$(INTDIR)\Ruler.obj" \
	"$(INTDIR)\Scsi.obj" \
	"$(INTDIR)\SCSIExplorer.obj" \
	"$(INTDIR)\server.obj" \
	"$(INTDIR)\ServerCfg.obj" \
	"$(INTDIR)\ShellAssociate.obj" \
	"$(INTDIR)\SPLITDLG.OBJ" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\SYSMEM.OBJ" \
	"$(INTDIR)\TRANSDLG.OBJ" \
	"$(INTDIR)\transformlist.obj" \
	"$(INTDIR)\TransformMgrPage.obj" \
	"$(INTDIR)\TRANSFRM.OBJ" \
	"$(INTDIR)\visualxf.obj" \
	"$(INTDIR)\WINSPLIT.OBJ" \
	"$(INTDIR)\XformSave.obj" \
	"$(INTDIR)\ZEUS.OBJ" \
	"$(INTDIR)\zeusdoc.obj" \
	".\lead80\lib\ltfil_n.lib" \
	".\lead80\lib\ltimg_n.lib" \
	".\lead80\lib\ltkrn_n.lib" \
	".\Spromeps.lib"

"$(OUTDIR)\rp95.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "zeus32 - Win32 Debug" || "$(CFG)" ==\
 "zeus32 - Win32 Fastcall Release" || "$(CFG)" ==\
 "zeus32 - Win32 Fastcall Release Kanji" || "$(CFG)" ==\
 "zeus32 - Win32 Fastcall Release Dongle" || "$(CFG)" == "zeus32 - Win32 Demo"\
 || "$(CFG)" == "zeus32 - Win32 Debug Kanji"
SOURCE=.\AdvancedDevice.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_ADVAN=\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devface.h"\
	".\nformat.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	

"$(INTDIR)\AdvancedDevice.obj"	"$(INTDIR)\AdvancedDevice.sbr" : $(SOURCE)\
 $(DEP_CPP_ADVAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_ADVAN=\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devface.h"\
	".\nformat.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	

"$(INTDIR)\AdvancedDevice.obj" : $(SOURCE) $(DEP_CPP_ADVAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_ADVAN=\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devface.h"\
	".\nformat.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	

"$(INTDIR)\AdvancedDevice.obj" : $(SOURCE) $(DEP_CPP_ADVAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_ADVAN=\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devface.h"\
	".\nformat.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	

"$(INTDIR)\AdvancedDevice.obj" : $(SOURCE) $(DEP_CPP_ADVAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_ADVAN=\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devface.h"\
	".\nformat.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	

"$(INTDIR)\AdvancedDevice.obj" : $(SOURCE) $(DEP_CPP_ADVAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_ADVAN=\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devface.h"\
	".\nformat.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	

"$(INTDIR)\AdvancedDevice.obj"	"$(INTDIR)\AdvancedDevice.sbr" : $(SOURCE)\
 $(DEP_CPP_ADVAN) "$(INTDIR)"


!ENDIF 

SOURCE=.\BHEADER.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_BHEAD=\
	".\Bheader.h"\
	".\control.hm"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\BHEADER.OBJ"	"$(INTDIR)\BHEADER.SBR" : $(SOURCE) $(DEP_CPP_BHEAD)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_BHEAD=\
	".\Bheader.h"\
	".\control.hm"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\BHEADER.OBJ" : $(SOURCE) $(DEP_CPP_BHEAD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_BHEAD=\
	".\Bheader.h"\
	".\control.hm"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\BHEADER.OBJ" : $(SOURCE) $(DEP_CPP_BHEAD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_BHEAD=\
	".\Bheader.h"\
	".\control.hm"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\BHEADER.OBJ" : $(SOURCE) $(DEP_CPP_BHEAD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_BHEAD=\
	".\Bheader.h"\
	".\control.hm"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\BHEADER.OBJ" : $(SOURCE) $(DEP_CPP_BHEAD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_BHEAD=\
	".\Bheader.h"\
	".\control.hm"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\BHEADER.OBJ"	"$(INTDIR)\BHEADER.SBR" : $(SOURCE) $(DEP_CPP_BHEAD)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\cabinstall.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_CABIN=\
	".\cabinstall.h"\
	".\Rexdirdi.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\cabinstall.obj"	"$(INTDIR)\cabinstall.sbr" : $(SOURCE)\
 $(DEP_CPP_CABIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_CABIN=\
	".\cabinstall.h"\
	".\Rexdirdi.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\cabinstall.obj" : $(SOURCE) $(DEP_CPP_CABIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_CABIN=\
	".\cabinstall.h"\
	".\Rexdirdi.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\cabinstall.obj" : $(SOURCE) $(DEP_CPP_CABIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_CABIN=\
	".\cabinstall.h"\
	".\Rexdirdi.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\cabinstall.obj" : $(SOURCE) $(DEP_CPP_CABIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_CABIN=\
	".\cabinstall.h"\
	".\Rexdirdi.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\cabinstall.obj" : $(SOURCE) $(DEP_CPP_CABIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_CABIN=\
	".\cabinstall.h"\
	".\Rexdirdi.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\cabinstall.obj"	"$(INTDIR)\cabinstall.sbr" : $(SOURCE)\
 $(DEP_CPP_CABIN) "$(INTDIR)"


!ENDIF 

SOURCE=.\CANVAS.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_CANVA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CANVAS.OBJ"	"$(INTDIR)\CANVAS.SBR" : $(SOURCE) $(DEP_CPP_CANVA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_CANVA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CANVAS.OBJ" : $(SOURCE) $(DEP_CPP_CANVA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_CANVA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CANVAS.OBJ" : $(SOURCE) $(DEP_CPP_CANVA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_CANVA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CANVAS.OBJ" : $(SOURCE) $(DEP_CPP_CANVA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_CANVA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CANVAS.OBJ" : $(SOURCE) $(DEP_CPP_CANVA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_CANVA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CANVAS.OBJ"	"$(INTDIR)\CANVAS.SBR" : $(SOURCE) $(DEP_CPP_CANVA)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\CQUEUE.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_CQUEU=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\nformat.h"\
	".\ps_lookup.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CQUEUE.OBJ"	"$(INTDIR)\CQUEUE.SBR" : $(SOURCE) $(DEP_CPP_CQUEU)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_CQUEU=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\nformat.h"\
	".\ps_lookup.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CQUEUE.OBJ" : $(SOURCE) $(DEP_CPP_CQUEU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_CQUEU=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\nformat.h"\
	".\ps_lookup.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CQUEUE.OBJ" : $(SOURCE) $(DEP_CPP_CQUEU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_CQUEU=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\nformat.h"\
	".\ps_lookup.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CQUEUE.OBJ" : $(SOURCE) $(DEP_CPP_CQUEU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_CQUEU=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\nformat.h"\
	".\ps_lookup.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CQUEUE.OBJ" : $(SOURCE) $(DEP_CPP_CQUEU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_CQUEU=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\nformat.h"\
	".\ps_lookup.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CQUEUE.OBJ"	"$(INTDIR)\CQUEUE.SBR" : $(SOURCE) $(DEP_CPP_CQUEU)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\CRIP.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_CRIP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Batch.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CRIP.OBJ"	"$(INTDIR)\CRIP.SBR" : $(SOURCE) $(DEP_CPP_CRIP_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_CRIP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Batch.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CRIP.OBJ" : $(SOURCE) $(DEP_CPP_CRIP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_CRIP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Batch.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CRIP.OBJ" : $(SOURCE) $(DEP_CPP_CRIP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_CRIP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Batch.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CRIP.OBJ" : $(SOURCE) $(DEP_CPP_CRIP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_CRIP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Batch.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CRIP.OBJ" : $(SOURCE) $(DEP_CPP_CRIP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_CRIP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Batch.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\CRIP.OBJ"	"$(INTDIR)\CRIP.SBR" : $(SOURCE) $(DEP_CPP_CRIP_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\CTM2RECT.CPP
DEP_CPP_CTM2R=\
	".\Ctm2rect.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\zeus.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CTM2RECT.OBJ"	"$(INTDIR)\CTM2RECT.SBR" : $(SOURCE) $(DEP_CPP_CTM2R)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CTM2RECT.OBJ" : $(SOURCE) $(DEP_CPP_CTM2R) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Od /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /D "USEKANJI" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CTM2RECT.OBJ" : $(SOURCE) $(DEP_CPP_CTM2R) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Od /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "USEDONGLE" /D "_MBCS" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CTM2RECT.OBJ" : $(SOURCE) $(DEP_CPP_CTM2R) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Ox /Ot /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /D "DEMO" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CTM2RECT.OBJ" : $(SOURCE) $(DEP_CPP_CTM2R) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

CPP_SWITCHES=/nologo /MTd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "NEWDEVICE" /D "_MBCS" /D "USEKANJI" /Fr"$(INTDIR)\\"\
 /Fp"$(INTDIR)\zeus.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CTM2RECT.OBJ"	"$(INTDIR)\CTM2RECT.SBR" : $(SOURCE) $(DEP_CPP_CTM2R)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\DECOMP.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_DECOM=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	

"$(INTDIR)\DECOMP.OBJ"	"$(INTDIR)\DECOMP.SBR" : $(SOURCE) $(DEP_CPP_DECOM)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_DECOM=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	

"$(INTDIR)\DECOMP.OBJ" : $(SOURCE) $(DEP_CPP_DECOM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_DECOM=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	

"$(INTDIR)\DECOMP.OBJ" : $(SOURCE) $(DEP_CPP_DECOM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_DECOM=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	

"$(INTDIR)\DECOMP.OBJ" : $(SOURCE) $(DEP_CPP_DECOM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_DECOM=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	

"$(INTDIR)\DECOMP.OBJ" : $(SOURCE) $(DEP_CPP_DECOM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_DECOM=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	

"$(INTDIR)\DECOMP.OBJ"	"$(INTDIR)\DECOMP.SBR" : $(SOURCE) $(DEP_CPP_DECOM)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\devconnect.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_DEVCO=\
	".\devconnect.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\devconnect.obj"	"$(INTDIR)\devconnect.sbr" : $(SOURCE)\
 $(DEP_CPP_DEVCO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_DEVCO=\
	".\devconnect.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\devconnect.obj" : $(SOURCE) $(DEP_CPP_DEVCO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_DEVCO=\
	".\devconnect.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\devconnect.obj" : $(SOURCE) $(DEP_CPP_DEVCO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_DEVCO=\
	".\devconnect.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\devconnect.obj" : $(SOURCE) $(DEP_CPP_DEVCO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_DEVCO=\
	".\devconnect.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\devconnect.obj" : $(SOURCE) $(DEP_CPP_DEVCO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_DEVCO=\
	".\devconnect.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\devconnect.obj"	"$(INTDIR)\devconnect.sbr" : $(SOURCE)\
 $(DEP_CPP_DEVCO) "$(INTDIR)"


!ENDIF 

SOURCE=.\devdlg.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_DEVDL=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\nformat.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\zeus.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\devdlg.obj"	"$(INTDIR)\devdlg.sbr" : $(SOURCE) $(DEP_CPP_DEVDL)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_DEVDL=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\nformat.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\devdlg.obj" : $(SOURCE) $(DEP_CPP_DEVDL) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_DEVDL=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\nformat.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Od /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /D "USEKANJI" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\devdlg.obj" : $(SOURCE) $(DEP_CPP_DEVDL) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_DEVDL=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\nformat.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Od /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "USEDONGLE" /D "_MBCS" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\devdlg.obj" : $(SOURCE) $(DEP_CPP_DEVDL) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_DEVDL=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\nformat.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Ox /Ot /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /D "DEMO" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\devdlg.obj" : $(SOURCE) $(DEP_CPP_DEVDL) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_DEVDL=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\AdvancedDevice.h"\
	".\control.hm"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\nformat.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /MTd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "NEWDEVICE" /D "_MBCS" /D "USEKANJI" /Fr"$(INTDIR)\\"\
 /Fp"$(INTDIR)\zeus.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\devdlg.obj"	"$(INTDIR)\devdlg.sbr" : $(SOURCE) $(DEP_CPP_DEVDL)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\device.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_DEVIC=\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\zeus.pch"\
 /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\device.obj"	"$(INTDIR)\device.sbr" : $(SOURCE) $(DEP_CPP_DEVIC)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_DEVIC=\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\device.obj" : $(SOURCE) $(DEP_CPP_DEVIC) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_DEVIC=\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Od /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /D "USEKANJI" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\device.obj" : $(SOURCE) $(DEP_CPP_DEVIC) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_DEVIC=\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Od /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "USEDONGLE" /D "_MBCS" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\device.obj" : $(SOURCE) $(DEP_CPP_DEVIC) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_DEVIC=\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /G5 /Gr /MT /W3 /GX /Ox /Ot /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "NEWDEVICE" /D "_MBCS" /D "DEMO" /Fp"$(INTDIR)\zeus.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\device.obj" : $(SOURCE) $(DEP_CPP_DEVIC) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_DEVIC=\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Zeus.h"\
	
CPP_SWITCHES=/nologo /MTd /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "NEWDEVICE" /D "_MBCS" /D "USEKANJI" /Fr"$(INTDIR)\\"\
 /Fp"$(INTDIR)\zeus.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\device.obj"	"$(INTDIR)\device.sbr" : $(SOURCE) $(DEP_CPP_DEVIC)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\DeviceList.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_DEVICE=\
	".\DeviceList.h"\
	".\Inidefs.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DeviceList.obj"	"$(INTDIR)\DeviceList.sbr" : $(SOURCE)\
 $(DEP_CPP_DEVICE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_DEVICE=\
	".\DeviceList.h"\
	".\Inidefs.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DeviceList.obj" : $(SOURCE) $(DEP_CPP_DEVICE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_DEVICE=\
	".\DeviceList.h"\
	".\Inidefs.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DeviceList.obj" : $(SOURCE) $(DEP_CPP_DEVICE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_DEVICE=\
	".\DeviceList.h"\
	".\Inidefs.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DeviceList.obj" : $(SOURCE) $(DEP_CPP_DEVICE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_DEVICE=\
	".\DeviceList.h"\
	".\Inidefs.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DeviceList.obj" : $(SOURCE) $(DEP_CPP_DEVICE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_DEVICE=\
	".\DeviceList.h"\
	".\Inidefs.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DeviceList.obj"	"$(INTDIR)\DeviceList.sbr" : $(SOURCE)\
 $(DEP_CPP_DEVICE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DevListPage.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_DEVLI=\
	".\cabinstall.h"\
	".\control.hm"\
	".\devconnect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\dlgwait.h"\
	".\PrinterNameChange.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DevListPage.obj"	"$(INTDIR)\DevListPage.sbr" : $(SOURCE)\
 $(DEP_CPP_DEVLI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_DEVLI=\
	".\cabinstall.h"\
	".\control.hm"\
	".\devconnect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\dlgwait.h"\
	".\PrinterNameChange.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DevListPage.obj" : $(SOURCE) $(DEP_CPP_DEVLI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_DEVLI=\
	".\cabinstall.h"\
	".\control.hm"\
	".\devconnect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\dlgwait.h"\
	".\PrinterNameChange.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DevListPage.obj" : $(SOURCE) $(DEP_CPP_DEVLI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_DEVLI=\
	".\cabinstall.h"\
	".\control.hm"\
	".\devconnect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\dlgwait.h"\
	".\PrinterNameChange.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DevListPage.obj" : $(SOURCE) $(DEP_CPP_DEVLI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_DEVLI=\
	".\cabinstall.h"\
	".\control.hm"\
	".\devconnect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\dlgwait.h"\
	".\PrinterNameChange.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DevListPage.obj" : $(SOURCE) $(DEP_CPP_DEVLI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_DEVLI=\
	".\cabinstall.h"\
	".\control.hm"\
	".\devconnect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\dlgwait.h"\
	".\PrinterNameChange.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DevListPage.obj"	"$(INTDIR)\DevListPage.sbr" : $(SOURCE)\
 $(DEP_CPP_DEVLI) "$(INTDIR)"


!ENDIF 

SOURCE=.\DISP.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_DISP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Prev.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DISP.OBJ"	"$(INTDIR)\DISP.SBR" : $(SOURCE) $(DEP_CPP_DISP_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_DISP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Prev.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DISP.OBJ" : $(SOURCE) $(DEP_CPP_DISP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_DISP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Prev.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DISP.OBJ" : $(SOURCE) $(DEP_CPP_DISP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_DISP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Prev.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DISP.OBJ" : $(SOURCE) $(DEP_CPP_DISP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_DISP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Prev.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DISP.OBJ" : $(SOURCE) $(DEP_CPP_DISP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_DISP_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Disp.h"\
	".\Inidefs.h"\
	".\Prev.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\DISP.OBJ"	"$(INTDIR)\DISP.SBR" : $(SOURCE) $(DEP_CPP_DISP_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\dlgwait.cpp
DEP_CPP_DLGWA=\
	".\dlgwait.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\dlgwait.obj"	"$(INTDIR)\dlgwait.sbr" : $(SOURCE) $(DEP_CPP_DLGWA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\dlgwait.obj" : $(SOURCE) $(DEP_CPP_DLGWA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\dlgwait.obj" : $(SOURCE) $(DEP_CPP_DLGWA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\dlgwait.obj" : $(SOURCE) $(DEP_CPP_DLGWA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\dlgwait.obj" : $(SOURCE) $(DEP_CPP_DLGWA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\dlgwait.obj"	"$(INTDIR)\dlgwait.sbr" : $(SOURCE) $(DEP_CPP_DLGWA)\
 "$(INTDIR)"


!ENDIF 

SOURCE=..\dongle\DongleClass.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_DONGL=\
	"..\dongle\dongleclass.h"\
	"..\dongle\spromeps.h"\
	"..\dongle\stdafx.h"\
	

"$(INTDIR)\DongleClass.obj"	"$(INTDIR)\DongleClass.sbr" : $(SOURCE)\
 $(DEP_CPP_DONGL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_DONGL=\
	"..\dongle\dongleclass.h"\
	"..\dongle\spromeps.h"\
	"..\dongle\stdafx.h"\
	

"$(INTDIR)\DongleClass.obj" : $(SOURCE) $(DEP_CPP_DONGL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_DONGL=\
	"..\dongle\dongleclass.h"\
	"..\dongle\spromeps.h"\
	"..\dongle\stdafx.h"\
	

"$(INTDIR)\DongleClass.obj" : $(SOURCE) $(DEP_CPP_DONGL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_DONGL=\
	"..\dongle\dongleclass.h"\
	"..\dongle\spromeps.h"\
	"..\dongle\stdafx.h"\
	

"$(INTDIR)\DongleClass.obj" : $(SOURCE) $(DEP_CPP_DONGL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_DONGL=\
	"..\dongle\dongleclass.h"\
	"..\dongle\spromeps.h"\
	"..\dongle\stdafx.h"\
	

"$(INTDIR)\DongleClass.obj" : $(SOURCE) $(DEP_CPP_DONGL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_DONGL=\
	"..\dongle\dongleclass.h"\
	"..\dongle\spromeps.h"\
	"..\dongle\stdafx.h"\
	

"$(INTDIR)\DongleClass.obj"	"$(INTDIR)\DongleClass.sbr" : $(SOURCE)\
 $(DEP_CPP_DONGL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\fileinfo.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_FILEI=\
	".\control.hm"\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\fileinfo.h"\
	".\OurPropertySheet.h"\
	".\Prefsmsg.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\fileinfo.obj"	"$(INTDIR)\fileinfo.sbr" : $(SOURCE) $(DEP_CPP_FILEI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_FILEI=\
	".\control.hm"\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\fileinfo.h"\
	".\OurPropertySheet.h"\
	".\Prefsmsg.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\fileinfo.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_FILEI=\
	".\control.hm"\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\fileinfo.h"\
	".\OurPropertySheet.h"\
	".\Prefsmsg.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\fileinfo.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_FILEI=\
	".\control.hm"\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\fileinfo.h"\
	".\OurPropertySheet.h"\
	".\Prefsmsg.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\fileinfo.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_FILEI=\
	".\control.hm"\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\fileinfo.h"\
	".\OurPropertySheet.h"\
	".\Prefsmsg.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\fileinfo.obj" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_FILEI=\
	".\control.hm"\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\fileinfo.h"\
	".\OurPropertySheet.h"\
	".\Prefsmsg.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\fileinfo.obj"	"$(INTDIR)\fileinfo.sbr" : $(SOURCE) $(DEP_CPP_FILEI)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\Frect.cpp
DEP_CPP_FRECT=\
	".\Frect.h"\
	".\roundit.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\Frect.obj"	"$(INTDIR)\Frect.sbr" : $(SOURCE) $(DEP_CPP_FRECT)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\Frect.obj" : $(SOURCE) $(DEP_CPP_FRECT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\Frect.obj" : $(SOURCE) $(DEP_CPP_FRECT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\Frect.obj" : $(SOURCE) $(DEP_CPP_FRECT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\Frect.obj" : $(SOURCE) $(DEP_CPP_FRECT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\Frect.obj"	"$(INTDIR)\Frect.sbr" : $(SOURCE) $(DEP_CPP_FRECT)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\general.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_GENER=\
	".\general.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\general.obj"	"$(INTDIR)\general.sbr" : $(SOURCE) $(DEP_CPP_GENER)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_GENER=\
	".\general.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\general.obj" : $(SOURCE) $(DEP_CPP_GENER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_GENER=\
	".\general.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\general.obj" : $(SOURCE) $(DEP_CPP_GENER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_GENER=\
	".\general.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\general.obj" : $(SOURCE) $(DEP_CPP_GENER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_GENER=\
	".\general.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\general.obj" : $(SOURCE) $(DEP_CPP_GENER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_GENER=\
	".\general.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\general.obj"	"$(INTDIR)\general.sbr" : $(SOURCE) $(DEP_CPP_GENER)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\GlobalProperties.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_GLOBA=\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\GlobalProperties.h"\
	".\OurPropertySheet.h"\
	".\PreviewPrefsPage.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\GlobalProperties.obj"	"$(INTDIR)\GlobalProperties.sbr" : $(SOURCE)\
 $(DEP_CPP_GLOBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_GLOBA=\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\GlobalProperties.h"\
	".\OurPropertySheet.h"\
	".\PreviewPrefsPage.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\GlobalProperties.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_GLOBA=\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\GlobalProperties.h"\
	".\OurPropertySheet.h"\
	".\PreviewPrefsPage.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\GlobalProperties.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_GLOBA=\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\GlobalProperties.h"\
	".\OurPropertySheet.h"\
	".\PreviewPrefsPage.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\GlobalProperties.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_GLOBA=\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\GlobalProperties.h"\
	".\OurPropertySheet.h"\
	".\PreviewPrefsPage.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\GlobalProperties.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_GLOBA=\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\GlobalProperties.h"\
	".\OurPropertySheet.h"\
	".\PreviewPrefsPage.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\GlobalProperties.obj"	"$(INTDIR)\GlobalProperties.sbr" : $(SOURCE)\
 $(DEP_CPP_GLOBA) "$(INTDIR)"


!ENDIF 

SOURCE=.\icon\icon.rc

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_RSC_ICON_=\
	".\icon\Res\Ico00009.ico"\
	

"$(INTDIR)\icon.res" : $(SOURCE) $(DEP_RSC_ICON_) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\icon.res" /i "icon" /i ".\icon" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_RSC_ICON_=\
	".\icon\Res\Ico00009.ico"\
	

"$(INTDIR)\icon.res" : $(SOURCE) $(DEP_RSC_ICON_) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\icon.res" /i "icon" /i ".\icon" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

SOURCE=.\ImagePause.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_IMAGE=\
	".\ImagePause.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ImagePause.obj"	"$(INTDIR)\ImagePause.sbr" : $(SOURCE)\
 $(DEP_CPP_IMAGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_IMAGE=\
	".\ImagePause.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ImagePause.obj" : $(SOURCE) $(DEP_CPP_IMAGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_IMAGE=\
	".\ImagePause.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ImagePause.obj" : $(SOURCE) $(DEP_CPP_IMAGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_IMAGE=\
	".\ImagePause.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ImagePause.obj" : $(SOURCE) $(DEP_CPP_IMAGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_IMAGE=\
	".\ImagePause.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ImagePause.obj" : $(SOURCE) $(DEP_CPP_IMAGE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_IMAGE=\
	".\ImagePause.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ImagePause.obj"	"$(INTDIR)\ImagePause.sbr" : $(SOURCE)\
 $(DEP_CPP_IMAGE) "$(INTDIR)"


!ENDIF 

SOURCE=.\loadtransform.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_LOADT=\
	".\loadtransform.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\loadtransform.obj"	"$(INTDIR)\loadtransform.sbr" : $(SOURCE)\
 $(DEP_CPP_LOADT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_LOADT=\
	".\loadtransform.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\loadtransform.obj" : $(SOURCE) $(DEP_CPP_LOADT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_LOADT=\
	".\loadtransform.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\loadtransform.obj" : $(SOURCE) $(DEP_CPP_LOADT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_LOADT=\
	".\loadtransform.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\loadtransform.obj" : $(SOURCE) $(DEP_CPP_LOADT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_LOADT=\
	".\loadtransform.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\loadtransform.obj" : $(SOURCE) $(DEP_CPP_LOADT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_LOADT=\
	".\loadtransform.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\loadtransform.obj"	"$(INTDIR)\loadtransform.sbr" : $(SOURCE)\
 $(DEP_CPP_LOADT) "$(INTDIR)"


!ENDIF 

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_MAINF=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\GlobalProperties.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\PreviewPrefsPage.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\SCSIExplorer.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_MAINF=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\mainfrm.obj"	"$(INTDIR)\mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_MAINF=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\GlobalProperties.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\PreviewPrefsPage.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\SCSIExplorer.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_MAINF=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\GlobalProperties.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\PreviewPrefsPage.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\SCSIExplorer.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_MAINF=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_MAINF=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\GlobalProperties.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\PreviewPrefsPage.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\SCSIExplorer.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_MAINF=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_MAINF=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\GlobalProperties.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\PreviewPrefsPage.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\SCSIExplorer.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_MAINF=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_MAINF=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\DevListPage.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\GlobalProperties.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\PreviewPrefsPage.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\SCSIExplorer.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_MAINF=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\mainfrm.obj"	"$(INTDIR)\mainfrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\newformv.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_NEWFO=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_NEWFO=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\newformv.obj"	"$(INTDIR)\newformv.sbr" : $(SOURCE) $(DEP_CPP_NEWFO)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_NEWFO=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	

"$(INTDIR)\newformv.obj" : $(SOURCE) $(DEP_CPP_NEWFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_NEWFO=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_NEWFO=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\newformv.obj" : $(SOURCE) $(DEP_CPP_NEWFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_NEWFO=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_NEWFO=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\newformv.obj" : $(SOURCE) $(DEP_CPP_NEWFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_NEWFO=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_NEWFO=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\newformv.obj" : $(SOURCE) $(DEP_CPP_NEWFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_NEWFO=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_NEWFO=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\newformv.obj"	"$(INTDIR)\newformv.sbr" : $(SOURCE) $(DEP_CPP_NEWFO)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\nformat.cpp
DEP_CPP_NFORM=\
	".\nformat.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\nformat.obj"	"$(INTDIR)\nformat.sbr" : $(SOURCE) $(DEP_CPP_NFORM)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\nformat.obj" : $(SOURCE) $(DEP_CPP_NFORM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\nformat.obj" : $(SOURCE) $(DEP_CPP_NFORM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\nformat.obj" : $(SOURCE) $(DEP_CPP_NFORM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\nformat.obj" : $(SOURCE) $(DEP_CPP_NFORM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\nformat.obj"	"$(INTDIR)\nformat.sbr" : $(SOURCE) $(DEP_CPP_NFORM)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\OurPropertySheet.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_OURPR=\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\OurPropertySheet.obj"	"$(INTDIR)\OurPropertySheet.sbr" : $(SOURCE)\
 $(DEP_CPP_OURPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_OURPR=\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\OurPropertySheet.obj" : $(SOURCE) $(DEP_CPP_OURPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_OURPR=\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\OurPropertySheet.obj" : $(SOURCE) $(DEP_CPP_OURPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_OURPR=\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\OurPropertySheet.obj" : $(SOURCE) $(DEP_CPP_OURPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_OURPR=\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\OurPropertySheet.obj" : $(SOURCE) $(DEP_CPP_OURPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_OURPR=\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\OurPropertySheet.obj"	"$(INTDIR)\OurPropertySheet.sbr" : $(SOURCE)\
 $(DEP_CPP_OURPR) "$(INTDIR)"


!ENDIF 

SOURCE=.\plist.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_PLIST=\
	".\plist.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\plist.obj"	"$(INTDIR)\plist.sbr" : $(SOURCE) $(DEP_CPP_PLIST)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_PLIST=\
	".\plist.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\plist.obj" : $(SOURCE) $(DEP_CPP_PLIST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_PLIST=\
	".\plist.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\plist.obj" : $(SOURCE) $(DEP_CPP_PLIST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_PLIST=\
	".\plist.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\plist.obj" : $(SOURCE) $(DEP_CPP_PLIST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_PLIST=\
	".\plist.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\plist.obj" : $(SOURCE) $(DEP_CPP_PLIST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_PLIST=\
	".\plist.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\plist.obj"	"$(INTDIR)\plist.sbr" : $(SOURCE) $(DEP_CPP_PLIST)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\PREVIEW.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_PREVI=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\devface.h"\
	".\device.h"\
	".\Disp.h"\
	".\ImagePause.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PREVIEW.OBJ"	"$(INTDIR)\PREVIEW.SBR" : $(SOURCE) $(DEP_CPP_PREVI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_PREVI=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\devface.h"\
	".\device.h"\
	".\Disp.h"\
	".\ImagePause.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PREVIEW.OBJ" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_PREVI=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\devface.h"\
	".\device.h"\
	".\Disp.h"\
	".\ImagePause.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PREVIEW.OBJ" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_PREVI=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\devface.h"\
	".\device.h"\
	".\Disp.h"\
	".\ImagePause.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PREVIEW.OBJ" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_PREVI=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\devface.h"\
	".\device.h"\
	".\Disp.h"\
	".\ImagePause.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PREVIEW.OBJ" : $(SOURCE) $(DEP_CPP_PREVI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_PREVI=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\devface.h"\
	".\device.h"\
	".\Disp.h"\
	".\ImagePause.h"\
	".\Inidefs.h"\
	".\Mainfrm.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\profile.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PREVIEW.OBJ"	"$(INTDIR)\PREVIEW.SBR" : $(SOURCE) $(DEP_CPP_PREVI)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\PreviewPrefsPage.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_PREVIE=\
	".\control.hm"\
	".\Inidefs.h"\
	".\PreviewPrefsPage.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PreviewPrefsPage.obj"	"$(INTDIR)\PreviewPrefsPage.sbr" : $(SOURCE)\
 $(DEP_CPP_PREVIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_PREVIE=\
	".\control.hm"\
	".\Inidefs.h"\
	".\PreviewPrefsPage.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PreviewPrefsPage.obj" : $(SOURCE) $(DEP_CPP_PREVIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_PREVIE=\
	".\control.hm"\
	".\Inidefs.h"\
	".\PreviewPrefsPage.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PreviewPrefsPage.obj" : $(SOURCE) $(DEP_CPP_PREVIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_PREVIE=\
	".\control.hm"\
	".\Inidefs.h"\
	".\PreviewPrefsPage.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PreviewPrefsPage.obj" : $(SOURCE) $(DEP_CPP_PREVIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_PREVIE=\
	".\control.hm"\
	".\Inidefs.h"\
	".\PreviewPrefsPage.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PreviewPrefsPage.obj" : $(SOURCE) $(DEP_CPP_PREVIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_PREVIE=\
	".\control.hm"\
	".\Inidefs.h"\
	".\PreviewPrefsPage.h"\
	".\profile.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\PreviewPrefsPage.obj"	"$(INTDIR)\PreviewPrefsPage.sbr" : $(SOURCE)\
 $(DEP_CPP_PREVIE) "$(INTDIR)"


!ENDIF 

SOURCE=.\PrinterNameChange.cpp
DEP_CPP_PRINT=\
	".\PrinterNameChange.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\PrinterNameChange.obj"	"$(INTDIR)\PrinterNameChange.sbr" : $(SOURCE)\
 $(DEP_CPP_PRINT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\PrinterNameChange.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\PrinterNameChange.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\PrinterNameChange.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\PrinterNameChange.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\PrinterNameChange.obj"	"$(INTDIR)\PrinterNameChange.sbr" : $(SOURCE)\
 $(DEP_CPP_PRINT) "$(INTDIR)"


!ENDIF 

SOURCE=.\ProcessManager.cpp
DEP_CPP_PROCE=\
	".\ipcdata.h"\
	".\ProcessManager.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\ProcessManager.obj"	"$(INTDIR)\ProcessManager.sbr" : $(SOURCE)\
 $(DEP_CPP_PROCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\ProcessManager.obj" : $(SOURCE) $(DEP_CPP_PROCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\ProcessManager.obj" : $(SOURCE) $(DEP_CPP_PROCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\ProcessManager.obj" : $(SOURCE) $(DEP_CPP_PROCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\ProcessManager.obj" : $(SOURCE) $(DEP_CPP_PROCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\ProcessManager.obj"	"$(INTDIR)\ProcessManager.sbr" : $(SOURCE)\
 $(DEP_CPP_PROCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\profile.cpp
DEP_CPP_PROFI=\
	".\profile.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\profile.obj"	"$(INTDIR)\profile.sbr" : $(SOURCE) $(DEP_CPP_PROFI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\profile.obj" : $(SOURCE) $(DEP_CPP_PROFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\profile.obj" : $(SOURCE) $(DEP_CPP_PROFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\profile.obj" : $(SOURCE) $(DEP_CPP_PROFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\profile.obj" : $(SOURCE) $(DEP_CPP_PROFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\profile.obj"	"$(INTDIR)\profile.sbr" : $(SOURCE) $(DEP_CPP_PROFI)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\qprefs.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_QPREF=\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\qprefs.obj"	"$(INTDIR)\qprefs.sbr" : $(SOURCE) $(DEP_CPP_QPREF)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_QPREF=\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\qprefs.obj" : $(SOURCE) $(DEP_CPP_QPREF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_QPREF=\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\qprefs.obj" : $(SOURCE) $(DEP_CPP_QPREF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_QPREF=\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\qprefs.obj" : $(SOURCE) $(DEP_CPP_QPREF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_QPREF=\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\qprefs.obj" : $(SOURCE) $(DEP_CPP_QPREF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_QPREF=\
	".\Cqueue.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\OurPropertySheet.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\qprefs.obj"	"$(INTDIR)\qprefs.sbr" : $(SOURCE) $(DEP_CPP_QPREF)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\QueueChild.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_QUEUE=\
	".\QueueChild.h"\
	".\Ripmsg.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\QueueChild.obj"	"$(INTDIR)\QueueChild.sbr" : $(SOURCE)\
 $(DEP_CPP_QUEUE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_QUEUE=\
	".\QueueChild.h"\
	".\Ripmsg.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\QueueChild.obj" : $(SOURCE) $(DEP_CPP_QUEUE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_QUEUE=\
	".\QueueChild.h"\
	".\Ripmsg.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\QueueChild.obj" : $(SOURCE) $(DEP_CPP_QUEUE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_QUEUE=\
	".\QueueChild.h"\
	".\Ripmsg.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\QueueChild.obj" : $(SOURCE) $(DEP_CPP_QUEUE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_QUEUE=\
	".\QueueChild.h"\
	".\Ripmsg.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\QueueChild.obj" : $(SOURCE) $(DEP_CPP_QUEUE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_QUEUE=\
	".\QueueChild.h"\
	".\Ripmsg.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\QueueChild.obj"	"$(INTDIR)\QueueChild.sbr" : $(SOURCE)\
 $(DEP_CPP_QUEUE) "$(INTDIR)"


!ENDIF 

SOURCE=.\report.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_REPOR=\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\plist.h"\
	".\report.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Transfrm.h"\
	

"$(INTDIR)\report.obj"	"$(INTDIR)\report.sbr" : $(SOURCE) $(DEP_CPP_REPOR)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_REPOR=\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\plist.h"\
	".\report.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Transfrm.h"\
	

"$(INTDIR)\report.obj" : $(SOURCE) $(DEP_CPP_REPOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_REPOR=\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\plist.h"\
	".\report.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Transfrm.h"\
	

"$(INTDIR)\report.obj" : $(SOURCE) $(DEP_CPP_REPOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_REPOR=\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\plist.h"\
	".\report.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Transfrm.h"\
	

"$(INTDIR)\report.obj" : $(SOURCE) $(DEP_CPP_REPOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_REPOR=\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\plist.h"\
	".\report.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Transfrm.h"\
	

"$(INTDIR)\report.obj" : $(SOURCE) $(DEP_CPP_REPOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_REPOR=\
	".\Cqueue.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\plist.h"\
	".\report.h"\
	".\Rip.h"\
	".\RipStruct.h"\
	".\Servdefs.h"\
	".\Transfrm.h"\
	

"$(INTDIR)\report.obj"	"$(INTDIR)\report.sbr" : $(SOURCE) $(DEP_CPP_REPOR)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\REXDIRDI.CPP
DEP_CPP_REXDI=\
	".\Rexdirdi.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\REXDIRDI.OBJ"	"$(INTDIR)\REXDIRDI.SBR" : $(SOURCE) $(DEP_CPP_REXDI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\REXDIRDI.OBJ" : $(SOURCE) $(DEP_CPP_REXDI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\REXDIRDI.OBJ" : $(SOURCE) $(DEP_CPP_REXDI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\REXDIRDI.OBJ" : $(SOURCE) $(DEP_CPP_REXDI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\REXDIRDI.OBJ" : $(SOURCE) $(DEP_CPP_REXDI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\REXDIRDI.OBJ"	"$(INTDIR)\REXDIRDI.SBR" : $(SOURCE) $(DEP_CPP_REXDI)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\ripitb.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_RIPIT=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitb.obj"	"$(INTDIR)\ripitb.sbr" : $(SOURCE) $(DEP_CPP_RIPIT)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_RIPIT=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitb.obj" : $(SOURCE) $(DEP_CPP_RIPIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_RIPIT=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitb.obj" : $(SOURCE) $(DEP_CPP_RIPIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_RIPIT=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitb.obj" : $(SOURCE) $(DEP_CPP_RIPIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_RIPIT=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitb.obj" : $(SOURCE) $(DEP_CPP_RIPIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_RIPIT=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitb.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitb.obj"	"$(INTDIR)\ripitb.sbr" : $(SOURCE) $(DEP_CPP_RIPIT)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\ripitj.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_RIPITJ=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitj.obj"	"$(INTDIR)\ripitj.sbr" : $(SOURCE) $(DEP_CPP_RIPITJ)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_RIPITJ=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitj.obj" : $(SOURCE) $(DEP_CPP_RIPITJ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_RIPITJ=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitj.obj" : $(SOURCE) $(DEP_CPP_RIPITJ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_RIPITJ=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitj.obj" : $(SOURCE) $(DEP_CPP_RIPITJ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_RIPITJ=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitj.obj" : $(SOURCE) $(DEP_CPP_RIPITJ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_RIPITJ=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\Rip.h"\
	".\ripitj.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripitj.obj"	"$(INTDIR)\ripitj.sbr" : $(SOURCE) $(DEP_CPP_RIPITJ)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\ripprocess.cpp
DEP_CPP_RIPPR=\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\ripprocess.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\ripprocess.obj"	"$(INTDIR)\ripprocess.sbr" : $(SOURCE)\
 $(DEP_CPP_RIPPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\ripprocess.obj" : $(SOURCE) $(DEP_CPP_RIPPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\ripprocess.obj" : $(SOURCE) $(DEP_CPP_RIPPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\ripprocess.obj" : $(SOURCE) $(DEP_CPP_RIPPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\ripprocess.obj" : $(SOURCE) $(DEP_CPP_RIPPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\ripprocess.obj"	"$(INTDIR)\ripprocess.sbr" : $(SOURCE)\
 $(DEP_CPP_RIPPR) "$(INTDIR)"


!ENDIF 

SOURCE=.\ripthrd.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_RIPTH=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripthrd.obj"	"$(INTDIR)\ripthrd.sbr" : $(SOURCE) $(DEP_CPP_RIPTH)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_RIPTH=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripthrd.obj" : $(SOURCE) $(DEP_CPP_RIPTH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_RIPTH=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripthrd.obj" : $(SOURCE) $(DEP_CPP_RIPTH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_RIPTH=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripthrd.obj" : $(SOURCE) $(DEP_CPP_RIPTH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_RIPTH=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripthrd.obj" : $(SOURCE) $(DEP_CPP_RIPTH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_RIPTH=\
	".\devface.h"\
	".\device.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ripthrd.obj"	"$(INTDIR)\ripthrd.sbr" : $(SOURCE) $(DEP_CPP_RIPTH)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\roundit.cpp
DEP_CPP_ROUND=\
	".\roundit.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\roundit.obj"	"$(INTDIR)\roundit.sbr" : $(SOURCE) $(DEP_CPP_ROUND)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\roundit.obj" : $(SOURCE) $(DEP_CPP_ROUND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\roundit.obj" : $(SOURCE) $(DEP_CPP_ROUND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\roundit.obj" : $(SOURCE) $(DEP_CPP_ROUND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\roundit.obj" : $(SOURCE) $(DEP_CPP_ROUND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\roundit.obj"	"$(INTDIR)\roundit.sbr" : $(SOURCE) $(DEP_CPP_ROUND)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\Ruler.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_RULER=\
	".\Ruler.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\Ruler.obj"	"$(INTDIR)\Ruler.sbr" : $(SOURCE) $(DEP_CPP_RULER)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_RULER=\
	".\Ruler.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\Ruler.obj" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_RULER=\
	".\Ruler.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\Ruler.obj" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_RULER=\
	".\Ruler.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\Ruler.obj" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_RULER=\
	".\Ruler.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\Ruler.obj" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_RULER=\
	".\Ruler.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\Ruler.obj"	"$(INTDIR)\Ruler.sbr" : $(SOURCE) $(DEP_CPP_RULER)\
 "$(INTDIR)"


!ENDIF 

SOURCE="..\Shared Source\scsi\Scsi.cpp"

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_SCSI_=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\stdafx.h"\
	"..\shared source\scsi\wnaspi32.h"\
	

"$(INTDIR)\Scsi.obj"	"$(INTDIR)\Scsi.sbr" : $(SOURCE) $(DEP_CPP_SCSI_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_SCSI_=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\stdafx.h"\
	"..\shared source\scsi\wnaspi32.h"\
	

"$(INTDIR)\Scsi.obj" : $(SOURCE) $(DEP_CPP_SCSI_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_SCSI_=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\stdafx.h"\
	"..\shared source\scsi\wnaspi32.h"\
	

"$(INTDIR)\Scsi.obj" : $(SOURCE) $(DEP_CPP_SCSI_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_SCSI_=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\stdafx.h"\
	"..\shared source\scsi\wnaspi32.h"\
	

"$(INTDIR)\Scsi.obj" : $(SOURCE) $(DEP_CPP_SCSI_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_SCSI_=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\stdafx.h"\
	"..\shared source\scsi\wnaspi32.h"\
	

"$(INTDIR)\Scsi.obj" : $(SOURCE) $(DEP_CPP_SCSI_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_SCSI_=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\stdafx.h"\
	"..\shared source\scsi\wnaspi32.h"\
	

"$(INTDIR)\Scsi.obj"	"$(INTDIR)\Scsi.sbr" : $(SOURCE) $(DEP_CPP_SCSI_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\SCSIExplorer.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_SCSIE=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\SCSIExplorer.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SCSIExplorer.obj"	"$(INTDIR)\SCSIExplorer.sbr" : $(SOURCE)\
 $(DEP_CPP_SCSIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_SCSIE=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\SCSIExplorer.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SCSIExplorer.obj" : $(SOURCE) $(DEP_CPP_SCSIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_SCSIE=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\SCSIExplorer.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SCSIExplorer.obj" : $(SOURCE) $(DEP_CPP_SCSIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_SCSIE=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\SCSIExplorer.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SCSIExplorer.obj" : $(SOURCE) $(DEP_CPP_SCSIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_SCSIE=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\SCSIExplorer.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SCSIExplorer.obj" : $(SOURCE) $(DEP_CPP_SCSIE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_SCSIE=\
	"..\shared source\scsi\scsi.h"\
	"..\shared source\scsi\scsidefs.h"\
	"..\shared source\scsi\wnaspi32.h"\
	".\SCSIExplorer.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SCSIExplorer.obj"	"$(INTDIR)\SCSIExplorer.sbr" : $(SOURCE)\
 $(DEP_CPP_SCSIE) "$(INTDIR)"


!ENDIF 

SOURCE=.\server.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_SERVE=\
	".\profile.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\server.obj"	"$(INTDIR)\server.sbr" : $(SOURCE) $(DEP_CPP_SERVE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_SERVE=\
	".\profile.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\server.obj" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_SERVE=\
	".\profile.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\server.obj" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_SERVE=\
	".\profile.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\server.obj" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_SERVE=\
	".\profile.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\server.obj" : $(SOURCE) $(DEP_CPP_SERVE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_SERVE=\
	".\profile.h"\
	".\server.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\server.obj"	"$(INTDIR)\server.sbr" : $(SOURCE) $(DEP_CPP_SERVE)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\ServerCfg.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_SERVER=\
	".\control.hm"\
	".\profile.h"\
	".\Rexdirdi.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ServerCfg.obj"	"$(INTDIR)\ServerCfg.sbr" : $(SOURCE)\
 $(DEP_CPP_SERVER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_SERVER=\
	".\control.hm"\
	".\profile.h"\
	".\Rexdirdi.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ServerCfg.obj" : $(SOURCE) $(DEP_CPP_SERVER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_SERVER=\
	".\control.hm"\
	".\profile.h"\
	".\Rexdirdi.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ServerCfg.obj" : $(SOURCE) $(DEP_CPP_SERVER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_SERVER=\
	".\control.hm"\
	".\profile.h"\
	".\Rexdirdi.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ServerCfg.obj" : $(SOURCE) $(DEP_CPP_SERVER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_SERVER=\
	".\control.hm"\
	".\profile.h"\
	".\Rexdirdi.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ServerCfg.obj" : $(SOURCE) $(DEP_CPP_SERVER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_SERVER=\
	".\control.hm"\
	".\profile.h"\
	".\Rexdirdi.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ServerCfg.obj"	"$(INTDIR)\ServerCfg.sbr" : $(SOURCE)\
 $(DEP_CPP_SERVER) "$(INTDIR)"


!ENDIF 

SOURCE=.\ShellAssociate.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_SHELL=\
	".\profile.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ShellAssociate.obj"	"$(INTDIR)\ShellAssociate.sbr" : $(SOURCE)\
 $(DEP_CPP_SHELL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_SHELL=\
	".\profile.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ShellAssociate.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_SHELL=\
	".\profile.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ShellAssociate.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_SHELL=\
	".\profile.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ShellAssociate.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_SHELL=\
	".\profile.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ShellAssociate.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_SHELL=\
	".\profile.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Zeus.h"\
	

"$(INTDIR)\ShellAssociate.obj"	"$(INTDIR)\ShellAssociate.sbr" : $(SOURCE)\
 $(DEP_CPP_SHELL) "$(INTDIR)"


!ENDIF 

SOURCE=.\SPLITDLG.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_SPLIT=\
	".\profile.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Winsplit.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SPLITDLG.OBJ"	"$(INTDIR)\SPLITDLG.SBR" : $(SOURCE) $(DEP_CPP_SPLIT)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_SPLIT=\
	".\profile.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Winsplit.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SPLITDLG.OBJ" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_SPLIT=\
	".\profile.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Winsplit.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SPLITDLG.OBJ" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_SPLIT=\
	".\profile.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Winsplit.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SPLITDLG.OBJ" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_SPLIT=\
	".\profile.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Winsplit.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SPLITDLG.OBJ" : $(SOURCE) $(DEP_CPP_SPLIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_SPLIT=\
	".\profile.h"\
	".\Splitdlg.h"\
	".\Stdafx.h"\
	".\Winsplit.h"\
	".\Zeus.h"\
	

"$(INTDIR)\SPLITDLG.OBJ"	"$(INTDIR)\SPLITDLG.SBR" : $(SOURCE) $(DEP_CPP_SPLIT)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\STDAFX.CPP
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\STDAFX.OBJ"	"$(INTDIR)\STDAFX.SBR" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\STDAFX.OBJ"	"$(INTDIR)\STDAFX.SBR" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\SYSMEM.CPP
DEP_CPP_SYSME=\
	".\nformat.h"\
	".\Sysmem.h"\
	

!IF  "$(CFG)" == "zeus32 - Win32 Debug"


"$(INTDIR)\SYSMEM.OBJ"	"$(INTDIR)\SYSMEM.SBR" : $(SOURCE) $(DEP_CPP_SYSME)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"


"$(INTDIR)\SYSMEM.OBJ" : $(SOURCE) $(DEP_CPP_SYSME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"


"$(INTDIR)\SYSMEM.OBJ" : $(SOURCE) $(DEP_CPP_SYSME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"


"$(INTDIR)\SYSMEM.OBJ" : $(SOURCE) $(DEP_CPP_SYSME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"


"$(INTDIR)\SYSMEM.OBJ" : $(SOURCE) $(DEP_CPP_SYSME) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"


"$(INTDIR)\SYSMEM.OBJ"	"$(INTDIR)\SYSMEM.SBR" : $(SOURCE) $(DEP_CPP_SYSME)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\TRANSDLG.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_TRANS=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSDLG.OBJ"	"$(INTDIR)\TRANSDLG.SBR" : $(SOURCE) $(DEP_CPP_TRANS)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_TRANS=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSDLG.OBJ" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_TRANS=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSDLG.OBJ" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_TRANS=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSDLG.OBJ" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_TRANS=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSDLG.OBJ" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_TRANS=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\Prefsmsg.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSDLG.OBJ"	"$(INTDIR)\TRANSDLG.SBR" : $(SOURCE) $(DEP_CPP_TRANS)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\transformlist.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_TRANSF=\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\transformlist.obj"	"$(INTDIR)\transformlist.sbr" : $(SOURCE)\
 $(DEP_CPP_TRANSF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_TRANSF=\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\transformlist.obj" : $(SOURCE) $(DEP_CPP_TRANSF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_TRANSF=\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\transformlist.obj" : $(SOURCE) $(DEP_CPP_TRANSF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_TRANSF=\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\transformlist.obj" : $(SOURCE) $(DEP_CPP_TRANSF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_TRANSF=\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\transformlist.obj" : $(SOURCE) $(DEP_CPP_TRANSF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_TRANSF=\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\transformlist.obj"	"$(INTDIR)\transformlist.sbr" : $(SOURCE)\
 $(DEP_CPP_TRANSF) "$(INTDIR)"


!ENDIF 

SOURCE=.\TransformMgrPage.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_TRANSFO=\
	".\control.hm"\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TransformMgrPage.obj"	"$(INTDIR)\TransformMgrPage.sbr" : $(SOURCE)\
 $(DEP_CPP_TRANSFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_TRANSFO=\
	".\control.hm"\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TransformMgrPage.obj" : $(SOURCE) $(DEP_CPP_TRANSFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_TRANSFO=\
	".\control.hm"\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TransformMgrPage.obj" : $(SOURCE) $(DEP_CPP_TRANSFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_TRANSFO=\
	".\control.hm"\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TransformMgrPage.obj" : $(SOURCE) $(DEP_CPP_TRANSFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_TRANSFO=\
	".\control.hm"\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TransformMgrPage.obj" : $(SOURCE) $(DEP_CPP_TRANSFO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_TRANSFO=\
	".\control.hm"\
	".\profile.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\TransformMgrPage.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TransformMgrPage.obj"	"$(INTDIR)\TransformMgrPage.sbr" : $(SOURCE)\
 $(DEP_CPP_TRANSFO) "$(INTDIR)"


!ENDIF 

SOURCE=.\TRANSFRM.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_TRANSFR=\
	".\profile.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSFRM.OBJ"	"$(INTDIR)\TRANSFRM.SBR" : $(SOURCE)\
 $(DEP_CPP_TRANSFR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_TRANSFR=\
	".\profile.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSFRM.OBJ" : $(SOURCE) $(DEP_CPP_TRANSFR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_TRANSFR=\
	".\profile.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSFRM.OBJ" : $(SOURCE) $(DEP_CPP_TRANSFR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_TRANSFR=\
	".\profile.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSFRM.OBJ" : $(SOURCE) $(DEP_CPP_TRANSFR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_TRANSFR=\
	".\profile.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSFRM.OBJ" : $(SOURCE) $(DEP_CPP_TRANSFR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_TRANSFR=\
	".\profile.h"\
	".\Stdafx.h"\
	".\Transfrm.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\TRANSFRM.OBJ"	"$(INTDIR)\TRANSFRM.SBR" : $(SOURCE)\
 $(DEP_CPP_TRANSFR) "$(INTDIR)"


!ENDIF 

SOURCE=.\visualxf.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_VISUA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\Ctm2rect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\loadtransform.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\XformSave.h"\
	".\Zeus.h"\
	

"$(INTDIR)\visualxf.obj"	"$(INTDIR)\visualxf.sbr" : $(SOURCE) $(DEP_CPP_VISUA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_VISUA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\Ctm2rect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\loadtransform.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\XformSave.h"\
	".\Zeus.h"\
	

"$(INTDIR)\visualxf.obj" : $(SOURCE) $(DEP_CPP_VISUA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_VISUA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\Ctm2rect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\loadtransform.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\XformSave.h"\
	".\Zeus.h"\
	

"$(INTDIR)\visualxf.obj" : $(SOURCE) $(DEP_CPP_VISUA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_VISUA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\Ctm2rect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\loadtransform.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\XformSave.h"\
	".\Zeus.h"\
	

"$(INTDIR)\visualxf.obj" : $(SOURCE) $(DEP_CPP_VISUA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_VISUA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\Ctm2rect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\loadtransform.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\XformSave.h"\
	".\Zeus.h"\
	

"$(INTDIR)\visualxf.obj" : $(SOURCE) $(DEP_CPP_VISUA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_VISUA=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Canvas.h"\
	".\control.hm"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\Ctm2rect.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\Frect.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\loadtransform.h"\
	".\Prev.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\roundit.h"\
	".\Ruler.h"\
	".\Servdefs.h"\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\visualxf.h"\
	".\XformSave.h"\
	".\Zeus.h"\
	

"$(INTDIR)\visualxf.obj"	"$(INTDIR)\visualxf.sbr" : $(SOURCE) $(DEP_CPP_VISUA)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\WINSPLIT.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_WINSP=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	".\Winsplit.h"\
	

"$(INTDIR)\WINSPLIT.OBJ"	"$(INTDIR)\WINSPLIT.SBR" : $(SOURCE) $(DEP_CPP_WINSP)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_WINSP=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	".\Winsplit.h"\
	

"$(INTDIR)\WINSPLIT.OBJ" : $(SOURCE) $(DEP_CPP_WINSP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_WINSP=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	".\Winsplit.h"\
	

"$(INTDIR)\WINSPLIT.OBJ" : $(SOURCE) $(DEP_CPP_WINSP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_WINSP=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	".\Winsplit.h"\
	

"$(INTDIR)\WINSPLIT.OBJ" : $(SOURCE) $(DEP_CPP_WINSP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_WINSP=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	".\Winsplit.h"\
	

"$(INTDIR)\WINSPLIT.OBJ" : $(SOURCE) $(DEP_CPP_WINSP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_WINSP=\
	".\Catdef.h"\
	".\Decomp.h"\
	".\Header.dat"\
	".\Winsplit.h"\
	

"$(INTDIR)\WINSPLIT.OBJ"	"$(INTDIR)\WINSPLIT.SBR" : $(SOURCE) $(DEP_CPP_WINSP)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\XformSave.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_XFORM=\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\XformSave.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\XformSave.obj"	"$(INTDIR)\XformSave.sbr" : $(SOURCE)\
 $(DEP_CPP_XFORM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_XFORM=\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\XformSave.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\XformSave.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_XFORM=\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\XformSave.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\XformSave.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_XFORM=\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\XformSave.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\XformSave.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_XFORM=\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\XformSave.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\XformSave.obj" : $(SOURCE) $(DEP_CPP_XFORM) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_XFORM=\
	".\Stdafx.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\XformSave.h"\
	".\Xfrmdefs.h"\
	".\Zeus.h"\
	

"$(INTDIR)\XformSave.obj"	"$(INTDIR)\XformSave.sbr" : $(SOURCE)\
 $(DEP_CPP_XFORM) "$(INTDIR)"


!ENDIF 

SOURCE=.\ZEUS.CPP

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_ZEUS_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUS_=\
	"..\dongles\DongleClass.h"\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\ZEUS.OBJ"	"$(INTDIR)\ZEUS.SBR" : $(SOURCE) $(DEP_CPP_ZEUS_)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_ZEUS_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	

"$(INTDIR)\ZEUS.OBJ" : $(SOURCE) $(DEP_CPP_ZEUS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_ZEUS_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUS_=\
	"..\dongles\DongleClass.h"\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\ZEUS.OBJ" : $(SOURCE) $(DEP_CPP_ZEUS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_ZEUS_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUS_=\
	"..\dongles\DongleClass.h"\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\ZEUS.OBJ" : $(SOURCE) $(DEP_CPP_ZEUS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_ZEUS_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUS_=\
	"..\dongles\DongleClass.h"\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\ZEUS.OBJ" : $(SOURCE) $(DEP_CPP_ZEUS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_ZEUS_=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\ShellAssociate.h"\
	".\Stdafx.h"\
	".\Sysmem.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUS_=\
	"..\dongles\DongleClass.h"\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\ZEUS.OBJ"	"$(INTDIR)\ZEUS.SBR" : $(SOURCE) $(DEP_CPP_ZEUS_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\zeus.rc

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_RSC_ZEUS_R=\
	".\Res\cursor1.cur"\
	".\Res\Ico00001.ico"\
	".\Res\Ico00002.ico"\
	".\Res\ico00003.ico"\
	".\Res\Ico00004.ico"\
	".\Res\Ico00005.ico"\
	".\Res\ico00006.ico"\
	".\Res\ico00007.ico"\
	".\Res\ico00008.ico"\
	".\Res\Ico00009.ico"\
	".\Res\Icon6.ico"\
	".\Res\Icon_fil.ico"\
	".\Res\Icon_pre.ico"\
	".\Res\Idr_zeus.ico"\
	".\Res\Landscap.ico"\
	".\Res\Portrait.ico"\
	".\Res\Status_o.ico"\
	".\Res\status_p.ico"\
	".\Res\toolbar1.bmp"\
	".\res\zeus.rc2"\
	".\rpsp_16.bmp"\
	".\Rpsplash.bmp"\
	

"$(INTDIR)\zeus.res" : $(SOURCE) $(DEP_RSC_ZEUS_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_RSC_ZEUS_R=\
	".\Res\cursor1.cur"\
	".\Res\Ico00001.ico"\
	".\Res\Ico00002.ico"\
	".\Res\ico00003.ico"\
	".\Res\Ico00004.ico"\
	".\Res\Ico00005.ico"\
	".\Res\ico00006.ico"\
	".\Res\ico00007.ico"\
	".\Res\ico00008.ico"\
	".\Res\Ico00009.ico"\
	".\Res\Icon6.ico"\
	".\Res\Icon_fil.ico"\
	".\Res\Icon_pre.ico"\
	".\Res\Idr_zeus.ico"\
	".\Res\Landscap.ico"\
	".\Res\Portrait.ico"\
	".\Res\Status_o.ico"\
	".\Res\status_p.ico"\
	".\Res\toolbar1.bmp"\
	".\res\zeus.rc2"\
	".\rpsp_16.bmp"\
	".\Rpsplash.bmp"\
	

"$(INTDIR)\zeus.res" : $(SOURCE) $(DEP_RSC_ZEUS_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_RSC_ZEUS_R=\
	".\Res\cursor1.cur"\
	".\Res\Ico00001.ico"\
	".\Res\Ico00002.ico"\
	".\Res\ico00003.ico"\
	".\Res\Ico00004.ico"\
	".\Res\Ico00005.ico"\
	".\Res\ico00006.ico"\
	".\Res\ico00007.ico"\
	".\Res\ico00008.ico"\
	".\Res\Ico00009.ico"\
	".\Res\Icon6.ico"\
	".\Res\Icon_fil.ico"\
	".\Res\Icon_pre.ico"\
	".\Res\Idr_zeus.ico"\
	".\Res\Landscap.ico"\
	".\Res\Portrait.ico"\
	".\Res\Status_o.ico"\
	".\Res\status_p.ico"\
	".\Res\toolbar1.bmp"\
	".\res\zeus.rc2"\
	".\rpsp_16.bmp"\
	".\Rpsplash.bmp"\
	

"$(INTDIR)\zeus.res" : $(SOURCE) $(DEP_RSC_ZEUS_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_RSC_ZEUS_R=\
	".\Res\cursor1.cur"\
	".\Res\Ico00001.ico"\
	".\Res\Ico00002.ico"\
	".\Res\ico00003.ico"\
	".\Res\Ico00004.ico"\
	".\Res\Ico00005.ico"\
	".\Res\ico00006.ico"\
	".\Res\ico00007.ico"\
	".\Res\ico00008.ico"\
	".\Res\Ico00009.ico"\
	".\Res\Icon6.ico"\
	".\Res\Icon_fil.ico"\
	".\Res\Icon_pre.ico"\
	".\Res\Idr_zeus.ico"\
	".\Res\Landscap.ico"\
	".\Res\Portrait.ico"\
	".\Res\Status_o.ico"\
	".\Res\status_p.ico"\
	".\Res\toolbar1.bmp"\
	".\res\zeus.rc2"\
	".\rpsp_16.bmp"\
	".\Rpsplash.bmp"\
	

"$(INTDIR)\zeus.res" : $(SOURCE) $(DEP_RSC_ZEUS_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

!ENDIF 

SOURCE=.\zeusdoc.cpp

!IF  "$(CFG)" == "zeus32 - Win32 Debug"

DEP_CPP_ZEUSD=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUSD=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\zeusdoc.obj"	"$(INTDIR)\zeusdoc.sbr" : $(SOURCE) $(DEP_CPP_ZEUSD)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release"

DEP_CPP_ZEUSD=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	

"$(INTDIR)\zeusdoc.obj" : $(SOURCE) $(DEP_CPP_ZEUSD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Kanji"

DEP_CPP_ZEUSD=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUSD=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\zeusdoc.obj" : $(SOURCE) $(DEP_CPP_ZEUSD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Fastcall Release Dongle"

DEP_CPP_ZEUSD=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUSD=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\zeusdoc.obj" : $(SOURCE) $(DEP_CPP_ZEUSD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Demo"

DEP_CPP_ZEUSD=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUSD=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\zeusdoc.obj" : $(SOURCE) $(DEP_CPP_ZEUSD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "zeus32 - Win32 Debug Kanji"

DEP_CPP_ZEUSD=\
	"..\lead80\include\l_bitmap.h"\
	"..\lead80\include\l_error.h"\
	"..\lead80\include\ltann.h"\
	"..\lead80\include\ltefx.h"\
	"..\lead80\include\lterr.h"\
	"..\lead80\include\ltfil.h"\
	"..\lead80\include\ltimg.h"\
	"..\lead80\include\ltkrn.h"\
	"..\lead80\include\ltlck.h"\
	"..\lead80\include\ltpck.h"\
	"..\lead80\include\ltsys.h"\
	"..\lead80\include\lttwn.h"\
	"..\lead80\include\lttyp.h"\
	"..\lead80\include\ltwnd.h"\
	".\Bheader.h"\
	".\Cqueue.h"\
	".\Crip.h"\
	".\devdlg.h"\
	".\devface.h"\
	".\device.h"\
	".\DeviceList.h"\
	".\Disp.h"\
	".\fileinfo.h"\
	".\Ftypes.h"\
	".\Inidefs.h"\
	".\ipcdata.h"\
	".\ipcripdata.h"\
	".\Mainfrm.h"\
	".\newformv.h"\
	".\OurPropertySheet.h"\
	".\plist.h"\
	".\Prev.h"\
	".\Preview.h"\
	".\ProcessManager.h"\
	".\profile.h"\
	".\qprefs.h"\
	".\QueueChild.h"\
	".\report.h"\
	".\Rip.h"\
	".\Ripmsg.h"\
	".\ripprocess.h"\
	".\RipStruct.h"\
	".\ripthrd.h"\
	".\Servdefs.h"\
	".\server.h"\
	".\ServerCfg.h"\
	".\Stdafx.h"\
	".\Transdlg.h"\
	".\transformlist.h"\
	".\Transfrm.h"\
	".\Zeus.h"\
	".\zeusdoc.h"\
	
NODEP_CPP_ZEUSD=\
	"h:\kat\dongles\DongleClass.h"\
	

"$(INTDIR)\zeusdoc.obj"	"$(INTDIR)\zeusdoc.sbr" : $(SOURCE) $(DEP_CPP_ZEUSD)\
 "$(INTDIR)"


!ENDIF 


!ENDIF 

