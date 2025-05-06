@echo off
:: 设置代码页为UTF-8
chcp 65001 > nul

echo ===== 开始构建程序 =====
echo 使用CMake和MinGW配置项目（C++17，64位架构）

:: 清理之前的构建
if exist build (
    echo 清理之前的构建...
    rmdir /s /q build
)

:: 创建构建目录
mkdir build
cd build

:: 使用CMake配置项目
echo 正在配置项目...
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
if %ERRORLEVEL% neq 0 (
    echo CMake配置失败！
    cd ..
    pause
    exit /b 1
)

:: 编译项目
echo 正在编译项目...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo 编译失败！
    cd ..
    pause
    exit /b 1
)

echo 编译成功！
echo 可执行文件位于: "%CD%\bin\release\IntegerSwap.exe"

:: 返回上级目录
cd ..

echo ===== 是否运行程序？ =====
set /p run_choice=按Y运行程序，按其他键退出:
if /i "%run_choice%"=="Y" (
    echo 正在运行程序...
    .\build\bin\release\IntegerSwap.exe
)

pause 