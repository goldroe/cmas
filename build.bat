@echo off

IF NOT EXIST .build MKDIR .build
PUSHD .build
CL -nologo -FC -Zi -WX -W4 -wd4100 -wd4201 -wd4189 ..\source\main.c -Fecmas.exe
COPY cmas.exe ..
PUSHD 
