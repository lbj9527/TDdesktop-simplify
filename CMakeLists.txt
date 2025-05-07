cmake_minimum_required(VERSION 3.14)

# 设置项目名称
project(TelegramClient VERSION 1.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 增加调试信息
set(CMAKE_VERBOSE_MAKEFILE ON)

# 设置Qt6前缀路径 - 确保使用64位版本
set(CMAKE_PREFIX_PATH "D:/install/Qt/6.9.0/mingw_64")

# 指定64位架构
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# 只查找Qt6包
find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)
message(STATUS "使用Qt6构建项目: ${Qt6_VERSION}")
set(QT_MAJOR_VERSION 6)

# Qt6特定设置
set(CMAKE_AUTOMOC_MOC_OPTIONS "-DWIN32")
# 确保找到Qt6工具
find_program(QT_MOC_EXECUTABLE NAMES moc moc6 HINTS "${CMAKE_PREFIX_PATH}/bin" REQUIRED)
find_program(QT_UIC_EXECUTABLE NAMES uic uic6 HINTS "${CMAKE_PREFIX_PATH}/bin" REQUIRED)
find_program(QT_RCC_EXECUTABLE NAMES rcc rcc6 HINTS "${CMAKE_PREFIX_PATH}/bin" REQUIRED)

message(STATUS "找到Qt MOC: ${QT_MOC_EXECUTABLE}")
message(STATUS "找到Qt UIC: ${QT_UIC_EXECUTABLE}")
message(STATUS "找到Qt RCC: ${QT_RCC_EXECUTABLE}")
message(STATUS "系统处理器架构: ${CMAKE_SYSTEM_PROCESSOR}")

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Debug和Release分别输出到不同目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)

# 设置编译类型(如果未指定)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 开启Qt的MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 添加源文件目录
add_subdirectory(src)

# 为可执行文件添加链接选项（移除静态链接，避免兼容性问题）
# if(WIN32)
#     target_link_libraries(IntegerSwap PRIVATE -static-libgcc -static-libstdc++)
# endif() 