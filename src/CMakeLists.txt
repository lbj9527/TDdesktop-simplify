# 收集源文件
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE UI_FILES "*.ui")

# 显示找到的文件
message(STATUS "找到的源文件: ${SOURCES}")
message(STATUS "找到的头文件: ${HEADERS}")
message(STATUS "找到的UI文件: ${UI_FILES}")

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES})

# 链接Qt6库和OpenSSL库
message(STATUS "链接Qt6库")
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

# 链接OpenSSL库
if(EXISTS "${VCPKG_DIR}/lib/libssl.lib")
    message(STATUS "链接VCPKG中的OpenSSL库")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "${VCPKG_DIR}/lib/libssl.lib"
        "${VCPKG_DIR}/lib/libcrypto.lib"
    )
else()
    message(WARNING "未找到VCPKG中的OpenSSL库")
endif()

# 明确指定C++17标准
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# 设置可执行文件的属性
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# 为可执行文件添加编译定义和标志
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<BOOL:${WIN32}>:UNICODE>
    $<$<BOOL:${WIN32}>:_UNICODE>
    $<$<BOOL:${WIN32}>:WIN32>
    $<$<BOOL:${WIN32}>:_WINDOWS>
)

# 添加额外的编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wno-deprecated-declarations
        -Wno-class-memaccess
        -fpermissive
        -Wno-missing-template-arg-list-after-template-kw
    )
endif()

# 安装目标
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib) 