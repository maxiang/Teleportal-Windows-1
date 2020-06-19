# teleportalQT

HOW TO RUN

1: Install 
a) gstreamer-1.0-mingw-x86_64-1.16.0.msi
b) gstreamer-1.0-devel-mingw-x86_64-1.16.0.msi

2: Set the windows environment variable
a) GSTREAMER_1_0_ROOT_X86_64=C:\gstreamer\1.0\x86_64\
b) PKG_CONFIG_PATH=C:\gstreamer\1.0\x86_64\lib\pkgconfig
c) C:\gstreamer\1.0\x86_64\bin added to %path%

3: 
a) gstqmlgl.dll is a plugin I compiled. 
b) It needs to be added to C:\gstreamer\1.0\x86_64\lib\gstreamer-1.0

4: Run TeleportalQT.exe in /RunDir/


HOW TO BUILD

1. boost_1.73
2. qt5.14.1 mingw64
3. ok!
4. open teleportalQT.pro start working
