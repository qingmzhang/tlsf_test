cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 99)

project(tlsf-test)
# 设置构建类型为 Debug
set(CMAKE_BUILD_TYPE Debug)

# 添加调试信息选项
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# 添加源文件目录
add_subdirectory(src)
find_package(SDL2 REQUIRED)

# 如果找到了SDL2，包含SDL2的头文件目录
if(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
    message(STATUS "SDL2_VERSION: ${SDL2_VERSION}")
else()
    message(FATAL_ERROR "SDL2 not found")
endif()
# 添加测试可执行文件
add_executable(test_tlsf test/test_tlsf.c)
add_executable(test_sdl test/test_sdl.c)
# 链接 tlsf 库
target_link_libraries(test_tlsf tlsf)
target_link_libraries(test_sdl ${SDL2_LIBRARIES})
