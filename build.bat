@echo off
:: 设置代码页为UTF-8
chcp 65001 > nul

echo ===== 开始构建Telegram客户端 =====
echo 使用CMake和自定义MinGW64配置项目（C++17）

:: 设置MinGW路径 - 改为64位版本
set "MINGW_PATH=D:\install\msys64\mingw64"
echo 使用MinGW路径: %MINGW_PATH%

:: 检查MinGW路径是否存在
if not exist "%MINGW_PATH%\bin\gcc.exe" (
    echo 错误：找不到MinGW编译器! 请确认路径: %MINGW_PATH%
    echo 预期路径: %MINGW_PATH%\bin\gcc.exe
    pause
    exit /b 1
)

:: 设置Qt安装路径 - 保持64位版本
set "QTDIR=D:\install\Qt\6.9.0\mingw_64"
echo 使用Qt目录: %QTDIR%

:: 检查Qt路径是否存在
if not exist "%QTDIR%\bin\qmake.exe" (
    echo 错误：在指定的Qt路径中找不到qmake.exe: %QTDIR%
    echo 请确保Qt安装路径正确。
    pause
    exit /b 1
)

:: 将MinGW和Qt的bin目录添加到PATH环境变量
set "PATH=%MINGW_PATH%\bin;%QTDIR%\bin;%PATH%"

:: 检查是否有MSYS2环境变量（可选）
if defined MSYS2_PATH (
    echo 检测到MSYS2环境: %MSYS2_PATH%
    set "PATH=%MSYS2_PATH%\usr\bin;%PATH%"
)

:: 清理之前的构建
if exist build (
    echo 清理之前的构建...
    rmdir /s /q build
)

:: 创建构建目录
mkdir build
cd build

:: 设置环境变量，避免路径问题
set "Qt6_DIR=%QTDIR%\lib\cmake\Qt6"
set "Qt6CoreTools_DIR=%QTDIR%\lib\cmake\Qt6CoreTools"
set "Qt6WidgetsTools_DIR=%QTDIR%\lib\cmake\Qt6WidgetsTools"
set "CMAKE_PREFIX_PATH=%QTDIR%"

:: 设置编译标志，处理空格和特殊字符
set "CXX_FLAGS=-Wno-deprecated-declarations -Wno-class-memaccess -fpermissive -Wno-missing-template-arg-list-after-template-kw"

:: 使用CMake配置项目
echo 正在配置项目...
cmake -G "MinGW Makefiles" ^
      -DCMAKE_PREFIX_PATH="%QTDIR%" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_C_COMPILER="%MINGW_PATH%\bin\gcc.exe" ^
      -DCMAKE_CXX_COMPILER="%MINGW_PATH%\bin\g++.exe" ^
      -DCMAKE_CXX_FLAGS="%CXX_FLAGS%" ^
      -DCMAKE_MAKE_PROGRAM="%MINGW_PATH%\bin\mingw32-make.exe" ^
      -DCMAKE_VERBOSE_MAKEFILE=ON ^
      ..

if %ERRORLEVEL% neq 0 (
    echo CMake配置失败！
    cd ..
    pause
    exit /b 1
)

:: 编译项目
echo 正在编译项目...
cmake --build . --config Release -- -j4

if %ERRORLEVEL% neq 0 (
    echo 编译失败！
    cd ..
    pause
    exit /b 1
)

echo 编译成功！
echo 可执行文件位于: "%CD%\bin\release\TelegramClient.exe"

:: 返回上级目录
cd ..

echo ===== 是否运行程序？ =====
set /p run_choice=按Y运行程序，按其他键退出:
if /i "%run_choice%"=="Y" (
    echo 正在运行程序...
    .\build\bin\release\TelegramClient.exe
)

pause