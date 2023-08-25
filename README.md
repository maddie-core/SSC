# Spaceship Compiler
---

## Installation
Recommended installation for windows if using Ninja (with cl) without visual studio:
Batch file layout:
```
:: Likely found under C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat
call <vcvarall.bat>

:: /W3   -- Warning level of 3
:: /GR   -- fast call conv. except for member functions
:: /MDd  -- Multithreaded debug version of standard library
:: /Zi   -- Create PDB file for seperate debugging information
:: /Ob0  -- Disables inline expansion
:: /Od   -- Disables optimizations
:: /RTC1 -- combination of /RTCs and /RTCu. Warns about unitialization data checks for stack corruption stuff.

cmake -S . -B "<build directory>" -G "Ninja" ^
-DCMAKE_MAKE_PROGRAM="<Path to Ninja installation>" ^
-DCMAKE_CXX_COMPILER="<path cl compiler>" ^
-DCMAKE_EXE_LINKER_FLAGS="/machine:<x32|x64>" ^
-DCMAKE_LINKER="<path to linker>" ^
-DCMAKE_CXX_FLAGS="/W3 /GR" ^
-DCMAKE_CXX_FLAGS_DEBUG="/MDd /Zi /Ob0 /Od /RTC1 /EHsc" ^
-DCMAKE_CXX_LINKER_FLAGS_DEBUG="/debug /INCREMENTAL" ^
-DCMAKE_CXX_STANDARD_LIBRARIES="kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib Dbghelp.lib"
``` 
