#include "fd_share_memory.h"
#include "cJSON.h"
#include "fd_npu.h"
#include <pthread.h>

#define DATA_LENGTH 1024

using namespace std;
fd_npu* fd_nlu;
std::string data;
CShareMemory *csm;

void callNlu(string asr){
	string action = "";

	action = fd_nlu->handleNlpResult(asr);

	fd_nlu->clearDialog();
	cout << "action:" << action << endl;
}

void *PthreadDataProcessing(void* args){

	while (1)
	{
		usleep(1000);
		cJSON *root = cJSON_Parse(data.c_str());
		if (root == NULL)
    	{
        	cJSON_Delete(root);
			csm->PutToShareMem((u8*)"{}",DATA_LENGTH);
			cout << "root 为空" << endl;
			sleep(1);
        	continue;
    	}
		
		//消息json为空
		if (root->child==0)
		{
			cJSON_Delete(root);
			continue;
		}
		
		//收到asr
    	cJSON *asrValue = cJSON_GetObjectItem(root,"asr");
		if (asrValue!=NULL)
		{
			cout << "asr = " << asrValue->valuestring << endl;
			cJSON_DeleteItemFromObject(root, "asr");
			callNlu(asrValue->valuestring);
		}
		//收到dialog_id
		cJSON *dialogIdValue = cJSON_GetObjectItem(root,"dialog_id");
		if (dialogIdValue!=NULL)
		{
			cout << "dialogIdValue = " << dialogIdValue->valuestring << endl;
			cJSON_DeleteItemFromObject(root, "dialog_id");
		}
		//收到唤醒信号
		cJSON *kwsValue = cJSON_GetObjectItem(root,"kws");
		if (kwsValue!=NULL)
		{
			cout << "kws = " << kwsValue->valuestring << endl;
			cJSON_DeleteItemFromObject(root, "kws");
		}
		//收到senseflow状态
		cJSON *statusValue = cJSON_GetObjectItem(root,"status");
		if (statusValue!=NULL)
		{
			cout << "status = " << statusValue->valuestring << endl;
			cJSON_DeleteItemFromObject(root, "status");
		}
		char *newJson = cJSON_Print(root);
		csm->PutToShareMem((u8*)newJson,DATA_LENGTH);
		cJSON_Delete(root);
	}
	
	pthread_exit(NULL);
}

int main()
{
	unsigned char buffer[DATA_LENGTH] = {0};
	unsigned int length = sizeof(buffer);
		
	csm = new CShareMemory("fd", length);
	fd_nlu = fd_npu::_GetInstance("/data/found/model/");
	pthread_t pthread_id;
	pthread_create(&pthread_id,NULL,PthreadDataProcessing,NULL);
 
	while(1)
	{
		usleep(1000 * 1);
		csm->GetFromShareMem(buffer,length);
		//cout << (char*)buffer << endl;
		data = (char*)buffer;
	}
	free(csm);
	return 0;
}