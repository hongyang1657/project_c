#include "fd_resource.h"
#include <fstream>
#include "fd_log.h"
#include "cJSON.h"
#include <time.h>
#include <string.h>

FdResource::FdResource() {
    if (config->_intent_model) {
        set_embedding_for_file(EMBEDDING_PATH);
        set_char_index_for_file(CHAR_2_INDEX_PATH);

        int r = set_model_config_for_file(MODEL_CONFIG_PATH);

        if (r != 0) {
            set_index_intent_for_file(INDEX_2_INTENT_PATH);
            set_intent_score_dic_for_file(INTENT_SCORE_PATH);
            set_intent_type_dic_for_file(INTENT_2_TYPE_PATH);
        }
    }
    
    if (config->_slot_model) {
        set_vocab_embedding_for_file(VOCAB_EMBEDDING_PATH);
        set_slot_embedding_for_file(SLOT_EMBEDDING_PATH);
        set_intent_embedding_for_file(INTENT_EMBEDDING_PATH);
        set_slot_index_dic_for_file(SLOT_2_INDEX_PATH);
        set_index_slot_dic_for_file(INDEX_2_SLOT_PATH);
    }

    set_tts_url_for_file(TTS_URL_PATH);
}

FdResource::~FdResource() {
}

/**************************************/
/*            加载 embedding           */
/**************************************/
int FdResource::set_embedding_for_file(std::string path) {
    time_t start = time(NULL);
    std::ifstream file(path, std::ios::binary);

    if (file)
    {
        file.read((char *)embeddings, sizeof(embeddings));
    }
    else
    {
        FD_DEBUG_L("<no such file path = " + path + ">");
        file.close();
        return -1;
    }
    file.close();

    time_t end = time(NULL);

    FD_LOG_L("embedding 结束加载");
    return 0;
}

int FdResource::set_vocab_embedding_for_file(std::string path) {
    std::ifstream file(path, std::ios::binary);

    if (file)
    {
        file.read((char *)vocab_embedding, sizeof(vocab_embedding));
    }
    else
    {
        FD_DEBUG_L("<no such file path = " + path + ">");
        file.close();
        return -1;
    }
    file.close();

    time_t end = time(NULL);

    FD_LOG_L("vocab embedding 结束加载");
    return 0;
}

int FdResource::set_slot_embedding_for_file(std::string path) {
    std::ifstream file(path, std::ios::binary);

    if (file)
    {
        file.read((char *)slot_embedding, sizeof(slot_embedding));
    }
    else
    {
        FD_DEBUG_L("<no such file path = " + path + ">");
        file.close();
        return -1;
    }
    file.close();

    time_t end = time(NULL);

    FD_LOG_L("slot embedding 结束加载");
    return 0;
}

int FdResource::set_intent_embedding_for_file(std::string path) {
    std::ifstream file(path, std::ios::binary);

    if (file)
    {
        file.read((char *)intent_embedding, sizeof(intent_embedding));
    }
    else
    {
        FD_DEBUG_L("<no such file path = " + path + ">");
        file.close();
        return -1;
    }
    file.close();

    time_t end = time(NULL);

    FD_LOG_L("intent embedding 结束加载");
    return 0;
}

int FdResource::set_index_intent_for_file(std::string path) {
    std::ifstream file(path);
    std::string line;
    std::string file_data = "";
    
    // 有该文件
    if(file) {
        while (getline (file, line)) {
            file_data += line;
        }
        cJSON *json = cJSON_Parse(file_data.c_str());

        for (int i = 0; i < cJSON_GetArraySize(json); i++)
        {
            try
            {
                cJSON *item = cJSON_GetArrayItem(json, i);
                int key = atoi(item->string);
                std::string value = item->valuestring;
                index_intent_dic[key] = value;
            }
            catch(const std::exception& e)
            {
                FD_DEBUG_L("index_2intent 加载失败");
                cJSON_Delete(json);
                file.close();
                return -1;
            }
        }    

        cJSON_Delete(json);
    } else {
        // 没有该文件
        FD_DEBUG_L("<no such file path = " + path + ">");
        file.close();
        return -1;
    }
    
    file.close();
    FD_LOG_L("index_2_intent 加载完成");
    return 0;
}

