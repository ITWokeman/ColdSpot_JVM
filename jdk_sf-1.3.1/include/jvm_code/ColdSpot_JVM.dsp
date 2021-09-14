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
# Begin Group "类加载执行子系统"

# PROP Default_Filter ""
# Begin Group "JVM类加载执行器"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\ClassLoadActuator.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\ClassLoadActuator.h
# End Source File
# End Group
# Begin Group ".CLASS文件解析器"

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
# Begin Group ".CLASS类文件加载器"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\ClassLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\ClassLoader.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\类加载执行子系统说明.txt"
# End Source File
# End Group
# Begin Group "字节码执行引擎"

# PROP Default_Filter ""
# Begin Group "JVM指令集"

# PROP Default_Filter ""
# Begin Group "比较指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\Comparisons.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\comparisons.h
# End Source File
# End Group
# Begin Group "常量指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\constants.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\constants.h
# End Source File
# End Group
# Begin Group "控制指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\control.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\control.h
# End Source File
# End Group
# Begin Group "转换指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\conversions.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\conversions.h
# End Source File
# End Group
# Begin Group "扩展指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\extended.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\extended.h
# End Source File
# End Group
# Begin Group "加载指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\Loads.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\loads.h
# End Source File
# End Group
# Begin Group "数学运算指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\math.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\math.h
# End Source File
# End Group
# Begin Group "引用指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\references.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\references.h
# End Source File
# End Group
# Begin Group "堆栈指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\stack.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\stack.h
# End Source File
# End Group
# Begin Group "存储指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\stores.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\stores.h
# End Source File
# End Group
# Begin Group "保留指令集"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\reserved.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\reserved.h
# End Source File
# End Group
# Begin Group "Hack方法库"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\HackMethod.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\HackMethod.h
# End Source File
# End Group
# End Group
# Begin Group "解释执行器"

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
# Begin Group "线程"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\headers\JVMThreadScheduler.h
# End Source File
# Begin Source File

SOURCE=.\headers\Thread.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\字节码执行引擎说明.txt"
# End Source File
# End Group
# Begin Group "本地方法库"

# PROP Default_Filter ""
# Begin Group "java包"

# PROP Default_Filter ""
# Begin Group "java.lang包"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\java_lang.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\java_lang.h
# End Source File
# End Group
# Begin Group "java.util包"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\java_util.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\java_util.h
# End Source File
# End Group
# End Group
# Begin Group "sun包"

# PROP Default_Filter ""
# Begin Group "sun.misc包"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\sun_misc.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\sun_misc.h
# End Source File
# End Group
# Begin Group "sun.reflect包"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\sun_reflect.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\sun_reflect.h
# End Source File
# End Group
# Begin Group "sun.security包"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\sun_security.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\sun_security.h
# End Source File
# End Group
# End Group
# Begin Group "saltedfish包"

# PROP Default_Filter ""
# Begin Group "saltedfish.io包"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\saltedfish_io.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\saltedfish_io.h
# End Source File
# End Group
# End Group
# Begin Group "本地方法注册器"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\headers\registry.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\本地方法库说明.txt"
# End Source File
# End Group
# Begin Group "运行时数据区"

# PROP Default_Filter ""
# Begin Group "堆区"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\Heap.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\Heap.h
# End Source File
# End Group
# Begin Group "方法区"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\methodArea.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\methodArea.h
# End Source File
# End Group
# Begin Group "垃圾回收器"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\Recycler.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\Recycler.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\运行时数据区说明.txt"
# End Source File
# End Group
# Begin Group "公共变量及方法"

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
# Begin Group "JVM入口"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\JVM.CPP
# End Source File
# End Group
# Begin Group "第三方库"

# PROP Default_Filter ""
# Begin Group "压缩文件读取库"

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
# Begin Group "JVM图标"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ico\jvm.ico
# End Source File
# Begin Source File

SOURCE=.\ico\rc.rc
# End Source File
# End Group
# Begin Group "UTF8解码器"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\UTF8CNDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\UTF8CNDecoder.h
# End Source File
# End Group
# Begin Group "虚拟机配置文件解析器"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpp\ConfigDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\headers\ConfigDecoder.h
# End Source File
# Begin Source File

SOURCE=".\explain\配置文件解析器说明.txt"
# End Source File
# End Group
# Begin Source File

SOURCE=".\explain\JVM模块调用图.TXT"
# End Source File
# Begin Source File

SOURCE=".\explain\JVM信息说明.TXT"
# End Source File
# End Target
# End Project
