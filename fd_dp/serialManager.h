#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

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

//唤醒对应的串口协议为1001，阈值未达到对应1002
#define WAKEUP 1001
#define SCORE_NOT_ENOUGH 1002

#define REC_BUFF_LEN 32
#define BASELINE 128


class SerialManager{
private:
    SerialManager();
    static SerialManager *serialmanager;
    
    int loadIntentConfigJson(std::string path);

    int generateAyahProtocol(int);
    static void char2Int(char[]);
    static void recSerialIntent();
    int hex2dec(std::string s);
    int judgeAggrement(int,int,int,int,int);

public:
    ~SerialManager();
    static SerialManager *getInstance();

    

    int initManager();  
    static int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop);
    int sendHeartbeat();
    int sendSerialByZhiyuanAgreement(int,int);
    int sendSerialByYiweiAgreement(int);
    int sendSerialByAYAHAgreement(int);
    int executeGpio(int intent);
    int getRecSerialIntent();
    int createSerialThread();
    static void * Pthread_Serial(void *arg );
    int serial_send(const char* data);
    void getSerialInput();
};

#endif
