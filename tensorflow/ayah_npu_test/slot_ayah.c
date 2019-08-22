#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "gxdnn.h"
#include "cJSON.h"
#define N 1000
#define MAX_LINE 1024  

#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 

const char *model_name = "/data/slot.npu";
GxDnnResult ret;
GxDnnDevice device;
GxDnnTask task;
int priority = 5;
GxDnnEventHandler event_handler = NULL;
int input_num, output_num;
int input_size, output_size;
GxDnnIOInfo *input, *output;
int i;
float image_buf[200] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 449, 996, 11};
float intent_buf[1] = {10};
float slot_name_buf[1] = {0};
float *result;
int result_num;
int max_index;
float max_value;

struct timeval tv;
int startTime;
int endTime;


int main(int argc, const char *argv[])
{
	
	//打印时间戳
    gettimeofday(&tv,NULL);
	startTime = tv.tv_sec*1000 + tv.tv_usec/1000;
    printf("NLP start :%d\n",startTime);  //毫秒

	//逐行读取
	// char buf[MAX_LINE];  /*缓冲区*/  
 	// FILE *fp;            /*文件指针*/  
 	// int len;             /*行字符个数*/  
  
 	// if((fp = fopen("/data/vector.txt","r")) == NULL)  
 	// {  
 	// 	perror("fail to read");
	// 	printf("fail to read");  
 	// 	exit (1) ;  
 	// }else
	//  {
	// 	//int err = remove("/data/vector.txt");
	// 	//printf("err = %d",err);
	//  }
	   
  
	// int j=0;
 	// while(fgets(buf,MAX_LINE,fp) != NULL)  
 	// {  
    //     len = strlen(buf);  
 	// 	buf[len-1] = '\0';  /*去掉换行符*/  
 	// 	//printf("%s %d \n",buf,len - 1);
	// 	image_buf[j] = atof(buf);
	// 	j++;
 	// }
    

	// 打开NPU设备
	GxDnnOpenDevice("/dev/gxnpu", &device);
    printf("111111111");

	// 传入模型文件，得到模型task
	ret = GxDnnCreateTaskFromFile(device, model_name, &task);
    printf("2222222222");
	// 获取task的输入输出信息
	GxDnnGetTaskIONum(task, &input_num, &output_num);
	input_size = input_num * sizeof(GxDnnIOInfo);
	output_size = output_num * sizeof(GxDnnIOInfo);
	printf("3333333333");
	input = malloc(input_size);
	output = malloc(output_size);
	if (input == NULL || output == NULL) {
		printf("Error: malloc fail.\n");
		return -1;
	}
	printf("input_size = %d , output_size = %d",input_size,output_size);
	GxDnnGetTaskIOInfo(task, input, input_size, output, output_size);

	// 拷贝输入数据到模型内存中
	memcpy(input[0].dataBuffer, (void*)image_buf, input[0].bufferSize);
    memcpy(input[0].dataBuffer, (void*)intent_buf, input[0].bufferSize);
    memcpy(input[0].dataBuffer, (void*)slot_name_buf, input[0].bufferSize);

	// 用同步方式运行模型
	GxDnnRunTask(task, priority, event_handler, NULL);
	result = output[0].dataBuffer;
	result_num = output[0].bufferSize/sizeof(float);
	max_value = -999999.99;
	for (i=0; i<result_num; i++) {
		printf("\nResult number: %d\n", i);
		printf("\nResult: %f\n", result[i]);
		if (result[i] > max_value) {
			max_value = result[i];
			max_index = i;
		}
	}
	printf("\nAyah Intent : %d\n", max_index);
	printf("\nIntent Score: %f\n", result[max_index]);

	//getIntent(max_index);
	// FILE *fp1;
    // char str[N+1];
    // if( (fp1=fopen("/data/index2intent.json","rt")) == NULL ){
    //     puts("Fail to open file!");
    //     exit(0);
    // }
   
    // while(fgets(str, N, fp1) != NULL){
    //     //printf("%s", str);
    // }
    // fclose(fp1);

    // //从缓冲区中解析出JSON结构
    // cJSON * json= cJSON_Parse(str);
    
    // //将传入的JSON结构转化为字符串 并打印
    // char *json_data = NULL;
    // //printf("data:%s\n",json_data = cJSON_Print(json));
    
    // char ss[100];
    // sprintf(ss,"%d",max_index);
	
    // cJSON *intent = cJSON_GetObjectItem(json,ss);
    // printf("Ayah intent is :%s\n",intent->valuestring);

	//打印时间戳
	gettimeofday(&tv,NULL);
	endTime = tv.tv_sec*1000 + tv.tv_usec/1000;
    printf("NLP End :%d\n",endTime);  //毫秒
	printf("NLP elapsed time :%d\n",endTime - startTime);

	//intent存文件

	// FILE* fpIntent;
	// fpIntent = fopen("/data/intent.txt","wb");
	// fwrite(ss,sizeof(ss),1,fpIntent);
	// fclose(fpIntent);

    // free(json_data);
    // //将JSON结构所占用的数据空间释放
    // cJSON_Delete(json);

	// 释放模型task
	GxDnnReleaseTask(task);

	// 关闭NPU设备
	GxDnnCloseDevice(device);

	free(input);
	free(output);
	return 0;
}