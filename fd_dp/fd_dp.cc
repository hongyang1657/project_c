#include "fd_dp.h"
#include "fd_log.h"
#include "cJSON.h"
#include "events2chars.h"
#include "Tool.h"
#include "serialManager.h"

using namespace std;

struct timeval tvIntent;
int startTime,endTime;
SerialManager *serialManager;


int main(){
    serialManager = SerialManager::getInstance();
    resource = FdResource::_GetInstance();
    config = FdConfig::_GetInstance();
    intentNpu = FdIntentNpu::_GetInstance();
    slotNpu = FdSlotNpu::_GetInstance();
    vector<std::string> list;
    list.push_back("{\"type\":\"u2a_speech\",\"speech\":\"我要睡觉了\",\"intent\":null,\"request_slots\":{},\"response_slots\":{}}");
    sleep(5);
    while (true)
    {
        sleep(5);

        _RecvEventsList(list);
    }
    
}

void _RecvEventsList(const std::vector<std::string> &events_list) {
    FD_LOG_L("接收到 events");

    if (!config->_intent_model) {
        return;
    }    

    float score;

    intentEmbeddingEventList(events_list);
    slotEmbeddingEventList(events_list);

    // FD_DEBUG_L("请求参数列表");
    // for (int i = 0; i < ROW_SIZE; i++)
    // {
    //     cout << query_index[i] << " ";
    // }
    // cout << endl;
    
    //打印intent预测耗时
    gettimeofday(&tvIntent,NULL);
	startTime = tvIntent.tv_sec*1000+tvIntent.tv_usec/1000;
    int intent_index = intentNpu->executeNpu((void *)query, score);
    gettimeofday(&tvIntent,NULL);
	endTime = tvIntent.tv_sec*1000+tvIntent.tv_usec/1000;
    cout<<"intent预测耗时="<<endTime-startTime<<endl;

    string intent_str = resource->getIntentWithIndex(intent_index);
    FD_LOG_L(intent_str);
    string type = resource->getTypeWithIntent(intent_str);
    
    // if (intent_str.length() && !intent_str.compare("回应聊天")) {
    //     FD_LOG_L("回应聊天");
    //     return;
    // }

    FD_LOG_L("00000000000");

    cJSON *actionJson = cJSON_CreateObject();
    
    string intentIndexStr = to_string(intent_index);

    cJSON_AddItemToObject(actionJson, "intent", cJSON_CreateString(intent_str.c_str()));
    cJSON_AddItemToObject(actionJson, "intent_index", cJSON_CreateString(intentIndexStr.c_str()));
    cJSON_AddItemToObject(actionJson, "type", cJSON_CreateString(type.c_str()));
    cJSON_AddItemToObject(actionJson, "score", cJSON_CreateNumber(score));

    cJSON *slots = cJSON_CreateObject();
    if (config->_slot_model)
    {
        printf("11111111111%s\n",intent_str.c_str());
        vector<string> slot_names = checkoutSlotNameWithIntent(intent_str);
        printf("22222222222 %d\n",slot_names.size());
        for (int i = 0; i < slot_names.size(); i++)
        {
            string slot_name = slot_names[i];
            string slot_value = "";
            int ret = 0;
            ret = checkoutSlotWithNpu(intent_index, slot_name, slot_value);
            FD_LOG_L("33333333");
            if(ret != 0) {
                continue;
            } else {
                cJSON_AddItemToObject(slots, slot_name.c_str(), cJSON_CreateString(slot_value.c_str()));
            }
        }
        
        // cJSON_AddItemToObjectCS(actionJson, "slots", slots);
    }
    cJSON_AddItemToObject(actionJson, "slots", slots);

    char *actionChar = cJSON_Print(actionJson);
    string actionString = actionChar;
    free(actionChar);
    FD_LOG_L("444444444444");
    cJSON_Delete(actionJson);

    serialManager->sendSerialByZhiyuanAgreement(45,0);

    //SenseBus::Publish("action_json", actionString);
}

