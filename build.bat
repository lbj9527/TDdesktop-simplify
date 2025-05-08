@echo off
chcp 65001 >nul
echo [信息] 正在为Telegram客户端配置构建环境...

REM 设置Qt目录
set QTDIR=D:\install\Qt\6.9.0\mingw_64
set PATH=%QTDIR%\bin;D:\install\msys64\mingw64\bin;%PATH%
set QT_VERSION_MAJOR=6
set VCPKG_BIN=C:\TDLib\td\vcpkg\installed\x64-windows\bin

REM 配置路径
set BUILD_DIR=build
set BIN_DIR=%BUILD_DIR%\bin
set RELEASE_DIR=%BIN_DIR%\release

REM 创建构建目录
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

REM 进入构建目录并运行CMake配置
cd %BUILD_DIR%
echo [信息] 正在运行CMake配置...
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QTDIR% ..

REM 编译代码
echo [信息] 正在编译项目...
cmake --build . --config Release

REM 检查编译是否成功
if %ERRORLEVEL% neq 0 (
    echo [错误] 编译失败！
    cd ..
    exit /b %ERRORLEVEL%
)

echo [信息] 编译成功！

REM 确保目标目录存在
if not exist %RELEASE_DIR% (
    echo [信息] 创建发布目录...
    mkdir %RELEASE_DIR%
)

echo [信息] 正在复制所需的Qt库文件...

REM 复制Qt核心库
echo [信息] 复制Qt核心库...
copy /Y "%QTDIR%\bin\Qt6Core.dll" "%RELEASE_DIR%" || echo [警告] 无法复制Qt6Core.dll
copy /Y "%QTDIR%\bin\Qt6Gui.dll" "%RELEASE_DIR%" || echo [警告] 无法复制Qt6Gui.dll
copy /Y "%QTDIR%\bin\Qt6Widgets.dll" "%RELEASE_DIR%" || echo [警告] 无法复制Qt6Widgets.dll
copy /Y "%QTDIR%\bin\Qt6Network.dll" "%RELEASE_DIR%" || echo [警告] 无法复制Qt6Network.dll

REM 复制Qt平台插件
echo [信息] 复制Qt平台插件...
if not exist "%RELEASE_DIR%\platforms" mkdir "%RELEASE_DIR%\platforms"
copy /Y "%QTDIR%\plugins\platforms\qwindows.dll" "%RELEASE_DIR%\platforms\" || echo [警告] 无法复制qwindows.dll

REM 复制Qt样式插件
echo [信息] 复制Qt样式插件...
if not exist "%RELEASE_DIR%\styles" mkdir "%RELEASE_DIR%\styles"
copy /Y "%QTDIR%\plugins\styles\qwindowsvistastyle.dll" "%RELEASE_DIR%\styles\" || echo [警告] 无法复制qwindowsvistastyle.dll

REM 复制Qt图像格式插件
echo [信息] 复制Qt图像格式插件...
if not exist "%RELEASE_DIR%\imageformats" mkdir "%RELEASE_DIR%\imageformats"
copy /Y "%QTDIR%\plugins\imageformats\qjpeg.dll" "%RELEASE_DIR%\imageformats\" || echo [警告] 无法复制qjpeg.dll
copy /Y "%QTDIR%\plugins\imageformats\qgif.dll" "%RELEASE_DIR%\imageformats\" || echo [警告] 无法复制qgif.dll
copy /Y "%QTDIR%\plugins\imageformats\qico.dll" "%RELEASE_DIR%\imageformats\" || echo [警告] 无法复制qico.dll
copy /Y "%QTDIR%\plugins\imageformats\qsvg.dll" "%RELEASE_DIR%\imageformats\" || echo [警告] 无法复制qsvg.dll

REM 复制TLS后端插件
echo [信息] 复制TLS后端插件...
if not exist "%RELEASE_DIR%\tls" mkdir "%RELEASE_DIR%\tls"
echo [详细] 搜索TLS插件于 %QTDIR%\plugins\tls\...
dir /b "%QTDIR%\plugins\tls\*.dll" > "%TEMP%\tls_plugins.txt" 2>nul
for /F "usebackq tokens=*" %%A in ("%TEMP%\tls_plugins.txt") do (
  echo [详细] 复制TLS插件: %%A
  copy /Y "%QTDIR%\plugins\tls\%%A" "%RELEASE_DIR%\tls\" || echo [警告] 无法复制%%A
)

