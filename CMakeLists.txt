cmake_minimum_required(VERSION 4.0)
project(chat)

set(CMAKE_CXX_STANDARD 17)
# 最后输出文件的目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/db)

add_subdirectory(src)