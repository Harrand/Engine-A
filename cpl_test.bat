@echo off
echo Setting compilation variables...
set scriptdir=%cd%
set incdir="%scriptdir%\inc"
set cpldirX=%scriptdir%\cpl\%date%
set cpldir=%cpldirX:/=.%
set libdir="%scriptdir%\lib"
set lnkdir="%cpldir%\lnk"
cd %scriptdir%
cd src
g++ -std=c++17 -Wall -Wextra -pedantic-errors -O3 -c demo.cpp -I%incdir% -I%scriptdir%\src
if not exist %lnkdir% mkdir %lnkdir%
color 5
echo Compiled demo.cpp...
move demo.o %lnkdir%
echo Copying over all dependencies...
cd "%scriptdir%\res\dep"
xcopy /s /I /Y "%cd%" %lnkdir%
color 4
echo Dependencies copied. Building final executable...
cd %lnkdir%
g++ -O3 -o topaz_test.exe demo.o "%scriptdir%\res\exe\topaz_test.res" -static-libstdc++ -L. -L%libdir% -L%scriptdir%\cmake-build-debug -lOpenGL32 -lSDL2 -lSDL2_mixer -lSDL2_ttf -lSDL2main -lengine_a.dll
move demo.o "%cpldir%"
color b
echo Topaz building complete, opening link directory...