#include "fd_slot_npu.h"
#include "Tool.h"

FdSlotNpu::FdSlotNpu() {
    FD_LOG_L("开始加载 SLOT 模型");

    GxDnnOpenDevice(SLOT_DEVICE_PATH, &device);
    ret = GxDnnCreateTaskFromFile(device, SLOT_NPU_PATH, &task);
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

    FD_LOG_L("SLOT 模型加载完成");
}
FdSlotNpu::~FdSlotNpu() {
}

int FdSlotNpu::executeSlotNpu(const void *query, const void *intent_index, const void *slot_index, int &s_index, int &e_index) {

	FD_LOG_L("query=");
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
    memcpy(input[1].dataBuffer, intent_index, input[1].bufferSize);
    memcpy(input[2].dataBuffer, slot_index, input[2].bufferSize);

    GxDnnRunTask(task, priority, event_handler, NULL);

	/****************************************/
	/*			提取slot模型结果			  */
	/****************************************/

    float *result_slot_s, *result_slot_e;
	result_slot_s = (float*)output[0].dataBuffer;
	result_slot_e = (float*)output[1].dataBuffer;

	int result_slot_s_num, result_slot_e_num;
	result_slot_s_num = output[0].bufferSize/sizeof(float);
	result_slot_e_num = output[1].bufferSize/sizeof(float);

	int result_slot_s_index[5], result_slot_e_index[5];
	float slot_s_max [5] = {-99.9,-99.9,-99.9,-99.9,-99.9}, slot_e_max[5] = {-99.9,-99.9,-99.9,-99.9,-99.9};

	float result_slot[25];
	float result_slot_index[25];

	int start_index, end_index;
	float start_max = -99.9, end_max = -99.9;

    // FD_DEBUG_L("slot start 所有评分");
	float slot_start_sum = 0;
	for (int i = 0; i < result_slot_s_num; i++)
	{	
		float temp = result_slot_s[i];
		// float temp = softMax(result_slot_s, result_slot_s[i], result_slot_s_num);
        int temp_index = i;

        // std::cout << temp << " ";
		slot_start_sum += temp;
		for (int j = 0; j < 5; j++)
		{
			if (temp > slot_s_max[j])
			{
                float t = temp;
                int t_i = temp_index;

				temp = slot_s_max[j];
				slot_s_max[j] = t;

                temp_index = result_slot_s_index[j];
				result_slot_s_index[j] = t_i;
			}
			
		}		
	}    
    // std::cout << std::endl;

    // FD_DEBUG_L("提取 slot start 最高信任度");
    // for (int i = 0; i < 5; i++)
    // {
    //     std::cout << slot_s_max[i] << " ";
    // }
    // std::cout << std::endl;
    
    // FD_DEBUG_L("slot end 所有评分");
	float slot_end_sum = 0;
	for (int i = 0; i < result_slot_e_num; i++)
	{	
		float temp = result_slot_e[i];
		// float temp = softMax(result_slot_e, result_slot_e[i], result_slot_e_num);
        int temp_index = i;

        // std::cout << temp << " ";
		slot_end_sum += temp;
		for (int j = 0; j < 5; j++)
		{
			if (temp > slot_e_max[j])
			{
                float t = temp;
                int t_i = temp_index;

				temp = slot_e_max[j];
				slot_e_max[j] = t;

                temp_index = result_slot_e_index[j];
				result_slot_e_index[j] = t_i;
			}
			
		}
	}
    // std::cout << std::endl;

    // FD_DEBUG_L("提取 slot end 最高信任度");
    // for (int i = 0; i < 5; i++)
    // {
    //     std::cout << slot_e_max[i] << " ";
    // }
    // std::cout << std::endl;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			result_slot[i*5 + j] = slot_s_max[i] + slot_e_max[j];
			result_slot_index[i*5 + j] = i*5 + j;
            // std::cout << result_slot[i*5+j] << " ";
		}
		
	}
	// std::cout << std::endl;

	for (int i = 0; i < 23; i++)
	{

		for (int j = 0; j < 24-i; j++)
		{
			if (result_slot[j] < result_slot[j + 1])
			{
				float temp = result_slot[j];
				result_slot[j] = result_slot[j + 1];
				result_slot[j + 1] = temp;

				int temp2 = result_slot_index[j];
				result_slot_index[j] = result_slot_index[j + 1];
				result_slot_index[j + 1] = temp2;
			}
			
		}
		
	}
	
    // FD_DEBUG_L("匹配 slot 下标");
	for (int i = 0; i < 25; i++)
	{
		int temp = result_slot_index[i];
		start_index = result_slot_s_index[temp/5];
		end_index = result_slot_e_index[temp%5];

        // std::cout << result_slot[i] << " " << start_index << " : " << end_index << std::endl;
		if (start_index <= end_index)
		{
			end_index = end_index == output_size-1? end_index-1: end_index;
			break;
		}
		
	}

    s_index = start_index;
    e_index = end_index;

    return 0;
}