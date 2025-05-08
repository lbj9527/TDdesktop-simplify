@echo off
:: 设置代码页为UTF-8
chcp 65001 > nul

echo [信息] 开始部署Telegram客户端...

:: 设置路径
set MINGW_PATH=D:\install\msys64\mingw64
set QTDIR=D:\install\Qt\6.9.0\mingw_64
set TARGET_DIR=.\build\bin\release
set VCPKG_BIN=C:\TDLib\td\vcpkg\installed\x64-windows\bin

:: 检查目标目录是否存在
if not exist "%TARGET_DIR%" (
    echo [错误] 目标目录不存在！请先构建项目。
    echo       预期路径: %TARGET_DIR%
    exit /b 1
)

:: 检查Qt目录是否存在
if not exist "%QTDIR%\bin\Qt6Core.dll" (
    echo [错误] Qt目录无效！找不到Qt核心库。
    echo       预期路径: %QTDIR%\bin\Qt6Core.dll
    exit /b 1
)

echo [信息] 复制Qt核心库...
copy /Y "%QTDIR%\bin\Qt6Core.dll" "%TARGET_DIR%" || echo [警告] 无法复制Qt6Core.dll
copy /Y "%QTDIR%\bin\Qt6Gui.dll" "%TARGET_DIR%" || echo [警告] 无法复制Qt6Gui.dll
copy /Y "%QTDIR%\bin\Qt6Widgets.dll" "%TARGET_DIR%" || echo [警告] 无法复制Qt6Widgets.dll
copy /Y "%QTDIR%\bin\Qt6Network.dll" "%TARGET_DIR%" || echo [警告] 无法复制Qt6Network.dll

echo [信息] 复制MinGW运行时库...
copy /Y "%MINGW_PATH%\bin\libgcc_s_seh-1.dll" "%TARGET_DIR%" || echo [警告] 无法复制libgcc_s_seh-1.dll
copy /Y "%MINGW_PATH%\bin\libstdc++-6.dll" "%TARGET_DIR%" || echo [警告] 无法复制libstdc++-6.dll
copy /Y "%MINGW_PATH%\bin\libwinpthread-1.dll" "%TARGET_DIR%" || echo [警告] 无法复制libwinpthread-1.dll

echo [信息] 复制Qt图像格式插件...
if not exist "%TARGET_DIR%\imageformats" mkdir "%TARGET_DIR%\imageformats"
copy /Y "%QTDIR%\plugins\imageformats\qjpeg.dll" "%TARGET_DIR%\imageformats\" || echo [警告] 无法复制qjpeg.dll
copy /Y "%QTDIR%\plugins\imageformats\qgif.dll" "%TARGET_DIR%\imageformats\" || echo [警告] 无法复制qgif.dll
copy /Y "%QTDIR%\plugins\imageformats\qico.dll" "%TARGET_DIR%\imageformats\" || echo [警告] 无法复制qico.dll
copy /Y "%QTDIR%\plugins\imageformats\qsvg.dll" "%TARGET_DIR%\imageformats\" || echo [警告] 无法复制qsvg.dll

echo [信息] 复制Qt平台插件...
if not exist "%TARGET_DIR%\platforms" mkdir "%TARGET_DIR%\platforms"
copy /Y "%QTDIR%\plugins\platforms\qwindows.dll" "%TARGET_DIR%\platforms\" || echo [警告] 无法复制qwindows.dll

echo [信息] 复制Qt样式插件...
if not exist "%TARGET_DIR%\styles" mkdir "%TARGET_DIR%\styles"
copy /Y "%QTDIR%\plugins\styles\qwindowsvistastyle.dll" "%TARGET_DIR%\styles\" || echo [警告] 无法复制qwindowsvistastyle.dll

echo [信息] 复制TLS后端插件...
if not exist "%TARGET_DIR%\tls" mkdir "%TARGET_DIR%\tls"
copy /Y "%QTDIR%\plugins\tls\qopensslbackend.dll" "%TARGET_DIR%\tls\" || echo [警告] 无法复制qopensslbackend.dll

echo [信息] 复制所有TLS相关插件...
dir /b "%QTDIR%\plugins\tls\*.dll" > "%TEMP%\tls_plugins.txt" 2>nul
for /F "usebackq tokens=*" %%A in ("%TEMP%\tls_plugins.txt") do (
  echo 复制TLS插件: %%A
  copy /Y "%QTDIR%\plugins\tls\%%A" "%TARGET_DIR%\tls\" || echo [警告] 无法复制%%A
)

echo [信息] 复制网络TLS插件...
if not exist "%TARGET_DIR%\networkinformation" mkdir "%TARGET_DIR%\networkinformation"
copy /Y "%QTDIR%\plugins\networkinformation\qnetworklistmanager.dll" "%TARGET_DIR%\networkinformation\" || echo [警告] 无法复制qnetworklistmanager.dll

echo [信息] 复制OpenSSL库...
set FOUND_SSL=0

