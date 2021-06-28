#include "Tool.h"
#include "fd_log.h"
#include "fd_config.h"

using namespace std;

#define CONFIG_PATH "/config.conf"

FdConfig::FdConfig()
{
    initConfigValue(CONFIG_PATH);
}

FdConfig::~FdConfig()
{
    cJSON_Delete(config_json);
}

void FdConfig::initConfigValue(string path) {
    ifstream file(path);

    if (!file) {
        FD_DEBUG_L("配置文件加载失败");
        file.close();
        return;
    }
    
    string data = "";
    string line = "";

    while (getline(file, line))
    {
        data += line;
    }

    file.close();

    if (config_json) {
        cJSON_Delete(config_json);
        config_json = NULL;
    }
    config_json = cJSON_Parse(data.c_str());
    if (!config_json) {
        FD_DEBUG_L("配置文件加载失败");
        return;
    }

    cJSON *serialLists = cJSON_GetObjectItem(config_json,"protocolLists");

    for (int i = 0; i < cJSON_GetArraySize(serialLists); i++) {
        cJSON *item = cJSON_GetArrayItem(serialLists, i);
        if (item == NULL) {
            continue;
        }

        cJSON *serial_json = cJSON_GetObjectItem(item, "serial");
        cJSON *pin_json = cJSON_GetObjectItem(item, "pin");
        cJSON *value_json = cJSON_GetObjectItem(item, "value");
        
        if (serial_json == NULL) {
            continue;
        }
        string serial = serial_json->valuestring;
        pin_map[serial] = pin_json->valueint;
        value_map[serial] = value_json->valueint;
        
    }

    FD_LOG_L("配置文件加载完成\n");
}



string FdConfig::getConfigString(std::string config_name, std::string config_key)
{
    string value = "";
    if (!config_json) {
        FD_DEBUG_L("配置文件异常" + config_name);
        return value;
    }

    cJSON* modular_json = cJSON_GetObjectItem(config_json, config_name.c_str());
    if (!modular_json) {
        FD_DEBUG_L("没有找到相关模块名" + config_name);
        return value;
    }

    cJSON* value_json = cJSON_GetObjectItem(modular_json, config_key.c_str());
    if (!value_json) {
        FD_DEBUG_L("没有找到相关键名" + config_name);
        return value;
    }

    value = value_json->valuestring;
    return value;
}

bool FdConfig::getConfigBool(std::string config_name, std::string config_key)
{
    bool value = false;

    if (!config_json) {
        FD_DEBUG_L("配置文件异常" + config_name);
        return value;
    }

    cJSON* modular_json = cJSON_GetObjectItem(config_json, config_name.c_str());
    if (!modular_json) {
        FD_DEBUG_L("没有找到相关模块名 " + config_name);
        return value;
    }

    cJSON* value_json = cJSON_GetObjectItem(modular_json, config_key.c_str());
    if (!value_json) {
        FD_DEBUG_L("没有找到相关键名" + config_name);
        return value;
    }

    value = value_json->valueint;
    return value;
}

int FdConfig::getConfigInt(std::string config_name, std::string config_key)
{
    int value = -1;

    if (!config_json) {
        FD_DEBUG_L("配置文件异常" + config_name);
        return value;
    }

    cJSON* modular_json = cJSON_GetObjectItem(config_json, config_name.c_str());
    if (!modular_json) {
        FD_DEBUG_L("没有找到相关模块名" + config_name);
        return value;
    }

    cJSON* value_json = cJSON_GetObjectItem(modular_json, config_key.c_str());
    if (!value_json) {
        FD_DEBUG_L("没有找到相关键名" + config_name);
        return value;
    }

    value = value_json->valueint;
    return value;
}
