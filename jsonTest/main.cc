#include <stdio.h>
#include <iostream>
#include "cJSON.h"
#include <fstream>
#include <string>
#include <string.h>
#include <map>
#include <cstring>
#include <stdlib.h>
#include <math.h>


using namespace std;
struct Motor_Config
{
    std::map<int,int> is_control;
    std::map<int,std::string> control_type;
    std::map<int,int> data1;
    std::map<int,int> data2;
};

struct Serial_IO_Config{
        std::map<int, std::string> command_type;
        std::map<int, int> offset;
        std::map<int, int> multiple;
        std::map<int, Motor_Config> motor;
} serial_io_config;


void loadIntentConfigJson(std::string path);


int main(){
    int a = 2;
    double b = pow(a,4.0);
    cout<<"b = "<<b<<endl;
    loadIntentConfigJson("/home/hongy/C_C++/jsonTest/serial_IO_config.json");
    
    return 0;
}

//加载json配置文件
void loadIntentConfigJson(string path){
    cout<<"loadIntentConfigJson... "<<endl;
    char filename[30];
    strcpy(filename, path.c_str());
    ifstream intent_file;
    intent_file.open(filename);
    string line;
    string intent_data = "";
    // 有该文件
    if(intent_file) {
        while (getline (intent_file, line)) {
            intent_data += line;
        }
    } else {
        // 没有该文件
        cout <<"<no such file path = >" << path << std::endl;

    }
    intent_file.close();
    
    cJSON *intent_json = cJSON_Parse(intent_data.c_str());

    if (!intent_json)
    {
        cout << "intent_json =" <<endl;
    }
    
    
    std::cout << "size = "<<cJSON_GetArraySize(intent_json)<< std::endl;
    for(int i=0; i<cJSON_GetArraySize(intent_json); i++)   //遍历最外层json键值对
    {   
        cout<<"################i = "<<i<<endl;
        cJSON * item = cJSON_GetArrayItem(intent_json, i);
        std::string key = item->string;
        
        cJSON *config = cJSON_GetObjectItem(intent_json,item->string);

        cJSON *command_type = cJSON_GetObjectItem(config,"command_type");
        cJSON *multiple = cJSON_GetObjectItem(config,"multiple");
        cJSON *offset = cJSON_GetObjectItem(config,"offset");
        cJSON *motor = cJSON_GetObjectItem(config,"motor");

        cJSON *motor_item = motor->child;
        Motor_Config motor_config;
        for (int j = 0; j < cJSON_GetArraySize(motor); j++)
        {   
            cout<<"################j = "<<j<<endl;
            cJSON *isControl = cJSON_GetObjectItem(motor_item,"isControl");
            cJSON *control_type = cJSON_GetObjectItem(motor_item,"control_type");
            cJSON *data1 = cJSON_GetObjectItem(motor_item,"data1");
            cJSON *data2 = cJSON_GetObjectItem(motor_item,"data2");
  
            if (isControl!=NULL&&isControl->type!=cJSON_NULL)
            {   
                cout<<"########isControl######## "<<endl;
                motor_config.is_control[j] = isControl->valueint;
            }
            if (control_type!=NULL&&control_type->type!=cJSON_NULL)
            {   
                cout<<"########control_type######## "<<endl;
                motor_config.control_type[j] = control_type->valuestring;
            }
            if (data1!=NULL&&data1->type!=cJSON_NULL)
            {
                cout<<"########data1######## "<<endl;
                motor_config.data1[j] = data1->valueint;
            }
            if (data2!=NULL&&data2->type!=cJSON_NULL)
            {
                cout<<"########data2######## "<<endl;
                motor_config.data2[j] = data2->valueint;
            }    
            motor_item = motor_item->next;
        }
        serial_io_config.motor[atoi(key.c_str())] = motor_config;
    
        if (command_type!=NULL&&command_type->type!=cJSON_NULL)
        {   
            cout<<"########command_type######## "<<endl;
            serial_io_config.command_type[atoi(key.c_str())] = command_type->valuestring;
        }
        if (offset!=NULL&&offset->type!=cJSON_NULL)
        {
            cout<<"########offset######## "<<endl;
            serial_io_config.offset[atoi(key.c_str())] = offset->valueint;
        }
        if (multiple!=NULL&&multiple->type!=cJSON_NULL)
        {
            cout<<"########multiple######## "<<endl;
            serial_io_config.multiple[atoi(key.c_str())] = multiple->valueint;
        }
        

    }

    
    cJSON_Delete(intent_json);
    std::cout<<"loadIntentConfigJson success "<<std::endl;
}