int FdResource::set_char_index_for_file(std::string path) {
    /*
    std::ifstream dic_file(path);
    std::string line;
    
    // 有该文件
    if(dic_file) {
        while (getline (dic_file, line)) {
            try
            {
                //单行键值分离
                unsigned long index = line.rfind(":", line.length()-0);
                
                std::string key = line.substr(0, index); //取键
                
                std::string value = line.substr(index+1, line.length()); //取值
                int int_value = atoi(value.c_str());
                char_index_dic[key] = int_value;
                index_char_dic[int_value] = key;
            }
            catch(const std::exception& e)
            {
                FD_DEBUG_L("char_2_index 加载失败");
                dic_file.close();
                return -1;
            }
            
        }
    } else {
        // 没有该文件
        FD_DEBUG_L("<no such file path = " + path + ">");
        dic_file.close();
        return -1;
    }
    
    dic_file.close();
    FD_LOG_L("char_2_index 加载完成");
    return 0;
    */

    std::ifstream file(path);
    std::string line;
    std::string data = "";

    if(file) {
        while (getline(file, line)) {
            data += line;
        }
    } else {
        file.close();
        FD_DEBUG_L("<no such file path = " + path + ">");
        return -1;
    }
    file.close();

    cJSON *json = cJSON_Parse(data.c_str());
    if (json) {
        for (int i = 0; i < cJSON_GetArraySize(json); i++) {
            cJSON *item = cJSON_GetArrayItem(json, i);

            if(item) {
                std::string key = item->string;
                int value = item->valueint;
                char_index_dic[key] = value;
                index_char_dic[value] = key;
            }
        }
        
    } else {
        cJSON_Delete(json);
        FD_DEBUG_L("char_2_index 加载失败");
        return -1;
    }
    

    FD_LOG_L("char_2_index 加载完成");
    return 0;
}

int FdResource::set_intent_score_dic_for_file(std::string path) {
    std::ifstream intent_file(path);
    std::string line;
    std::string intent_data = "";
    // 有该文件
    if(intent_file) {
        while (getline (intent_file, line)) {
            intent_data += line;
        }
    } else {
        // 没有该文件
        FD_DEBUG_L("<no such file path = " + path + ">");
        intent_file.close();
        return -1;
    }
    intent_file.close();
    try
    {
        cJSON *intent_json = cJSON_Parse(intent_data.c_str());
        
        for(int i=0; i<cJSON_GetArraySize(intent_json); i++)   //遍历最外层json键值对
        {
            cJSON * item = cJSON_GetArrayItem(intent_json, i);
            std::string key = item->string;
            std::string value = item->valuestring;
            
            intent_score_dic[atoi(key.c_str())] = atof(value.c_str());
        }
        
        cJSON_Delete(intent_json);
    }
    catch(const std::exception& e)
    {
        FD_DEBUG_L("intennt_score 加载失败");
        return -1;
    }
    

    FD_LOG_L("intent_score 加载完成");
    return 0;
}

int FdResource::set_index_slot_dic_for_file(std::string path) {
    std::ifstream file(path);
    std::string line;
    std::string file_data = "";
    // 有该文件
    if(file) {
        while (getline (file, line)) {
            file_data += line;
        }
    } else {
        // 没有该文件
        FD_DEBUG_L("<no such file path = >" + path);
        file.close();
        return -1;
    }
    file.close();
    
    cJSON *json = cJSON_Parse(file_data.c_str());
    
    for(int i=0; i<cJSON_GetArraySize(json); i++)   //遍历最外层json键值对
    {
        cJSON * item = cJSON_GetArrayItem(json, i);
        
        int index = atoi(item->string);
        index_slot_dic[index] = item->valuestring;
        
    }
    
    cJSON_Delete(json);
    return 0;
}
    
