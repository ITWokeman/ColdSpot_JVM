# Microsoft Developer Studio Project File - Name="my jvm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=my jvm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ColdSpot_JVM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ColdSpot_JVM.mak" CFG="my jvm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "my jvm - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Gm /GX /ZI /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\..\bin\ColdSpot_JVM.exe" /pdbtype:sept
# Begin Target

# Name "my jvm - Win32 Debug"
# Begin Group "�����ִ����ϵͳ"

# PROP Default_Filter ""
# Begin Group "JVM�����ִ����"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\ClassLoadActuator.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\ClassLoadActuator.h
# End Source File
# End Group
# Begin Group ".CLASS�ļ�������"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\AccessFlagDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\AccessFlagDecoder.h
# End Source File
# Begin Source File

SOURCE=.\cpp\ClassFileDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\ClassFileDecoder.h
# End Source File
# End Group
# Begin Group ".CLASS���ļ�������"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\ClassLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\ClassLoader.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\�����ִ����ϵͳ˵��.txt"
# End Source File
# End Group
# Begin Group "�ֽ���ִ������"

# PROP Default_Filter ""
# Begin Group "JVMָ�"

# PROP Default_Filter ""
# Begin Group "�Ƚ�ָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\Comparisons.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\comparisons.h
# End Source File
# End Group
# Begin Group "����ָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\constants.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\constants.h
# End Source File
# End Group
# Begin Group "����ָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\control.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\control.h
# End Source File
# End Group
# Begin Group "ת��ָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\conversions.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\conversions.h
# End Source File
# End Group
# Begin Group "��չָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\extended.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\extended.h
# End Source File
# End Group
# Begin Group "����ָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\Loads.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\loads.h
# End Source File
# End Group
# Begin Group "��ѧ����ָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\math.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\math.h
# End Source File
# End Group
# Begin Group "����ָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\references.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\references.h
# End Source File
# End Group
# Begin Group "��ջָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\stack.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\stack.h
# End Source File
# End Group
# Begin Group "�洢ָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\stores.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\stores.h
# End Source File
# End Group
# Begin Group "����ָ�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\reserved.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\reserved.h
# End Source File
# End Group
# Begin Group "Hack������"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\HackMethod.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\HackMethod.h
# End Source File
# End Group
# End Group
# Begin Group "����ִ����"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\InstDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\InstDecoder.h
# End Source File
# Begin Source File

SOURCE=.\cpp\Interpreter.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\Interpreter.h
# End Source File
# End Group
# Begin Group "�߳�"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\headers\JVMThreadScheduler.h
# End Source File
# Begin Source File

SOURCE=.\headers\Thread.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\�ֽ���ִ������˵��.txt"
# End Source File
# End Group
# Begin Group "���ط�����"

# PROP Default_Filter ""
# Begin Group "java��"

# PROP Default_Filter ""
# Begin Group "java.lang��"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\java_lang.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\java_lang.h
# End Source File
# End Group
# Begin Group "java.util��"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\java_util.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\java_util.h
# End Source File
# End Group
# End Group
# Begin Group "sun��"

# PROP Default_Filter ""
# Begin Group "sun.misc��"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\sun_misc.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\sun_misc.h
# End Source File
# End Group
# Begin Group "sun.reflect��"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\sun_reflect.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\sun_reflect.h
# End Source File
# End Group
# Begin Group "sun.security��"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\sun_security.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\sun_security.h
# End Source File
# End Group
# End Group
# Begin Group "saltedfish��"

# PROP Default_Filter ""
# Begin Group "saltedfish.io��"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\saltedfish_io.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\saltedfish_io.h
# End Source File
# End Group
# End Group
# Begin Group "���ط���ע����"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\headers\registry.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\���ط�����˵��.txt"
# End Source File
# End Group
# Begin Group "����ʱ������"

# PROP Default_Filter ""
# Begin Group "����"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\Heap.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\Heap.h
# End Source File
# End Group
# Begin Group "������"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\methodArea.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\methodArea.h
# End Source File
# End Group
# Begin Group "����������"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\Recycler.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\Recycler.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\����ʱ������˵��.txt"
# End Source File
# End Group
# Begin Group "��������������"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\headers\UtilTypes.h
# End Source File
# Begin Source File

SOURCE=.\cpp\VMTools.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\VMTools.h
# End Source File
# End Group
# Begin Group "JVM���"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\JVM.CPP
# End Source File
# End Group
# Begin Group "��������"

# PROP Default_Filter ""
# Begin Group "ѹ���ļ���ȡ��"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib\zlib\ioapi.h
# End Source File
# Begin Source File

SOURCE=.\lib\zlib\unzip.h
# End Source File
# Begin Source File

SOURCE=.\lib\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=.\lib\zlib\zlib.h
# End Source File
# End Group
# Begin Source File

SOURCE="E:\VC++\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=.\lib\clib\PSAPI.H
# End Source File
# End Group
# Begin Group "JVMͼ��"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ico\jvm.ico
# End Source File
# Begin Source File

SOURCE=.\ico\rc.rc
# End Source File
# End Group
# Begin Group "UTF8������"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\UTF8CNDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\UTF8CNDecoder.h
# End Source File
# End Group
# Begin Group "����������ļ�������"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\ConfigDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\ConfigDecoder.h
# End Source File
# Begin Source File

SOURCE=".\explain\�����ļ�������˵��.txt"
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\JVMģ�����ͼ.TXT"
# End Source File
# Begin Source File

SOURCE=".\explain\JVM��Ϣ˵��.TXT"
# End Source File
# End Target
# End Project