REM 复制网络TLS插件
echo [信息] 复制网络TLS插件...
if not exist "%RELEASE_DIR%\networkinformation" mkdir "%RELEASE_DIR%\networkinformation"
copy /Y "%QTDIR%\plugins\networkinformation\qnetworklistmanager.dll" "%RELEASE_DIR%\networkinformation\" || echo [警告] 无法复制qnetworklistmanager.dll

REM 复制MinGW运行时库
echo [信息] 复制MinGW运行时库...
set MINGW_PATH=D:\install\msys64\mingw64
copy /Y "%MINGW_PATH%\bin\libgcc_s_seh-1.dll" "%RELEASE_DIR%" || echo [警告] 无法复制libgcc_s_seh-1.dll
copy /Y "%MINGW_PATH%\bin\libstdc++-6.dll" "%RELEASE_DIR%" || echo [警告] 无法复制libstdc++-6.dll
copy /Y "%MINGW_PATH%\bin\libwinpthread-1.dll" "%RELEASE_DIR%" || echo [警告] 无法复制libwinpthread-1.dll

REM 复制OpenSSL库
echo [信息] 复制OpenSSL库...
set FOUND_SSL=0

REM 检查VCPKG目录中的OpenSSL库
echo [信息] 从VCPKG目录搜索OpenSSL库...
if exist "%VCPKG_BIN%" (
    echo [详细] 复制VCPKG中的OpenSSL库...
    copy /Y "%VCPKG_BIN%\libssl*.dll" "%RELEASE_DIR%" || echo [警告] 无法复制libssl*.dll
    copy /Y "%VCPKG_BIN%\libcrypto*.dll" "%RELEASE_DIR%" || echo [警告] 无法复制libcrypto*.dll
    copy /Y "%VCPKG_BIN%\ssl*.dll" "%RELEASE_DIR%" || echo [警告] 无法复制ssl*.dll
    copy /Y "%VCPKG_BIN%\crypto*.dll" "%RELEASE_DIR%" || echo [警告] 无法复制crypto*.dll
    
    echo [详细] 检查和复制VCPKG依赖库...
    dir /b "%VCPKG_BIN%\*.dll" > "%TEMP%\vcpkg_libs.txt" 2>nul
    for /F "usebackq tokens=*" %%A in ("%TEMP%\vcpkg_libs.txt") do (
        echo 复制库: %%A
        copy /Y "%VCPKG_BIN%\%%A" "%RELEASE_DIR%" || echo [警告] 无法复制%%A
    )
    set FOUND_SSL=1
) else (
    echo [警告] VCPKG目录不存在: %VCPKG_BIN%
)

REM 检查MinGW目录中的OpenSSL库
if %FOUND_SSL% equ 0 (
  echo [详细] 从MinGW目录搜索OpenSSL库...
  dir /b "%MINGW_PATH%\bin\libssl*.dll" > "%TEMP%\ssl_libs.txt" 2>nul
  dir /b "%MINGW_PATH%\bin\libcrypto*.dll" >> "%TEMP%\ssl_libs.txt" 2>nul
  for /F "usebackq tokens=*" %%A in ("%TEMP%\ssl_libs.txt") do (
    echo [详细] 复制OpenSSL库: %%A
    copy /Y "%MINGW_PATH%\bin\%%A" "%RELEASE_DIR%" || echo [警告] 无法复制%%A
    set FOUND_SSL=1
  )
)

REM 检查Qt目录中的OpenSSL库
if %FOUND_SSL% equ 0 (
  echo [详细] 从Qt目录搜索OpenSSL库...
  dir /b "%QTDIR%\bin\libssl*.dll" > "%TEMP%\ssl_libs.txt" 2>nul
  dir /b "%QTDIR%\bin\libcrypto*.dll" >> "%TEMP%\ssl_libs.txt" 2>nul
  for /F "usebackq tokens=*" %%A in ("%TEMP%\ssl_libs.txt") do (
    echo [详细] 复制OpenSSL库: %%A
    copy /Y "%QTDIR%\bin\%%A" "%RELEASE_DIR%" || echo [警告] 无法复制%%A
    set FOUND_SSL=1
  )
)

if %FOUND_SSL% equ 0 (
    echo [警告] 未找到OpenSSL库，TLS功能可能不可用！
    echo [提示] 请考虑安装OpenSSL并重新运行deploy.bat脚本
)

REM 创建qt.conf文件
echo [信息] 创建qt.conf文件...
echo [Paths] > "%RELEASE_DIR%\qt.conf"
echo Plugins = ./ >> "%RELEASE_DIR%\qt.conf"

echo [信息] 构建完成！可执行文件位于 %RELEASE_DIR%\TelegramClient.exe
cd ..

exit /b 0