#ifndef FD_GPIO_H
#define FD_GPIO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <pthread.h>

using namespace std;


class FdGPIO{
private:
    static FdGPIO *fdGPIO;
    FdGPIO();
    

public:
    ~FdGPIO();
    static FdGPIO *getInstance();
    static int control(string pinVal,string rstVal);
    static int readGpioPort(string pinVal);
    static void * PthreadGpio(void *arg);
    int gpioScriptTask(string);
};

#endif
