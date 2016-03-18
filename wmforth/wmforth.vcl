<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: wmforth - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\FUJI\LOCALS~1\Temp\RSP41C.tmp" with contents
[
/nologo /W3 /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /Fp"ARMV4Rel/wmforth.pch" /YX /Fo"ARMV4Rel/" /O2 /MC /c 
"C:\jacques\sourceforge\wmforth\forth.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\FUJI\LOCALS~1\Temp\RSP41C.tmp" 
Creating temporary file "C:\DOCUME~1\FUJI\LOCALS~1\Temp\RSP41D.tmp" with contents
[
commctrl.lib coredll.lib winsock.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:no /pdb:"ARMV4Rel/wmforth.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4Rel/wmforth.exe" /subsystem:windowsce,4.00 /align:"4096" /MACHINE:ARM 
.\ARMV4Rel\asm.obj
.\ARMV4Rel\forth.obj
.\ARMV4Rel\forthcode.obj
.\ARMV4Rel\util.obj
]
Creating command line "link.exe @C:\DOCUME~1\FUJI\LOCALS~1\Temp\RSP41D.tmp"
<h3>Output Window</h3>
Compiling...
forth.cpp
c:\jacques\sourceforge\wmforth\forth.cpp(731) : warning C4101: 'bufinbuf' : unreferenced local variable
c:\jacques\sourceforge\wmforth\forth.cpp(730) : warning C4101: 'szHello' : unreferenced local variable
c:\jacques\sourceforge\wmforth\forth.cpp(729) : warning C4101: 'ps' : unreferenced local variable
c:\jacques\sourceforge\wmforth\forth.cpp(727) : warning C4101: 'hdc' : unreferenced local variable
c:\jacques\sourceforge\wmforth\forth.cpp(770) : warning C4101: 'rt' : unreferenced local variable
Linking...



<h3>Results</h3>
wmforth.exe - 0 error(s), 5 warning(s)
</pre>
</body>
</html>
