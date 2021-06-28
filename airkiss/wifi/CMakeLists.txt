cmake_minimum_required(VERSION 2.8)
project(net_config)

set(CMAKE_C_COMPILER "arm-openwrt-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER "arm-openwrt-linux-gnueabi-g++")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fPIC -Wall -Wextra -O2 -lpthread -lm")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -Wnon-virtual-dtor -std=c++11 -O2")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -Wnon-virtual-dtor -std=c++11 -O0 -g")

file(GLOB SF_COMPONENTS_SRC
    ./src/api_net_config.c
)

option(NETWORK_HAVE_AIRKISS "having airkiss" ON)
if(NETWORK_HAVE_AIRKISS)
    add_definitions("-DHAVE_AIRKISS")
    file(GLOB SF_COMPONENTS_SRC ${SF_COMPONENTS_SRC}
        ./src/airkiss_src.c
        ./capture/common.c
        ./capture/osdep.c
        ./capture/linux.c
        ./capture/radiotap/radiotap-parser.c
        ./utils/wifi_scan.c
    )
    include_directories(
        ${CMAKE_SOURCE_DIR}/include
    )
    link_directories(
        ${CMAKE_SOURCE_DIR}/lib
    )
endif(NETWORK_HAVE_AIRKISS)

option(NETWORK_HAVE_SOUNDWAVE "having sound wave" OFF)
if(NETWORK_HAVE_SOUNDWAVE)
    add_definitions("-DHAVE_SOUNDWAVE")
    file(GLOB SF_COMPONENTS_SRC ${SF_COMPONENTS_SRC}
        ./src/wav_src.c
        ./src/kfifo.c
    )
    include_directories(
        ${CMAKE_SOURCE_DIR}/branch_16K/include
    )
    link_directories(
        ${CMAKE_SOURCE_DIR}/branch_16K/lib
    )
endif(NETWORK_HAVE_SOUNDWAVE)

option(NETWORK_HAVE_BLE "having ble" OFF)
if(NETWORK_HAVE_BLE)
    add_definitions("-DHAVE_BLE")
    file(GLOB SF_COMPONENTS_SRC ${SF_COMPONENTS_SRC}
        ./src/bt_src.c 
    )
    include_directories(
        ${CMAKE_SOURCE_DIR}/include
    )
    link_directories(
        ${CMAKE_SOURCE_DIR}/lib
    )
endif(NETWORK_HAVE_BLE)

add_library(${PROJECT_NAME} STATIC ${SF_COMPONENTS_SRC})
#add_executable(net_config ${SF_COMPONENTS_SRC})
#add_definitions("-DAPP_TEST")

if(NETWORK_HAVE_SOUNDWAVE)
    target_link_libraries(${PROJECT_NAME} ADT)
endif(NETWORK_HAVE_SOUNDWAVE)

if(NETWORK_HAVE_AIRKISS)
    target_link_libraries(${PROJECT_NAME} airkiss iw)
endif(NETWORK_HAVE_AIRKISS)

if(NETWORK_HAVE_BLE)
    target_link_libraries(${PROJECT_NAME} rokid-bt)
endif(NETWORK_HAVE_BLE)

