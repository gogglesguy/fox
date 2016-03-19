# Microsoft Developer Studio Project File - Name="foxdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FOXDLL - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "foxdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "foxdll.mak" CFG="FOXDLL - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "foxdll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foxdll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "foxdll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FOXDLL_EXPORTS" /YX /FD /c
# ADD CPP /MD /W3 /GR /GX /Ox /Og /Oi /Os /Gf /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_USRDLL" /D "FOXDLL" /D "FOXDLL_EXPORTS" /D "HAVE_GL_H" /D "HAVE_GLU_H" /FD /c
# SUBTRACT CPP /nologo /Gy /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib opengl32.lib glu32.lib comctl32.lib winspool.lib wsock32.lib imm32.lib /nologo /dll /pdb:none /machine:I386 /out:"..\..\..\lib\FOXDLL-1.7.dll"

!ELSEIF  "$(CFG)" == "foxdll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FOXDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /MDd /W3 /GR /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "_USRDLL" /D "FOXDLL" /D "FOXDLL_EXPORTS" /D "HAVE_GL_H" /D "HAVE_GLU_H" /FD /GZ /c
# SUBTRACT CPP /nologo /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib opengl32.lib glu32.lib comctl32.lib winspool.lib wsock32.lib imm32.lib /nologo /dll /pdb:none /debug /machine:I386 /out:"..\..\..\lib\FOXDLLD-1.7.dll"

!ENDIF 

# Begin Target

