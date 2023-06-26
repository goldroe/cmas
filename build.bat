@echo off

IF NOT EXIST .build MKDIR .build

PUSHD .build
CL -nologo -FC -Zi -WX -W4 -wd4100 -wd4201 -wd4189 -DDEVELOPER ..\code\cmas.c ..\code\tok.c ..\code\parse.c ..\code\ast.c ..\code\common.c -Fecmas.exe
COPY cmas.exe ..
POPD
