@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by RP95.HPJ. >"help\rasterplus.hm"
echo. >>"help\rasterplus.hm"
echo // Commands (ID_* and IDM_*) >>"help\rasterplus.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"help\rasterplus.hm"
echo. >>"help\rasterplus.hm"
echo // Prompts (IDP_*) >>"help\rasterplus.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"help\rasterplus.hm"
echo. >>"help\rasterplus.hm"
echo // Resources (IDR_*) >>"help\rasterplus.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"help\rasterplus.hm"
echo. >>"help\rasterplus.hm"
echo // Dialogs (IDD_*) >>"help\rasterplus.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"help\rasterplus.hm"
echo. >>"help\rasterplus.hm"
echo // Frame Controls (IDW_*) >>"help\rasterplus.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"help\rasterplus.hm"
REM -- Make help for Project RP95
@echo off
echo Building Win32 Help files
start /wait hcrtf -x "help\rasterplus.hpj"
echo.
if exist WinDebug\nul copy "help\rasterplus.hlp" WinDebug
if exist WinDebug\nul copy "help\rasterplus.cnt" WinDebug
if exist fastcall\nul copy "help\rasterplus.hlp" fastcall
if exist fastcall\nul copy "help\rasterplus.cnt" fastcall
if exist Demo\nul copy "help\rasterplus.hlp" Demo
if exist Demo\nul copy "help\rasterplus.cnt" Demo
if exist rp_kanji\nul copy "help\rasterplus.hlp" rp_kanji
if exist rp_kanji\nul copy "help\rasterplus.cnt" rp_kanji
if exist rp_kanji_d\nul copy "help\rasterplus.hlp" rp_kanji_d
if exist rp_kanji_d\nul copy "help\rasterplus.cnt" rp_kanji_d
if exist dongle\nul copy "help\rasterplus.hlp" dongle
if exist dongle\nul copy "help\rasterplus.cnt" dongle
if exist DebugRP30\nul copy "help\rasterplus.hlp" DebugRP30
if exist DebugRP30\nul copy "help\rasterplus.cnt" DebugRP30
if exist ReleaseRP30\nul copy "help\rasterplus.hlp" ReleaseRP30
if exist ReleaseRP30\nul copy "help\rasterplus.cnt" ReleaseRP30
echo.