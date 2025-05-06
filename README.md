# 整数交换程序 (IntegerSwap)

## 项目描述
这个项目展示了两种在C++中交换整数值的方法：使用指针和使用引用。程序演示了这两种方法的工作原理，并输出交换前后的结果。

## 功能特点
- 使用指针实现整数交换
- 使用引用实现整数交换
- 支持中文输出（使用UTF-8编码）

## 目录结构
```
.
├── build/                # 构建目录（自动生成）
│   └── bin/
│       ├── debug/        # Debug版本输出目录
│       └── release/      # Release版本输出目录
├── include/              # 头文件目录
├── src/                  # 源代码目录
│   └── main.cpp          # 主程序源文件
├── CMakeLists.txt        # 主CMake配置文件
├── build.bat             # Windows构建脚本
└── README.md             # 项目说明文件
```

## 构建说明
该项目使用CMake管理构建过程，支持Windows平台下的MinGW编译器。

### 在Windows上构建（使用批处理脚本）
1. 运行`build.bat`脚本
2. 程序会自动完成配置、编译
3. 构建成功后可以选择是否立即运行程序

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
./bin/release/IntegerSwap  # Linux/macOS
.\bin\release\IntegerSwap.exe  # Windows
```

## 技术细节
- 编程语言：C++17
- 构建系统：CMake 3.14+
- 编译器：MinGW GCC
- 支持平台：Windows（可扩展到其他平台）

## 许可证
[添加您的许可证信息] 