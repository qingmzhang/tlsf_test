cmake_minimum_required(VERSION 3.10)

project(tlsf-test)
# 设置构建类型为 Debug
set(CMAKE_BUILD_TYPE Debug)

# 添加调试信息选项
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# 添加源文件目录
add_subdirectory(src)

# 添加测试可执行文件
add_executable(test_tlsf test/test_tlsf.c)
# 链接 tlsf 库
target_link_libraries(test_tlsf tlsf)