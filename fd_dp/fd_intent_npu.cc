#include "fd_intent_npu.h"
#include "Tool.h"

using namespace std;

FdIntentNpu::FdIntentNpu()
{
    FD_LOG_L("开始加载模型");

    GxDnnOpenDevice(INTENT_DEVICE_PATH, &device);
    ret = GxDnnCreateTaskFromFile(device, INTENT_NPU_PATH, &task);
    if (ret != GXDNN_RESULT_SUCCESS)
    {
        FD_DEBUG_L("Error: load model fail");
        return;
    }

    int input_num, output_num;
    GxDnnGetTaskIONum(task, &input_num, &output_num);

    input_size = input_num * sizeof(struct NpuIOInfo);
    output_size = output_num * sizeof(struct NpuIOInfo);
    
    input = (NpuIOInfo *)malloc(input_size);
    output = (NpuIOInfo *)malloc(output_size);

    resource = FdResource::_GetInstance();

    FD_LOG_L("模型加载完成");
}

FdIntentNpu::~FdIntentNpu()
{
}

int FdIntentNpu::executeNpu(const void *query, float &score) {
    if (input == NULL || output == NULL)
    {
        FD_DEBUG_L("Error: malloc fail");
        return -1;
    }

    if (ret != GXDNN_RESULT_SUCCESS)
    {
        FD_DEBUG_L("Error: load model fail");
        return -1;
    }

    GxDnnEventHandler event_handler = NULL;
    int priority = 5;
    
    
    memset(input, 0, input_size);
    memset(output, 0, output_size);

    GxDnnGetTaskIOInfo(task, input, input_size, output, output_size);

    memcpy(input[0].dataBuffer, query, input[0].bufferSize);
    GxDnnRunTask(task, priority, event_handler, NULL);

        
    float *result = (float *)output[0].dataBuffer;
    int result_num = output[0].bufferSize/sizeof(float);
    float max_value = -999999.99;
    int max_index;

    for (int i=0; i<result_num; i++) {
        if (result[i] > max_value) {
            max_value = result[i];
            max_index = i;
        }
    }

    score = softMax(result, result_num);  

    return max_index;
}