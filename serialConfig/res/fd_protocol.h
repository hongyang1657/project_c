#ifndef FD_PROTOCOL_H
#define FD_PROTOCOL_H

#include <stdio.h>              // printf   
#include <fcntl.h>              // open   
#include <string.h>             // bzero   
#include <stdlib.h>             // exit   
#include <sys/times.h>          // times   
#include <sys/types.h>          // pid_t   
#include <termios.h>          //termios, tcgetattr(), tcsetattr()   
#include <unistd.h>   
#include <sys/ioctl.h>          // ioctl  
#include "cJSON.h"
#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <pthread.h>
#include <cstring>
#include <vector>
#include <cmath>
#include <math.h>
#include <signal.h>
#include "fd_gpio.h"

#define FD_PROTOCOL_REC_BUFF_LEN        11
#define FD_PROTOCOL_BASELINE            128
#define FD_PROTOCOL_BAUDRATE            38400

#define FD_PROTOCOL_N_SPEED         38400
#define FD_PROTOCOL_N_BITS          8
#define FD_PROTOCOL_N_EVENT         'N'
#define FD_PROTOCOL_N_STOP          1
//唤醒对应的串口协议为1001，阈值未达到对应1002
#define FD_PROTOCOL_WAKEUP              1001
#define FD_PROTOCOL_SCORE_NOT_ENOUGH    1002
#define FD_PROTOCOL_POWER_ON            1003

#define FD_PROTOCOL_SERIAL_DEV1         "/dev/ttyS0"

typedef void (*recvData)(char*,int); //回调方法

/**
 * 通用协议
 * */
class fd_protocol {
private:
    int fd;
    int ret;
    int n = 0;
    
    termios oldstdio;
    // char Rx_Data[FD_PROTOCOL_REC_BUFF_LEN];
    int RecvBuffLen = 10;

    static void * Pthread_Serial(void *arg );
    fd_protocol();
    ~fd_protocol();
    recvData recvSerial = NULL;
public:
    

    static fd_protocol *_GetInstance()
    {
        static fd_protocol instance;
        return &instance;
    }

    int S_fd = 0;
    struct timeval tv1;
    int pollingTimes = 0;
    int pollingSencond = 0;
    
    int init_protocol(int recv_len = FD_PROTOCOL_REC_BUFF_LEN, int nSpeed = FD_PROTOCOL_N_SPEED, int nBits = FD_PROTOCOL_N_BITS, char nEvent = FD_PROTOCOL_N_EVENT, int nStop = FD_PROTOCOL_N_STOP);
    int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop);
    int serial_send(const char* data);
    int createSerialThread();
    void setDataRecvListener(void (*recvData)(char*,int));
    
};



#endif //FD_PROTOCOL_H