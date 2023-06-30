@echo off

IF NOT EXIST .build MKDIR .build

PUSHD .build
CL -nologo -FC -Zi -WX -W4 -wd4100 -wd4201 -wd4189 -DDEBUG -DDEVELOPER ..\code\cmas.cpp ..\code\tok.cpp ..\code\parse.cpp ..\code\string.cpp -Fecmas.exe
COPY cmas.exe ..
POPD
