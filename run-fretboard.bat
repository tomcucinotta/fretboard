@echo off
echo Fretboard - visual chords for guitar or ukulele on an ANSI terminal
echo Copyright (c) 2019 by Tommaso Cucinotta
set mode="-g"
echo;
fretboard %mode%
:rep
echo;
echo Type a chord name (Am, G, C7, Fmaj7, ...), or any of the commands:
echo   exit (or x) ... exit the program
echo   guit (or g) ... guitar mode
echo   ukul (or u) ... ukulele mode
set /P crd="your choice: "

if "%crd%"=="exit" exit
if "%crd%"=="x" exit
if "%crd%"=="ukul" (set mode="-u" & echo Switched to ukulele mode & goto rep)
if "%crd%"=="u" (set mode="-u" & echo Switched to ukulele mode & goto rep)
if "%crd%"=="guit" (set mode="-g" & echo Switched to guitar mode & goto rep)
if "%crd%"=="g" (set mode="-g" & echo Switched to guitar mode & goto rep)
echo;
fretboard %mode% -c %crd%
goto rep
