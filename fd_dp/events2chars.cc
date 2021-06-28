//
//  events2chars.cpp
//  AM
//
//  Created by Trilobite on 2019/4/8.
//  Copyright © 2019 Trilobite. All rights reserved.
//

#include "events2chars.h"
#include "fd_log.h"

using namespace::std;

std::vector<std::string> events2chars(std::vector<std::string> events, bool special_intent_slot) {
    
    std::vector<std::string> chars;
    
    for (std::string str : events) {
        cJSON *event = cJSON_Parse(str.c_str());
        char *type_str_char = cJSON_Print(cJSON_GetObjectItem(event, "type"));
        std::string type_str = type_str_char;
        free(type_str_char);
        type_str = clearHeadTailQuotes(type_str);
        
        if (!type_str.compare("u2a_speech")) {
            chars.push_back('[' + type_str + ']');  //压入类型
            char *speech_char = cJSON_Print(cJSON_GetObjectItem(event, "speech"));
            std::string speech = speech_char;
            free(speech_char);
            speech = clearHeadTailQuotes(speech);
            
            std::vector <std::string> dump = subdump(speech);
            
            for (std::string str : dump) {
                chars.push_back(str);
            }
        }
        else if(!type_str.compare("a2s")) {
            chars.push_back('[' + type_str + ']');  //压入类型
            
            char *intent_char = cJSON_Print(cJSON_GetObjectItem(event, "intent"));
            std::string intent = intent_char;
            free(intent_char);
            intent = clearHeadTailQuotes(intent);
            
            if (special_intent_slot) {
                chars.push_back('[' + intent + ']');
            } else {
                std::vector <std::string> dump = subdump(intent);
                for (std::string str : dump) {
                    chars.push_back(str);
                }
            }
            
            //解析槽值
            map<string, string, less<string>> tmp_slots;
            cJSON *rq_slots = cJSON_GetObjectItem(event, "request_slots");
            cJSON *rp_slots = cJSON_GetObjectItem(event, "response_slots");

            for (int i = 0; i < cJSON_GetArraySize(rq_slots); i++) {
                cJSON *item = cJSON_GetArrayItem(rq_slots, i);
                if (cJSON_Object == item->type) {
                    continue;
                } else {
                    std::string key = item->string;
                    std::string value = item->valuestring;
                    key = clearHeadTailQuotes(key);
                    value = clearHeadTailQuotes(value);
                    
                    tmp_slots.insert(map<std::string, std::string>::value_type(key, value));
                    
                }
//                cJSON_Delete(item);
            }
            
            for (int i = 0; i < cJSON_GetArraySize(rp_slots); i++) {
                cJSON *item = cJSON_GetArrayItem(rp_slots, i);
                if (cJSON_Object == item->type) {
                    continue;
                } else {
                    std::string key = item->string;
                    std::string value = item->valuestring;
                    key = clearHeadTailQuotes(key);
                    value = clearHeadTailQuotes(value);
                    
                    tmp_slots.insert(map<std::string, std::string>::value_type(key, value));
                    
                }
//                cJSON_Delete(item);
            }

            for (map<std::string, std::string>::iterator iter = tmp_slots.begin(); iter!= tmp_slots.end(); iter++) {
                if (special_intent_slot) {
                    chars.push_back('[' + iter->first + ']');
                } else {
                    std::vector <std::string> dump = subdump(iter->first);
                    for (std::string str : dump) {
                        chars.push_back(str);
                    }
                }
                
                chars.push_back("[sep_of_name_value]");
                std::vector <std::string> dump = subdump(iter->second);
                for (std::string str : dump) {
                    chars.push_back(str);
                }
                
                if (iter != tmp_slots.end()) {
                    chars.push_back("[sep_of_slots]");
                }
            }
            
//            cJSON_Delete(rq_slots);
//            cJSON_Delete(rp_slots);
        }
        else if (!type_str.compare("a2u_api_call")) {
            char *intent_char = cJSON_Print(cJSON_GetObjectItem(event, "intent"));
            std::string intent = intent_char;
            free(intent_char);
            intent = clearHeadTailQuotes(intent);
            
            if (intent.find("release_turn", 0)) {
                continue;
            }
            
            chars.push_back('[' + type_str + ']');  //压入类型
            
            if (special_intent_slot) {
                chars.push_back('[' + intent + ']');
            } else {
                std::vector <std::string> dump = subdump(intent);
                for (std::string str : dump) {
                    chars.push_back(str);
                }
            }
            
            map<string, string, less<string>> tmp_slots;
            cJSON *rq_slots = cJSON_GetObjectItem(event, "request_slots");
            
//            读取slots数据
            for (int i = 0; i < cJSON_GetArraySize(rq_slots); i++) {
                cJSON *item = cJSON_GetArrayItem(rq_slots, i);
                if (cJSON_Object == item->type) {
                    continue;
                } else {
                    std::string key = item->string;
                    std::string value = item->valuestring;
                    key = clearHeadTailQuotes(key);
                    value = clearHeadTailQuotes(value);
                    
                    tmp_slots.insert(map<std::string, std::string>::value_type(key, value));
                    
                }
//                cJSON_Delete(item);
            }
            
//            slots数据拼接
            for (map<std::string, std::string>::iterator iter = tmp_slots.begin(); iter!= tmp_slots.end(); iter++) {
                if (special_intent_slot) {
                    chars.push_back('[' + iter->first + ']');
                } else {
                    std::vector <std::string> dump = subdump(iter->first);
                    for (std::string str : dump) {
                        chars.push_back(str);
                    }
                }
                
                chars.push_back("[sep_of_name_value]");
                std::vector <std::string> dump = subdump(iter->second);
                for (std::string str : dump) {
                    chars.push_back(str);
                }
                
                if (iter != tmp_slots.end()) {
                    chars.push_back("[sep_of_slots]");
                }
            }
            
//            cJSON_Delete(rq_slots);
        }
        else if(!type_str.compare("a2u_speech")) {
            char *intent_char = cJSON_Print(cJSON_GetObjectItem(event, "intent"));
            std::string intent = intent_char;
            free(intent_char);
            intent = clearHeadTailQuotes(intent);
            
            chars.push_back('[' + type_str + ']');  //压入类型
            
            if (special_intent_slot) {
                chars.push_back('[' + intent + ']');
            } else {
                std::vector <std::string> dump = subdump(intent);
                for (std::string str : dump) {
                    chars.push_back(str);
                }
            }
            
            map<string, string, less<string>> tmp_slots;
            cJSON *rq_slots = cJSON_GetObjectItem(event, "request_slots");
            
            //            读取slots数据
            for (int i = 0; i < cJSON_GetArraySize(rq_slots); i++) {
                cJSON *item = cJSON_GetArrayItem(rq_slots, i);
                if (cJSON_Object == item->type) {
                    continue;
                } else {
                    std::string key = item->string;
                    std::string value = item->valuestring;
                    key = clearHeadTailQuotes(key);
                    value = clearHeadTailQuotes(value);
                    
                    tmp_slots.insert(map<std::string, std::string>::value_type(key, value));
                    
                }
//                cJSON_Delete(item);
            }
            
            //            slots数据拼接
            for (map<std::string, std::string>::iterator iter = tmp_slots.begin(); iter!= tmp_slots.end(); iter++) {
                if (special_intent_slot) {
                    chars.push_back('[' + iter->first + ']');
                } else {
                    std::vector <std::string> dump = subdump(iter->first);
                    for (std::string str : dump) {
                        chars.push_back(str);
                    }
                }
                
                chars.push_back("[sep_of_name_value]");
                std::vector <std::string> dump = subdump(iter->second);
                for (std::string str : dump) {
                    chars.push_back(str);
                }
                
                if (iter != tmp_slots.end()) {
                    chars.push_back("[sep_of_slots]");
                }
            }
            
//            cJSON_Delete(rq_slots);
        }
        else if(!type_str.compare("event2a")) {
            char *intent_char = cJSON_Print(cJSON_GetObjectItem(event, "intent"));
            std::string intent = intent_char;
            free(intent_char);
            intent = clearHeadTailQuotes(intent);
            
            chars.push_back('[' + type_str + ']');  //压入类型
            
            if (special_intent_slot) {
                chars.push_back('[' + intent + ']');
            } else {
                std::vector <std::string> dump = subdump(intent);
                for (std::string str : dump) {
                    chars.push_back(str);
                }
            }
            
            map<string, string, less<string>> tmp_slots;
            cJSON *rq_slots = cJSON_GetObjectItem(event, "request_slots");
            
            //            读取slots数据
            for (int i = 0; i < cJSON_GetArraySize(rq_slots); i++) {
                cJSON *item = cJSON_GetArrayItem(rq_slots, i);
                if (cJSON_Object == item->type) {
                    continue;
                } else {
                    std::string key = item->string;
                    std::string value = item->valuestring;
                    key = clearHeadTailQuotes(key);
                    value = clearHeadTailQuotes(value);
                    
                    tmp_slots.insert(map<std::string, std::string>::value_type(key, value));
                    
                }
//                cJSON_Delete(item);
            }
            
            //            slots数据拼接
            for (map<std::string, std::string>::iterator iter = tmp_slots.begin(); iter!= tmp_slots.end(); iter++) {
                if (special_intent_slot) {
                    chars.push_back('[' + iter->first + ']');
                } else {
                    std::vector <std::string> dump = subdump(iter->first);
                    for (std::string str : dump) {
                        chars.push_back(str);
                    }
                }
                
                chars.push_back("[sep_of_name_value]");
                std::vector <std::string> dump = subdump(iter->second);
                for (std::string str : dump) {
                    chars.push_back(str);
                }
                
                if (iter != tmp_slots.end()) {
                    chars.push_back("[sep_of_slots]");
                }
            }
            
//            cJSON_Delete(rq_slots);
        }
        else {
            FD_DEBUG_L("type 错误 ：" + type_str);
            throw exception();
        }
        
        chars.push_back("[end_of_event]");
        
        cJSON_Delete(event);
    }
    
    return chars;
}