# Name "foxdll - Win32 Release"
# Name "foxdll - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\lib\arrownext.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\arrowprev.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigapp.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigcdrom.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigcomputer.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigdesktop.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigdoc.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigfloppy.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigfloppy3.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigfloppy5.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigfolder.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bigfolderopen.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bignetdrive.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bignethood.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bookclr.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bookset.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cmymode.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\deleteicon.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dialmode.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dirupicon.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dockbottom.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dockflip.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dockfree.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dockleft.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dockright.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\docktop.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\entericon.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\erroricon.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\eyedrop.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fileaccept.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\filecancel.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\filecopy.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\filedelete.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\filehidden.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\filelink.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\filemove.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\filerename.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fileshown.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\foldernew.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\include\fx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\fx3d.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX4Splitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX4Splitter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX7Segment.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX7Segment.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX885910Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX885910Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX885911Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX885911Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX885913Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX885913Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX885914Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX885914Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX885915Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX885915Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX885916Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX885916Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX88591Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX88591Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX88592Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX88592Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX88593Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX88593Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX88594Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX88594Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX88595Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX88595Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX88596Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX88596Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX88597Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX88597Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX88598Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX88598Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FX88599Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FX88599Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXAccelTable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXAccelTable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXApp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXApp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXArray.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXArrowButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXArrowButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxascii.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\fxascii.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXAtomic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXAtomic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXAutoPtr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXAutoThreadStorageKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXAutoThreadStorageKey.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXBarrier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXBarrier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXBitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXBitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXBitmapFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXBitmapFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXBitmapView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXBitmapView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXBMPIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXBMPIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXBMPImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXBMPImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxbmpio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXBZFileStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXBZFileStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCalendar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCalendar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCalendarView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCalendarView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCheckButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCheckButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXChoiceBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXChoiceBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXColorBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXColorBar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXColorDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXColorDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXColorList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXColorList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXColorRing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXColorRing.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXColors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXColors.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXColorSelector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXColorSelector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXColorWell.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXColorWell.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXColorWheel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXColorWheel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXComboBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXComplexd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXComplexd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXComplexf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXComplexf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXComposeContext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXComposeContext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXComposite.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXComposite.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXConcurrent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXConcurrent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCondition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCondition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXConsole.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXConsole.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP1250Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP1250Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP1251Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP1251Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP1252Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP1252Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP1253Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP1253Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP1254Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP1254Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP1255Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP1255Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP1256Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP1256Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP1257Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP1257Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP1258Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP1258Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP437Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP437Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP850Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP850Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP852Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP852Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP855Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP855Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP856Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP856Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP857Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP857Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP860Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP860Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP861Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP861Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP862Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP862Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP863Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP863Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP864Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP864Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP865Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP865Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP866Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP866Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP869Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP869Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCP874Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCP874Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxcpuid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\fxcpuid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCURCursor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCURCursor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXCursor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXCursor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDataTarget.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDataTarget.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDC.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDCPrint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDCPrint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDCWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDCWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDDSIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDDSIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDDSImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDDSImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxddsio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDebugTarget.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDebugTarget.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\fxdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDelegator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDelegator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDial.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDialogBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDialogBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDict.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDict.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDir.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDirBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDirBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDirDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDirDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDirList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDirList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDirSelector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDirSelector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDirVisitor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDirVisitor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDLL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDLL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDockBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDockBar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDockHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDockHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDockSite.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDockSite.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDockTitle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDockTitle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDocument.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDragCorner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDragCorner.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDrawable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDrawable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXDriveBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXDriveBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXElement.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\fxendian.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXException.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXExpression.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXExpression.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXExtentd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXExtentd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXExtentf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXExtentf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxezquantize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFileDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFileDict.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFileDict.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFileList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFileList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFileSelector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFileSelector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFileStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFileStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFoldingList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFoldingList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFontDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFontDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFontSelector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFontSelector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxfsquantize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGauge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGauge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxgetticks.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGIFCursor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGIFCursor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGIFIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGIFIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGIFImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGIFImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxgifio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLCanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLCanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLCone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLCone.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLContext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLContext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLCube.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLCube.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLCylinder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLShape.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLShape.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLSphere.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLTriangleMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLTriangleMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLViewer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLViewer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGLVisual.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGLVisual.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGradientBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGradientBar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGroupBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGroupBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXGZFileStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXGZFileStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXhalf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXhalf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXHash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXHash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXHeader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXHeader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXHorizontalFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXHorizontalFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXICOIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXICOIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXICOImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXICOImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxicoio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXIconDict.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXIconDict.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXIconList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXIconList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXIconSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXIconSource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXId.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXId.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXIFFIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXIFFIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXIFFImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXIFFImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxiffio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXImageFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXImageFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXImageView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXImageView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXInputDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXInputDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXIO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXIO.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXIODevice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXIODevice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXJP2Icon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXJP2Icon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXJP2Image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXJP2Image.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxjp2io.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxjpegio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXJPGIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXJPGIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXJPGImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXJPGImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxkeyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\fxkeys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxkeysym.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXKnob.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXKnob.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXKOI8RCodec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXKOI8RCodec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXLabel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXLabel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXListBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXListBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXLocale.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXLocale.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMainWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMainWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMat2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMat2d.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMat2f.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMat2f.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMat3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMat3d.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMat3f.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMat3f.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMat4d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMat4d.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMat4f.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMat4f.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMDIButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMDIButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMDIChild.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMDIChild.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMDIClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMDIClient.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMemMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMemMap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMemoryStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMemoryStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuBar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuCaption.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuCaption.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuCascade.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuCascade.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuCheck.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuCheck.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuCommand.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuPane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuPane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuRadio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuRadio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuSeparator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuSeparator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMenuTitle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMenuTitle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMessageBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMessageChannel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMessageChannel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMetaClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMetaClass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXMutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXMutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXObjectList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXObjectList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXOptionMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXOptionMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPacker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPacker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxparsegeometry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPCXIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPCXIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPCXImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPCXImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxpcxio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPicker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPicker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPipe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPipe.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPNGIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPNGIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPNGImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPNGImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxpngio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPopup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPopup.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPPMIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPPMIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPPMImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPPMImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxppmio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPrintDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPrintDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxprintf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxpriv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxpriv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXProcess.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXProcess.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXProgressBar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXProgressDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXProgressDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxpsio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPtrList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPtrList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXPtrQueue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXPtrQueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXQuatd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXQuatd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXQuatf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXQuatf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRadioButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRanged.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRanged.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRangef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRangef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRangeSlider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRangeSlider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRASIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRASIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRASImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRASImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxrasio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXReadWriteLock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXReadWriteLock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRealSlider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRealSlider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRealSpinner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRealSpinner.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRecentFiles.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRecentFiles.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRectangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRectangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRefPtr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRegion.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRegion.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRegistry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRegistry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXReplaceDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXReplaceDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRGBIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRGBIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRGBImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRGBImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxrgbio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRootWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRootWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRuler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRuler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXRulerView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRulerView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXRunnable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxscanf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXScrollArea.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXScrollArea.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXScrollBar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXScrollPane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXScrollPane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXScrollWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXScrollWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSearchDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSearchDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSemaphore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSemaphore.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSeparator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSeparator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSettings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXShell.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXShell.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXShutter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXShutter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSize.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSlider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSlider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSphered.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSphered.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSpheref.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSpheref.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSpinLock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSpinLock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSpinner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSpinner.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSplashWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSplashWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSplitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSplitter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSpring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSpring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXStat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXStat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXStatusBar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXStatusLine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXStatusLine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXStringDict.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXStringDict.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxstrtod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxstrtoll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxstrtoull.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSwitcher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSwitcher.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTabBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTabBar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTabBook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTabBook.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTabItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTabItem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxtargaio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXText.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXText.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTextCodec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTextCodec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTextField.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTextField.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTGAIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTGAIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTGAImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTGAImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXThread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXThreadPool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXThreadPool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTIFIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTIFIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTIFImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTIFImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxtifio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXToggleButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXToggleButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXToolBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXToolBar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXToolBarGrip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXToolBarGrip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXToolBarShell.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXToolBarShell.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXToolBarTab.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXToolBarTab.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXToolTip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXToolTip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTopWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTopWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTranslator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTranslator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTreeList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTreeList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTreeListBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTreeListBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXTriStateButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXTriStateButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXUndoList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXUndoList.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxunicode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\fxunicode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXURL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXURL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXUTF16Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXUTF16Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXUTF32Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXUTF32Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXUTF8Codec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXUTF8Codec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxutils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXVec2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXVec2d.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXVec2f.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXVec2f.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXVec3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXVec3d.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXVec3f.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXVec3f.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXVec4d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXVec4d.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXVec4f.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXVec4f.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\fxver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXVerticalFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXVerticalFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXVisual.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXVisual.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXWEBPIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXWEBPIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXWEBPImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXWEBPImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxwebpio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXWizard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXWizard.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXWorker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXWorker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxwuquantize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXXBMIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXXBMIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXXBMImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXXBMImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxxbmio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXXPMIcon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXXPMIcon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\FXXPMImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\include\FXXPMImage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\fxxpmio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\gotohome.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\gotoicon.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\gotowork.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\hsvmode.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\icons

