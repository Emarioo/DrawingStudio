@echo off
@setlocal enabledelayedexpansion

SET COMPILE_OPTIONS=/DEBUG /std:c++17 /EHsc /TP /Z7 /MTd /nologo
SET DEFINITIONS=/DENGONE_PHYSICS /DENGONE_LOGGER /DENGONE_TRACKER /DENGONE_OPENGL
SET FORCE_INCLUDES=
SET INCLUDE_DIRS=/IDrawingStudio/include /Ilibs/GLEW/include /Ilibs/GLFW/include /Ilibs/RP3D/include
SET LINK_OPTIONS=/DEBUG /IGNORE:4006 /NOLOGO
SET LIBRARIES=Engone.lib
SET LIBRARY_DIRS=

if exist "D:\Backup\CodeProjects\ProjectUnknown\Engone\include\" (
    SET INCLUDE_DIRS=!INCLUDE_DIRS! /ID:\Backup\CodeProjects\ProjectUnknown\Engone\include
    echo d | xcopy "D:\Backup\CodeProjects\ProjectUnknown\Engone\include" "libs\Engone\include" /s /d > nul
) else (
    SET INCLUDE_DIRS=!INCLUDE_DIRS! /Ilibs\Engone\include
)
if exist "D:\Backup\CodeProjects\ProjectUnknown\bin\Engone\Debug-MSVC\Engone.lib" (
    SET LIBRARY_DIRS=!LIBRARY_DIRS! /LIBPATH:D:\Backup\CodeProjects\ProjectUnknown\bin\Engone\Debug-MSVC
    echo f | xcopy "D:\Backup\CodeProjects\ProjectUnknown\bin\Engone\Debug-MSVC\Engone.lib" "libs\Engone\lib\Engone.lib" /d > nul
) else (
    SET LIBRARY_DIRS=!LIBRARY_DIRS! /LIBPATH:libs/Engone/lib
)

set /a startTime=6000*( 100%time:~3,2% %% 100 ) + 100 * ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

@REM ########### VCVARS IS HERE ##########
call vcvars64.bat > nul

rem ######### GATHER FILES ################
SET objdir=bin\intermediates
mkdir %objdir% 2> nul

SET d_srcfile=!objdir!\all_studio.cpp

type nul > !d_srcfile!
for /r %%i in (*.cpp) do (
    SET file=%%i
    
    if not "x!file:DrwStd=!"=="x!file!" (
        echo #include ^"!file:\=/!^" >> !d_srcfile!
    )
)

rem ########## COMPILE #############

set /a c_startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

cl /c !COMPILE_OPTIONS! !FORCE_INCLUDES! !DEFINITIONS! !INCLUDE_DIRS! !objdir!\all_studio.cpp /Fo!objdir!\all_studio.o
if not %errorlevel% == 0 ( exit )

set /a c_endTime=6000*(100%time:~3,2% %% 100 )+100*(100%time:~6,2% %% 100 )+(100%time:~9,2% %% 100 )

rem ############# LINKING ################

mkdir bin\DrwStd 2> nul

rem Todo: link with optimizations or debug info?

set /a l_startTime=6000*( 100%time:~3,2% %% 100 ) + 100* ( 100%time:~6,2% %% 100 ) + ( 100%time:~9,2% %% 100 )

link !objdir!\all_studio.o !LIBRARY_DIRS! !LIBRARIES! !LINK_OPTIONS! /SUBSYSTEM:CONSOLE /OUT:bin/DrwStd.exe
if not %errorlevel% == 0 ( exit )

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