int FdResource::set_slot_index_dic_for_file(std::string path) {
    std::ifstream file(path);
    std::string line;
    std::string file_data = "";
    // 有该文件
    if(file) {
        while (getline (file, line)) {
            file_data += line;
        }
    } else {
        // 没有该文件
        FD_DEBUG_L("<no such file path = >" + path);
        file.close();
        return -1;
    }
    file.close();
    
    cJSON *json = cJSON_Parse(file_data.c_str());
    
    for(int i=0; i<cJSON_GetArraySize(json); i++)   //遍历最外层json键值对
    {
        cJSON * item = cJSON_GetArrayItem(json, i);
        
        std::string key = item->string;
        slot_index_dic[key] = item->valueint;
        
    }
    
    cJSON_Delete(json);
    return 0;
}

int FdResource::set_intent_type_dic_for_file(std::string path) {
    std::ifstream file(path);
    std::string line;
    std::string file_data = "";
    // 有该文件
    if(file) {
        while (getline (file, line)) {
            file_data += line;
        }
    } else {
        // 没有该文件
        FD_DEBUG_L("<no such file path = >" + path);
        file.close();
        return -1;
    }
    file.close();
    
    cJSON *json = cJSON_Parse(file_data.c_str());
    
    for(int i=0; i<cJSON_GetArraySize(json); i++)   //遍历最外层json键值对
    {
        cJSON * item = cJSON_GetArrayItem(json, i);
        
        std::string key = item->string;
        std::string value = item->valuestring;
        intent_type_dic[key] = value;
        
    }
    
    cJSON_Delete(json);
    return 0;
}

int FdResource::set_tts_url_for_file(std::string path) {
    FD_LOG_L("开始加载tts配置");
    std::ifstream file(path);
    std::string line;
    std::string file_data = "";

    //配置文件存在
    if (file) {
        while (getline(file, line)) {
            file_data += line;
        }
    } else {
        FD_DEBUG_L("<no such file path = >" + path);
        file.close();
        return -1;
    }
    file.close();

    cJSON *json = cJSON_Parse(file_data.c_str());

    for (int i = 0; i < cJSON_GetArraySize(json); i++) {
        cJSON *item_json = cJSON_GetArrayItem(json, i);
        cJSON *mode_json = cJSON_GetObjectItem(item_json, "mode");
        cJSON *urls_json = cJSON_GetObjectItem(item_json, "urls");

        if (!mode_json || !urls_json) {
            continue;
        }

        std::vector<std::vector<std::string>> urls;
        for (int j = 0; j < cJSON_GetArraySize(urls_json); j++) {
            cJSON *urls_item_json = cJSON_GetArrayItem(urls_json, j);
            std::vector<std::string> urls_item;

            for (int k = 0; k < cJSON_GetArraySize(urls_item_json); k++) {
                cJSON *url_item = cJSON_GetArrayItem(urls_item_json, k);
                std::string url_str = url_item->valuestring;
                urls_item.push_back(url_str);
            }

            urls.push_back(urls_item);
            
        }

        std::string key = item_json->string;
        tts_url_dic[key] = urls;

        std::string mode = mode_json->valuestring;
        tts_mode_dic[key] = mode;

    }

    FD_LOG_L("tts配置加载结束");
    cJSON_Delete(json);
    return 0;
}

/*
void FdResource::set_tts_url_for_file(std::string path) {
    FD_LOG_L("开始加载tts配置");
    std::ifstream file(path);
    std::string line;
    std::string file_data = "";
    // 有该文件
    if(file) {
        while (getline (file, line)) {
            file_data += line;
        }
    } else {
        // 没有该文件
        FD_DEBUG_L("<no such file path = >" + path);
        file.close();
        return;
    }
    file.close();
    
    cJSON *json = cJSON_Parse(file_data.c_str());
    
    for(int i=0; i<cJSON_GetArraySize(json); i++)   //遍历最外层json键值对
    {
        cJSON * item = cJSON_GetArrayItem(json, i);
        cJSON * url_json = cJSON_GetObjectItem(item, "urls");

        std::string key = item->string;
        std::vector<std::string>urls;

        for (int i = 0; i < cJSON_GetArraySize(url_json); i++) {
            cJSON *url_item = cJSON_GetArrayItem(url_json, i);
            std::string url = url_item->valuestring;
            // FD_LOG_L(url);
            urls.push_back(url);
        }
        tts_url_dic[key] = urls;
        
    }
    FD_LOG_L("tts配置加载完成");
    cJSON_Delete(json);
}
*/


