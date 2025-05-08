# Telegram 客户端

一个简化版的 Telegram 客户端，使用 C++和 Qt6 开发。

## 功能特点

- 简洁的用户界面
- 配置文件保存在程序目录
- 支持中文界面
- 完整的 TLS 加密支持

## 安装要求

- Qt6（已针对 Qt 6.9.0 优化）
- MinGW 64 位编译器
- Windows 10 或更高版本

## 在 Windows 上构建（使用批处理脚本）

1. 确保已安装 Qt6 和 MinGW 64 位编译器
2. 运行`build.bat`脚本进行编译
3. 编译成功后，可执行文件将位于`build/bin/release`目录

> **注意**: 脚本已配置为使用 64 位 MinGW 编译器和 Qt 6.9.0，路径为`D:\install\Qt\6.9.0\mingw_64`。如需更改，请编辑`build.bat`文件中的路径设置。

## 手动构建

如果您不想使用批处理脚本，也可以手动构建项目：

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=D:/install/Qt/6.9.0/mingw_64 ..
cmake --build . --config Release
```

## 部署

编译完成后，您需要确保所有必要的 Qt 库文件都已复制到可执行文件目录。可以使用以下方法之一：

1. 运行`deploy.bat`脚本，它会自动复制所有必要的库文件
2. 使用`cmake --install .`命令安装程序（需要先设置`CMAKE_INSTALL_PREFIX`）

## 配置文件

程序使用 JSON 格式的配置文件保存设置：

- 配置文件保存在程序目录下的`config.json`
- 程序启动时会自动加载配置文件
- 程序退出时会自动保存配置文件
- 可以通过菜单"文件 > 配置文件位置..."查看配置文件位置

## 技术细节

- 程序使用 Qt6 框架开发
- 使用 C++17 标准
- 配置文件采用 JSON 格式，而不是传统的 QSettings
- 程序退出时自动保存配置
- 使用 UTF-8 编码处理所有文本

## Qt 版本兼容性

该项目使用 Qt6 构建，经过测试的具体版本为 Qt 6.9.0。
项目针对 64 位架构优化，推荐使用 64 位 MinGW 编译器。

## 故障排除

如果遇到"找不到 DLL 文件"的错误，请确保：

1. 已运行`deploy.bat`脚本复制所有必要的 DLL
2. 检查 OpenSSL 库是否已正确复制（对 TLS 功能至关重要）
3. 确保使用 UTF-8 编码处理文本（如果看到乱码）

若程序无法启动，请在命令行中运行以查看错误信息：

```cmd
cd build\bin\release
TelegramClient.exe
```
