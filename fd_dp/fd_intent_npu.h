#ifndef FD_INTENT_NPU_H
#define FD_INTENT_NPU_H

#include <assert.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <cstring>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdlib.h>
#include <sstream>
#include <sys/time.h>
#include "gxdnn.h"
#include "cJSON.h"
#include "fd_log.h"
#include "fd_resource.h"

#define INTENT_NPU_PATH    "/data/found/model/intent.npu"
#define INTENT_DEVICE_PATH "/dev/gxnpu"

class FdIntentNpu
{
private:
    FdIntentNpu();
    
    GxDnnResult ret;
    GxDnnDevice device;
    GxDnnTask task;

    int input_size, output_size;
    GxDnnIOInfo *input, *output;

    FdResource *resource;

public:
    ~FdIntentNpu();

    static FdIntentNpu *_GetInstance()
    {
        static FdIntentNpu instance;
        return &instance;
    }

    int executeNpu(const void *query, float &score);
};


#endif //FD_INTENT_NPU_H