int FdResource::set_model_config_for_file(std::string path) {
    std::ifstream file(path);
    std::string line;
    std::string data;
    if (file) {
        while (getline(file, line)) {
            data += line;
        }
    } else {
        FD_DEBUG_L("<no such file path = >" + path);
        file.close();
        return -1;
    }
    file.close();

    cJSON *json = cJSON_Parse(data.c_str());
    
    for (int i = 0; i < cJSON_GetArraySize(json); i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);
        if (item == NULL) {
            continue;
        }

        cJSON *type_json = cJSON_GetObjectItem(item, "type");
        cJSON *index_json = cJSON_GetObjectItem(item, "index");
        cJSON *intent_json = cJSON_GetObjectItem(item, "intent");
        cJSON *optimal_score_json = cJSON_GetObjectItem(item, "optimal_score");
        cJSON *ambiguous_score_json = cJSON_GetObjectItem(item, "ambiguous_score");
        
        if (index_json == NULL) {
            continue;
        }
        int index = index_json->valueint;
        std::string intent;

        if (intent_json) {
            intent = intent_json->valuestring;
            index_intent_dic[index] = intent;
        }

        if (type_json) {
            std::string type = type_json->valuestring;
            intent_type_dic[intent] = type;
        }

        if (optimal_score_json) {
            double score = optimal_score_json->valuedouble;
            intent_score_dic[index] = score;
        }

        if (ambiguous_score_json) {
            double ambiguous_score = ambiguous_score_json->valuedouble;
            intent_ambiguous_score_dic[index] = ambiguous_score;
        }
        
    }
    
    FD_LOG_L("intent config 加载完成");
    cJSON_Delete(json);
    return 0;
}

/***********************************/
/*            获取数据函数           */
/***********************************/
std::string FdResource::getIntentWithIndex(int index) {
    return index_intent_dic[index];
}
    
float FdResource::getScoreWithIntent(int intent) {
    return intent_score_dic[intent];
}

float FdResource::getAmbiguousScoreWithIntent(int intent) {
    return intent_ambiguous_score_dic[intent];
}

float *FdResource::getEmbeddingWithIndex(int index) {
    return embeddings[index];
}

float *FdResource::getVocabEmbeddingWithIndex(int index) {
    return vocab_embedding[index];
}

float *FdResource::getIntentEmbeddingWithIndex(int index) {
    return intent_embedding[index];
}

float *FdResource::getSlotEmbeddingWithIndex(int index) {
    return slot_embedding[index];
}

int FdResource::char2index(std::string char_str) {
    if (char_index_dic.find(char_str) != char_index_dic.end()) {
        return char_index_dic[char_str];
    } else {
        return -1;
    }
}

std::string FdResource::index2char(int index) {
    return index_char_dic[index];
}

// std::map<int, std::string> index_slot_dic;
std::string FdResource::getSlotWithIntent(int intent) {
    return index_slot_dic[intent];
}

// std::map<std::string, int> slot_index_dic;
int FdResource::getIntentIndexWithSlot(std::string slot) {
    return slot_index_dic[slot];
}

std::string FdResource::getTypeWithIntent(std::string intent) {
    return intent_type_dic[intent];
}

std::vector<std::string> FdResource::getTtsUrlsWithIntentIndex(std::string intent_index) {
    std::vector<std::string> tts_urls;
    // std::vector<std::string> tts_urls = tts_url_dic[intent_index];

    std::string mode = tts_mode_dic[intent_index];
    std::vector <std::vector<std::string> > tts_urls_list = tts_url_dic[intent_index];

    if (mode == "primary") {
        for (int i = 0; i < tts_urls_list.size(); i++) {
            std::vector<std::string> tts_urls_item = tts_urls_list[i];
            for (int j = 0; j < tts_urls_item.size(); j++)
            {
                tts_urls.push_back(tts_urls_item[j]);
            }
            
        }
        
    } else if(mode == "random" && tts_urls_list.size()) {
        srand(time(NULL));
        int rand_num = ( rand() % tts_urls_list.size() );
        //FD_DEBUG_L("random = " + std::to_string(rand_num));
        tts_urls = tts_urls_list[rand_num];
    }

    return tts_urls;
}