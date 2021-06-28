#ifndef FD_SLOT_NPU_H
#define FD_SLOT_NPU_H

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

#define SLOT_NPU_PATH       "/data/found/model/slot.npu"
#define SLOT_DEVICE_PATH    "/dev/gxnpu"

class FdSlotNpu
{
private:
    FdSlotNpu();
    
    GxDnnResult ret;
    GxDnnDevice device;
    GxDnnTask task;

    int input_size, output_size;
    GxDnnIOInfo *input, *output;

    FdResource *resource;

public:
    ~FdSlotNpu();

    static FdSlotNpu *_GetInstance()
    {
        static FdSlotNpu instance;
        return &instance;
    }
    
    int executeSlotNpu(const void *query, const void *intent_index, const void *slot_index, int &s_index, int &e_index);

};


#endif //FD_SLOT_NPU_H