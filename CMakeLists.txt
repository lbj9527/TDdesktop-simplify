cmake_minimum_required(VERSION 3.14)

# 设置项目名称
project(TelegramClient VERSION 1.0 LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 增加调试信息
set(CMAKE_VERBOSE_MAKEFILE ON)

# 指定64位架构
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# 使用Qt6
set(QT_VERSION_MAJOR 6)
message(STATUS "使用Qt主版本: ${QT_VERSION_MAJOR}")

# 设置VCPKG路径
set(VCPKG_DIR "C:/TDLib/td/vcpkg/installed/x64-windows")
list(APPEND CMAKE_PREFIX_PATH ${VCPKG_DIR})
message(STATUS "添加VCPKG路径: ${VCPKG_DIR}")

# 查找Qt6包
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
message(STATUS "使用的Qt路径: ${CMAKE_PREFIX_PATH}")

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
include_directories(${VCPKG_DIR}/include) # 添加VCPKG包含目录

# 设置库目录
link_directories(${VCPKG_DIR}/lib) # 添加VCPKG库目录

# 开启Qt的MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 添加源文件目录
add_subdirectory(src)

# 为Windows添加安装规则，复制Qt DLL和插件
if(WIN32)
    # 定义安装规则
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
    
    # 安装Qt核心库
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    get_filename_component(_qt_root_dir "${_qt_bin_dir}" DIRECTORY)
    
    message(STATUS "Qt二进制文件目录: ${_qt_bin_dir}")
    message(STATUS "Qt根目录: ${_qt_root_dir}")
    
    # 复制核心Qt DLLs
    install(FILES
        "${_qt_bin_dir}/Qt6Core.dll"
        "${_qt_bin_dir}/Qt6Gui.dll"
        "${_qt_bin_dir}/Qt6Widgets.dll"
        "${_qt_bin_dir}/Qt6Network.dll"
        DESTINATION bin
    )
    
    # 复制MinGW运行库
    install(FILES
        "${_qt_bin_dir}/../../mingw_64/bin/libgcc_s_seh-1.dll"
        "${_qt_bin_dir}/../../mingw_64/bin/libstdc++-6.dll"
        "${_qt_bin_dir}/../../mingw_64/bin/libwinpthread-1.dll"
        DESTINATION bin
    )
    
    # 添加TLS和网络插件
    install(DIRECTORY
        "${_qt_root_dir}/plugins/tls"
        "${_qt_root_dir}/plugins/networkinformation"
        DESTINATION bin
    )
    
    # 复制Qt插件
    install(DIRECTORY
        "${_qt_root_dir}/plugins/platforms"
        "${_qt_root_dir}/plugins/imageformats"
        "${_qt_root_dir}/plugins/styles"
        DESTINATION bin
    )
    
    # 检查并复制VCPKG的OpenSSL库
    if(EXISTS "${VCPKG_DIR}/bin/libssl.dll")
        file(GLOB VCPKG_SSL_LIBS "${VCPKG_DIR}/bin/libssl*.dll" "${VCPKG_DIR}/bin/libcrypto*.dll" 
                                 "${VCPKG_DIR}/bin/ssl*.dll" "${VCPKG_DIR}/bin/crypto*.dll")
        install(FILES ${VCPKG_SSL_LIBS} DESTINATION bin)
        message(STATUS "将从VCPKG安装OpenSSL库: ${VCPKG_SSL_LIBS}")
    # 检查并复制Qt目录的OpenSSL库
    elseif(EXISTS "${_qt_bin_dir}/libssl-3-x64.dll")
        install(FILES
            "${_qt_bin_dir}/libssl-3-x64.dll"
            "${_qt_bin_dir}/libcrypto-3-x64.dll"
            DESTINATION bin
        )
        message(STATUS "将安装OpenSSL 3.x库")
    elseif(EXISTS "${_qt_bin_dir}/libssl-1_1-x64.dll")
        install(FILES
            "${_qt_bin_dir}/libssl-1_1-x64.dll"
            "${_qt_bin_dir}/libcrypto-1_1-x64.dll"
            DESTINATION bin
        )
        message(STATUS "将安装OpenSSL 1.1.x库")
    elseif(EXISTS "${_qt_bin_dir}/../../mingw_64/bin/libssl-3-x64.dll")
        install(FILES
            "${_qt_bin_dir}/../../mingw_64/bin/libssl-3-x64.dll"
            "${_qt_bin_dir}/../../mingw_64/bin/libcrypto-3-x64.dll"
            DESTINATION bin
        )
        message(STATUS "将从MinGW目录安装OpenSSL 3.x库")
    elseif(EXISTS "${_qt_bin_dir}/../../mingw_64/bin/libssl-1_1-x64.dll")
        install(FILES
            "${_qt_bin_dir}/../../mingw_64/bin/libssl-1_1-x64.dll"
            "${_qt_bin_dir}/../../mingw_64/bin/libcrypto-1_1-x64.dll"
            DESTINATION bin
        )
        message(STATUS "将从MinGW目录安装OpenSSL 1.1.x库")
    else()
        message(WARNING "未找到OpenSSL库，TLS功能可能不可用")
    endif()
    
    # 检查并复制ICU库
    if(EXISTS "${_qt_bin_dir}/icudt*.dll")
        file(GLOB ICU_DLLS "${_qt_bin_dir}/icudt*.dll" "${_qt_bin_dir}/icuin*.dll" "${_qt_bin_dir}/icuuc*.dll")
        install(FILES ${ICU_DLLS} DESTINATION bin)
        message(STATUS "将安装ICU库")
    endif()
    
    # 创建qt.conf文件
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" "[Paths]\nPlugins = ./\n")
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" DESTINATION bin)
    
    # 安装说明
    message(STATUS "安装时将复制Qt库和插件到目标目录")
endif()

# 为可执行文件添加链接选项（移除静态链接，避免兼容性问题）
# if(WIN32)
#     target_link_libraries(IntegerSwap PRIVATE -static-libgcc -static-libstdc++)
# endif() 