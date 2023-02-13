@echo off

SET INCLUDES=-I ext/stb
SET SRC=src\main.cpp src/common.cpp src/lex.cpp
SET WARNING_FLAGS=-W4 -WX -wd4100 -wd4101 -wd4189 -wd4996 -wd4530 -wd4102
SET COMPILER_FLAGS=-DDEBUG -D_DEBUG -nologo -FC -MD -Zi %WARNING_FLAGS% %INCLUDES% -Febin/cmas.exe -Fobin\ -Fdbin\

CL %COMPILER_FLAGS% %SRC%
