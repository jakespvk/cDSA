@echo off

clang-cl PriorityQueue.c ^
    /vctoolsdir "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.51.36231" ^
    /winsdkdir "C:\Program Files (x86)\Windows Kits\10" ^
    /winsdkversion 10.0.26100.0 ^
    user32.lib ^
    Gdi32.lib ^
    -FC ^
    -Zi ^
    -o main.exe