int checkoutSlotWithNpu(int intent_index, string &slot_name, string &slot_value) {
    //TODO 调用 slot 模型
    int slot_index = resource->getIntentIndexWithSlot(slot_name);
    int s_index, e_index;
    int length = 0;
    int ret = 0;
    slot_value = "";

    float *intent_indexs = resource->getIntentEmbeddingWithIndex(intent_index);
    float *slot_indexs = resource->getSlotEmbeddingWithIndex(slot_index);
    
    // FD_DEBUG_L("intent_index");
    // for (int i = 0; i < 50; i++)
    // {
    //     std::cout << intent_indexs[i] << " ";
    // }
    // std::cout << std::endl;

    // FD_DEBUG_L("slot_index");
    // for (int i = 0; i < 50; i++)
    // {
    //     std::cout << slot_indexs[i] << " ";
    // }
    // std::cout << std::endl;
    

    ret = slotNpu->executeSlotNpu((void *)query_slot, (void *)resource->getIntentEmbeddingWithIndex(intent_index), (void *)resource->getSlotEmbeddingWithIndex(slot_index), s_index, e_index);
    
    if(ret != 0) {
        return ret;
    }

    length = e_index - s_index;

    for (int j = s_index; j <= e_index && e_index != ROW_SIZE-1; j++)
    {
        //提取slot值
        // string _char = resource->index2char(query_index[j]);
        string _char = query_chars[j];
        if (!_char.length() || !_char.compare("[pad]")) {
            slot_value = "";
            break;
        }

        if (!includeChinese(_char) && j != e_index) {
            _char += " ";
        }
        
        
        
        slot_value += _char;
    }
    return ret;
}


void intentEmbeddingEventList(std::vector<std::string> event_list) {

    std::vector<std::string> char_list = events2chars(event_list, true);
    
    memset(query_index, 0, sizeof(query_index));
    query_str = "";

//    超长度截取
    while (char_list.size() > (ROW_SIZE)) {
        char_list.erase(char_list.begin());
    }
    
    int str_len = (int)char_list.size();
    int *result_arr = new int[ROW_SIZE];  //结果集
    
//    补前置空缺
    for (int i = 0; i < ROW_SIZE-str_len; i++) {
        result_arr[i] = 1;
        query_chars[i] = "[pad]";
    }
    
    for (int i = 0; i < char_list.size(); i++) {
        
        std::string char_str = char_list[i];
        query_str += char_str;

        int result = resource->char2index(char_str);
        
        query_chars[ROW_SIZE-str_len+i] = char_str;
        result_arr[ROW_SIZE-str_len+i] = result != -1 ? result : 0;
    }

    FD_LOG_L(query_str);

    // FD_DEBUG_L("intent 请求填充参数");
    for (int i = 0; i < ROW_SIZE; i++) {
        int embedding_index = result_arr[i];
        memset(query[i], 0, COLUMN_SIZE*sizeof(float));
        query_index[i] = embedding_index;
        
        float *temp = resource->getEmbeddingWithIndex(embedding_index);
        // query[i] = result_arr[i];
        for (int j = 0; j < 300; j++)
        {
            query[i][j] = temp[j];
            // std::cout << query[i][j] << " ";
        }
        // std::cout << std::endl;
    }
    // std::cout << std::endl;

    free(result_arr);
}

void slotEmbeddingEventList(std::vector<std::string> event_list) {
    
    std::vector<std::string> char_list = events2chars(event_list, true);

//    超长度截取
    while (char_list.size() > (ROW_SIZE)) {
        char_list.erase(char_list.begin());
    }
    
    int str_len = (int)char_list.size();
    int *result_arr = new int[ROW_SIZE];  //结果集
    
//    补前置空缺
    for (int i = 0; i < ROW_SIZE-str_len; i++) {
        result_arr[i] = 1;
    }
    
    for (int i = 0; i < char_list.size(); i++) {
        
        std::string char_str = char_list[i];

        int result = resource->char2index(char_str);
        
        result_arr[ROW_SIZE-str_len+i] = result != -1 ? result : 0;
    }

    // FD_DEBUG_L("slot 请求填充参数");
    for (int i = 0; i < ROW_SIZE; i++) {
        int embedding_index = result_arr[i];

        float *temp = resource->getVocabEmbeddingWithIndex(embedding_index);
        // query[i] = result_arr[i];
        for (int j = 0; j < COLUMN_SIZE; j++)
        {
            query_slot[i][j] = temp[j];
            // std::cout << query_slot[i][j] << " ";
        }
        // std::cout << std::endl;
    }

    free(result_arr);
}

vector<string> checkoutSlotNameWithIntent(std::string intent) {
    
    vector<string> slot_names;

    bool flag = false;
    string slot_name = "";

    for (int i = 0; i < intent.length(); i++)
    {
        if (intent[i] == '<')
        {
            flag = true;
        } else if (flag && intent[i] == '>') {
            flag = false;
            slot_names.push_back(slot_name);
            slot_name = "";
        } else if(flag) {
            slot_name += intent[i];
        }
        
    }

    return slot_names;
}