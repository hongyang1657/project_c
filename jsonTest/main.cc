#include <stdio.h>
#include <iostream>
#include "cJSON.h"
#include <fstream>
#include <string>
#include <string.h>
#include <map>


using namespace std;
std::map<int, std::string> intentConfigMap;

int main(){
    
    int ayah_intent;
    string intentName;
    string intentJson = "{\"intent_index\": 3,\"intent_name\": \"科嘉集成灶_二档风\"}";
    //解析json
    cJSON *pIntentRoot = cJSON_Parse(intentJson.c_str());
    cJSON *pIntentIndex = cJSON_GetObjectItem(pIntentRoot, "intent_index");         // 解析子节点pValue的day字段字符串内容
    cJSON *pIntentName = cJSON_GetObjectItem(pIntentRoot, "intent_name");
    ayah_intent = pIntentIndex->valueint;
    intentName = pIntentName->valuestring;
    cJSON_Delete(pIntentRoot);

    printf("IntentIndex = %d,IntentName = %s\n",ayah_intent,intentName);
}