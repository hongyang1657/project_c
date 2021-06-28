#include "fd_config.h"

using namespace std;

FdConfig::FdConfig()
{
    set_value_config_path(CONFIG_PATH);
}

FdConfig::~FdConfig()
{
}

void FdConfig::set_value_config_path(string path) {
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

    cJSON *json = cJSON_Parse(data.c_str());

    cJSON *intent_json = cJSON_GetObjectItem(json, "intent_config");
    if (intent_json) {
        _intent_model = intent_json->valueint;
        FD_LOG_L("intent model status: " + to_string(_intent_model));
    }

    cJSON *slot_json = cJSON_GetObjectItem(json, "slot_config");
    if (slot_json) {
        _slot_model = slot_json->valueint;
        FD_LOG_L("slot model status: " + to_string(_slot_model));
    }
    
    cJSON *round_json = cJSON_GetObjectItem(json, "round_config");
    if (round_json) {
        _multi_round = round_json->valueint;
        FD_LOG_L("multi round status: " + to_string(_multi_round));
    }

    cJSON *offline_json = cJSON_GetObjectItem(json, "offline_config");
    if (offline_json) {
        _offline = offline_json->valueint;
        FD_LOG_L("offline model status: " + to_string(_offline));
    }

    cJSON *online_json = cJSON_GetObjectItem(json, "online_config");
    if (online_json) {
        _online = online_json->valueint;
        FD_LOG_L("online model status: " + to_string(_online));
    }

    cJSON_Delete(json);
    file.close();
    FD_LOG_L("配置完成\n");
}