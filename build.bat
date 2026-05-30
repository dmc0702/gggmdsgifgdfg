@echo off
echo ========== 编译GameApp ==========

cd /d C:\Users\Administrator\Desktop\new-folder

echo 正在生成Makefile...
C:\Qt\6.11.1\mingw_64\bin\qmake.exe GameApp.pro

if errorlevel 1 (
    echo qmake失败！
    pause
    exit /b
)

echo 正在编译...
C:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe

if errorlevel 1 (
    echo 编译失败！
    pause
    exit /b
)

echo 编译成功！

if exist release\GameApp.exe (
    echo 正在运行...
    release\GameApp.exe
) else if exist debug\GameApp.exe (
    debug\GameApp.exe
) else (
    echo 查找可执行文件...
    dir /s *.exe
)

pause