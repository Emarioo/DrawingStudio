@echo off
@setlocal enabledelayedexpansion

@REM set WITH_CONSOLE=1

SET COMPILE_OPTIONS=/std:c++17 /EHsc /TP /MTd /nologo /Zi
SET LINK_OPTIONS=/NOLOGO /INCREMENTAL:NO /IGNORE:4006 /IGNORE:4098 /DEBUG /IGNORE:4006 /ignore:4099 Advapi32.lib gdi32.lib shell32.lib user32.lib OpenGL32.lib
SET INCLUDE_DIRS=/Iinclude /Ilibs/stb/include /Ilibs/glfw-3.3.8/include /Ilibs/glew-2.1.0/include /Ilibs/glm/include
SET DEFINITIONS=/DOS_WINDOWS /FI pch.h /DGLEW_STATIC

SET LINK_OPTIONS=!LINK_OPTIONS! /DEBUG libs/glfw-3.3.8/lib/glfw3_mt.lib libs/glew-2.1.0/lib/glew32s.lib

if !WITH_CONSOLE!==1 (
    SET LINK_OPTIONS=!LINK_OPTIONS! /SUBSYSTEM:CONSOLE
) else (
    SET LINK_OPTIONS=!LINK_OPTIONS! /SUBSYSTEM:WINDOWS
) 


set /a startTime=6000*( 100%time:~3,2% %% 100 ) + 100 * ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

rem ######### GATHER FILES ################
SET objdir=bin\intermediates
mkdir %objdir% 2> nul

SET d_srcfile=!objdir!\all_studio.cpp

type nul > !d_srcfile!
for /r %%i in (*.cpp) do (
    SET file=%%i
    
    if "x!file:__=!"=="x!file!" if "x!file:bin=!"=="x!file!" (
        if not "x!file:DrwStd=!"=="x!file!" (
            echo #include ^"!file:\=/!^">> !d_srcfile!
        ) else if not "x!file:Engone=!"=="x!file!" (
            echo #include ^"!file:\=/!^">> !d_srcfile!
        )
    )
)

rem ########## COMPILE #############

set /a c_startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

cl /c !COMPILE_OPTIONS! !DEFINITIONS! !INCLUDE_DIRS! !objdir!\all_studio.cpp /Fo!objdir!\all_studio.o
@REM if not %errorlevel% == 0 ( exit )

set /a c_endTime=6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )

rem ############# LINKING ################

mkdir bin\DrwStd 2> nul

rem Todo: link with optimizations or debug info?

set /a l_startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

@REM rc /?
rc /nologo /fo bin/intermediates/resources.res DrawingStudio\resources.rc

link !objdir!\all_studio.o !LINK_OPTIONS! bin/intermediates/resources.res /OUT:bin/DrwStd.exe
@REM if not %errorlevel% == 0 ( exit )

set /a l_endTime=6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )

set /a endTime=6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )

set /a c_finS=(c_endTime-c_startTime)/100
set /a c_finS2=(c_endTime-c_startTime)%%100
set /a l_finS=(l_endTime-l_startTime)/100
set /a l_finS2=(l_endTime-l_startTime)%%100
set /a finS=(endTime-startTime)/100
set /a finS2=(endTime-startTime)%%100

echo Compilation in %c_finS%.%c_finS2% seconds
echo Linking in %l_finS%.%l_finS2% seconds
echo Finished in %finS%.%finS2% seconds

@REM pushd bin
bin\DrwStd.exe
@REM popd