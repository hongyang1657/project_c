cmake_minimum_required(VERSION 2.8)

project(DP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -Wnon-virtual-dtor -O2")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fPIC -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -L. -lgxdnn -lpthread -lm")

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)
# 指定生成目标
add_executable(DP ${DIR_SRCS})
