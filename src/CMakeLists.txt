# 收集源文件
file(GLOB_RECURSE SOURCES "*.cpp")

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 设置可执行文件的属性
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
)

# 为可执行文件添加链接选项(如果需要)
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

# 安装目标
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib) 