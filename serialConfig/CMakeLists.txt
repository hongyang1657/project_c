cmake_minimum_required(VERSION 3.5.1)

project(SerialConfig)

add_compile_options(-std=c++11 -w)

#交叉编译选项
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin/)

include_directories(
    ${PROJECT_SOURCE_DIR}/res
    ${PROJECT_SOURCE_DIR}/include
)

file(
    GLOB SRC_LIST
    ${PROJECT_SOURCE_DIR}/res/*.cc
    ${PROJECT_SOURCE_DIR}/res/*.c
)


#link_directories(${PROJECT_SOURCE_DIR}/lib)

#生成应用程序fd_demo
add_executable(${PROJECT_NAME} ${SRC_LIST})

#连接库
target_link_libraries(${PROJECT_NAME}
    pthread dl
)

#对目标程序执行shell命令
#PRE_BUILD - 命令将会在其他依赖项执行前执行
#PRE_LINK - 命令将会在其他依赖项执行完后执行
#POST_BUILD - 命令将会在目标构建完后执行。
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    #将生成的可执行程序利用strip压缩

    COMMAND "/usr/bin/arm-linux-gnueabihf-strip" ${PROJECT_SOURCE_DIR}/build/bin/${PROJECT_NAME}
    #将生成的可执行程序cp到production文件夹
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/build/bin/${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/production/
)