!IF  "$(CFG)" == "foxdll - Win32 Release"

!ELSEIF  "$(CFG)" == "foxdll - Win32 Debug"

# Begin Custom Build
WkspDir=.
InputPath=..\..\..\lib\icons

BuildCmds= \
	cd ..\..\..\lib \
	$(WkspDir)\reswrap\Debug\reswrap -i icons.h -o icons.cpp arrownext.gif bookclr.gif fileaccept.gif landscape.gif minizipdrive.gif arrowprev.gif bookset.gif filecancel.gif listmode.gif portrait.gif bigapp.gif cmymode.gif filecopy.gif miniapp.gif questionicon.gif bigcdrom.gif deleteicon.gif filedelete.gif minicdrom.gif rgbmode.gif bigcomputer.gif dialmode.gif filehidden.gif minicomputer.gif searchicon.gif bigdesktop.gif dirupicon.gif filelink.gif minidesktop.gif showbigicons.gif bigdoc.gif dockbottom.gif filemove.gif minidoc.gif showdetails.gif bigfloppy.gif dockflip.gif filerename.gif minifloppy.gif showsmallicons.gif bigfloppy3.gif dockfree.gif fileshown.gif minifloppy3.gif bigfloppy5.gif dockleft.gif foldernew.gif minifloppy5.gif warningicon.gif bigfolder.gif dockright.gif gotohome.gif minifolder.gif winclose.gif bigfolderopen.gif docktop.gif gotoicon.gif minifolderopen.gif winmaximize.gif bigharddisk.gif entericon.gif gotowork.gif miniharddisk.gif winminimize.gif bignetdrive.gif erroricon.gif hsvmode.gif mininetdrive.gif winrestore.gif bignethood.gif eyedrop.gif infoicon.gif mininethood.gif \
	$(WkspDir)\reswrap\Debug\reswrap -h -o icons.h arrownext.gif bookclr.gif fileaccept.gif landscape.gif minizipdrive.gif arrowprev.gif bookset.gif filecancel.gif listmode.gif portrait.gif bigapp.gif cmymode.gif filecopy.gif miniapp.gif questionicon.gif bigcdrom.gif deleteicon.gif filedelete.gif minicdrom.gif rgbmode.gif bigcomputer.gif dialmode.gif filehidden.gif minicomputer.gif searchicon.gif bigdesktop.gif dirupicon.gif filelink.gif minidesktop.gif showbigicons.gif bigdoc.gif dockbottom.gif filemove.gif minidoc.gif showdetails.gif bigfloppy.gif dockflip.gif filerename.gif minifloppy.gif showsmallicons.gif bigfloppy3.gif dockfree.gif fileshown.gif minifloppy3.gif bigfloppy5.gif dockleft.gif foldernew.gif minifloppy5.gif warningicon.gif bigfolder.gif dockright.gif gotohome.gif minifolder.gif winclose.gif bigfolderopen.gif docktop.gif gotoicon.gif minifolderopen.gif winmaximize.gif bigharddisk.gif entericon.gif gotowork.gif miniharddisk.gif winminimize.gif bignetdrive.gif erroricon.gif hsvmode.gif mininetdrive.gif winrestore.gif bignethood.gif eyedrop.gif infoicon.gif mininethood.gif \
	

"icons.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"icons.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\icons.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\icons.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\infoicon.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\jitter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\landscape.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\listmode.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\miniapp.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\minicdrom.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\minicomputer.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\minidesktop.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\minidoc.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\minifloppy.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\minifolder.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\minifolderopen.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\miniharddisk.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mininetdrive.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\mininethood.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\minizipdrive.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\portrait.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\questionicon.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\rgbmode.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\searchicon.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\showbigicons.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\showdetails.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\showsmallicons.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\undo_gif.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\version.rc
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\warningicon.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\winclose.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\winmaximize.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\winminimize.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\winrestore.gif
# End Source File
# Begin Source File

SOURCE=..\..\..\include\xincs.h
# End Source File
# End Target
# End Project