echo [信息] 从VCPKG目录搜索OpenSSL库...
if exist "%VCPKG_BIN%" (
    echo [信息] 复制VCPKG中的OpenSSL库...
    copy /Y "%VCPKG_BIN%\libssl*.dll" "%TARGET_DIR%" || echo [警告] 无法复制libssl*.dll
    copy /Y "%VCPKG_BIN%\libcrypto*.dll" "%TARGET_DIR%" || echo [警告] 无法复制libcrypto*.dll
    copy /Y "%VCPKG_BIN%\ssl*.dll" "%TARGET_DIR%" || echo [警告] 无法复制ssl*.dll
    copy /Y "%VCPKG_BIN%\crypto*.dll" "%TARGET_DIR%" || echo [警告] 无法复制crypto*.dll
    
    echo [信息] 检查和复制VCPKG依赖库...
    dir /b "%VCPKG_BIN%\*.dll" > "%TEMP%\vcpkg_libs.txt" 2>nul
    for /F "usebackq tokens=*" %%A in ("%TEMP%\vcpkg_libs.txt") do (
        echo 复制库: %%A
        copy /Y "%VCPKG_BIN%\%%A" "%TARGET_DIR%" || echo [警告] 无法复制%%A
    )
    set FOUND_SSL=1
) else (
    echo [警告] VCPKG目录不存在: %VCPKG_BIN%
)

REM 检查MinGW目录中的OpenSSL库
if %FOUND_SSL% equ 0 (
  echo [信息] 从MinGW目录搜索OpenSSL库...
  dir /b "%MINGW_PATH%\bin\libssl*.dll" > "%TEMP%\ssl_libs.txt" 2>nul
  dir /b "%MINGW_PATH%\bin\libcrypto*.dll" >> "%TEMP%\ssl_libs.txt" 2>nul
  for /F "usebackq tokens=*" %%A in ("%TEMP%\ssl_libs.txt") do (
    echo 复制OpenSSL库: %%A
    copy /Y "%MINGW_PATH%\bin\%%A" "%TARGET_DIR%" || echo [警告] 无法复制%%A
    set FOUND_SSL=1
  )
)

REM 检查Qt目录中的OpenSSL库
if %FOUND_SSL% equ 0 (
  echo [信息] 从Qt目录搜索OpenSSL库...
  dir /b "%QTDIR%\bin\libssl*.dll" > "%TEMP%\ssl_libs.txt" 2>nul
  dir /b "%QTDIR%\bin\libcrypto*.dll" >> "%TEMP%\ssl_libs.txt" 2>nul
  for /F "usebackq tokens=*" %%A in ("%TEMP%\ssl_libs.txt") do (
    echo 复制OpenSSL库: %%A
    copy /Y "%QTDIR%\bin\%%A" "%TARGET_DIR%" || echo [警告] 无法复制%%A
    set FOUND_SSL=1
  )
)

REM 尝试从Windows系统路径搜索OpenSSL库
if %FOUND_SSL% equ 0 (
  echo [信息] 从系统路径搜索OpenSSL库...
  where /r C:\ libssl*.dll > "%TEMP%\ssl_paths.txt" 2>nul
  where /r C:\ libcrypto*.dll >> "%TEMP%\ssl_paths.txt" 2>nul
  for /F "usebackq tokens=*" %%A in ("%TEMP%\ssl_paths.txt") do (
    echo 发现系统OpenSSL库: %%A
    copy /Y "%%A" "%TARGET_DIR%" || echo [警告] 无法复制%%A
    set FOUND_SSL=1
  )
)

if %FOUND_SSL% equ 0 (
  echo [警告] 未找到OpenSSL库，尝试下载并安装最新版本...
  echo 请访问 https://slproweb.com/products/Win32OpenSSL.html 下载并安装OpenSSL
  echo 安装后重新运行此脚本
)

REM 复制ICU库（如果需要）
echo [信息] 复制ICU库（如果存在）...
if exist "%QTDIR%\bin\icudt*.dll" (
    copy /Y "%QTDIR%\bin\icudt*.dll" "%TARGET_DIR%" || echo [警告] 无法复制ICU数据库DLL
    copy /Y "%QTDIR%\bin\icuin*.dll" "%TARGET_DIR%" || echo [警告] 无法复制ICU国际化DLL
    copy /Y "%QTDIR%\bin\icuuc*.dll" "%TARGET_DIR%" || echo [警告] 无法复制ICU通用DLL
)

REM 创建qt.conf文件
echo [信息] 创建qt.conf文件...
echo [Paths] > "%TARGET_DIR%\qt.conf"
echo Plugins = ./ >> "%TARGET_DIR%\qt.conf"

echo [信息] 部署完成！现在可以运行 %TARGET_DIR%\TelegramClient.exe
echo [信息] 如果遇到TLS相关错误，确保OpenSSL库已正确安装，并确认有以下文件：
echo [信息] - %TARGET_DIR%\tls\qopensslbackend.dll
echo [信息] - %TARGET_DIR%\libssl*.dll
echo [信息] - %TARGET_DIR%\libcrypto*.dll

if %FOUND_SSL% equ 0 (
  echo [警告] 未能找到OpenSSL库！程序可能无法正常使用TLS功能
  echo [警告] 网络连接可能会失败
)

exit /b 0 