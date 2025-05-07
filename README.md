# Telegram 客户端 (TelegramClient)

## 项目描述

这个项目是一个简化的 Telegram 客户端，基于 Qt 和 MTProto 协议实现。项目提供基本的 Telegram 登录功能和用户信息获取功能，参考了官方 tdesktop 代码库的设计理念。

## 功能特点

- 使用 API ID 和 API Hash 登录 Telegram 账户
- 支持手机验证码登录流程
- 获取账户基本信息
- 模块化的代码结构
- Qt 界面设计
- SOCKS5 代理支持

## 目录结构

```
.
├── build/                # 构建目录（自动生成）
│   └── bin/
│       ├── debug/        # Debug版本输出目录
│       └── release/      # Release版本输出目录
├── include/              # 头文件目录
├── src/                  # 源代码目录
│   ├── core/             # 核心功能实现
│   │   ├── telegram_client.h
│   │   └── telegram_client.cpp
│   ├── mtproto/          # MTProto协议实现
│   │   ├── mtproto_client.h
│   │   └── mtproto_client.cpp
│   ├── ui/               # 用户界面
│   │   ├── mainwindow.h
│   │   └── mainwindow.cpp
│   └── main.cpp          # 主程序入口
├── CMakeLists.txt        # 主CMake配置文件
├── build.bat             # Windows构建脚本
└── README.md             # 项目说明文件
```

## 如何获取 API ID 和 API Hash

1. 访问 https://my.telegram.org/auth
2. 登录您的 Telegram 账户
3. 点击"API development tools"
4. 填写应用信息，创建一个新的应用
5. 系统将生成 API ID 和 API Hash，供您在此客户端中使用

## 构建说明

该项目使用 CMake 管理构建过程，支持 Windows 平台下的 Qt 开发环境。

### 在 Windows 上构建（使用批处理脚本）

1. 确保已安装 Qt5 或 Qt6
2. 运行`build.bat`脚本
3. 程序会自动完成配置、编译
4. 构建成功后可以选择是否立即运行程序

> **注意**：当前脚本配置为使用 64 位 MinGW 编译器（路径：D:\install\msys64\mingw64）和 64 位 Qt6.9.0（路径：D:\install\Qt\6.9.0\mingw_64）。如需更改路径，请编辑`build.bat`文件中的相应变量。

### Qt 版本兼容性

本项目已经过更新以支持 Qt6：

- 使用 Qt6 构建整个项目
- 当前配置为使用 Qt 6.9.0（64 位）
- 使用匹配的 64 位 MinGW 编译器

### 手动构建（适用于所有平台）

```bash
# 创建并进入构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译项目
cmake --build .

# 运行程序
./bin/release/TelegramClient  # Linux/macOS
.\bin\release\TelegramClient.exe  # Windows
```

## 使用说明

1. 启动程序后，输入您的 API ID, API Hash 和电话号码
2. 点击"登录"按钮，系统将发送验证码到您的手机
3. 在验证码页面输入收到的验证码并点击"验证"
4. 登录成功后，点击"获取账户信息"按钮查看您的账户信息

### 配置 SOCKS5 代理

对于无法直接连接 Telegram 服务器的网络环境，您可以配置 SOCKS5 代理：

1. 在菜单栏点击"设置" -> "代理设置..."
2. 勾选"启用 SOCKS5 代理"
3. 输入代理服务器地址和端口
4. 如果代理需要认证，请输入用户名和密码
5. 点击"应用"保存设置

代理设置将被保存并在下次启动程序时自动应用。

## 技术细节

- 编程语言：C++17
- GUI 框架：Qt5
- 网络通信：QNetworkAccessManager
- 代理支持：QNetworkProxy (SOCKS5)
- 配置存储：QSettings
- 构建系统：CMake 3.14+
- 编译器：GCC/MinGW

## 注意事项

- 此客户端为简化实现，不包括完整的 MTProto 协议功能
- 仅支持基本的登录和用户信息获取
- 生产环境中请使用官方 Telegram Desktop 或更完整的实现
- SOCKS5 代理仅支持简单的连接方式，不支持 UDP 流量代理

## 许可证

[GPLv3 with OpenSSL exception]
