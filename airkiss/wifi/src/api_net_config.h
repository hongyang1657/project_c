#ifndef API_NET_CONFIGH_H
#define API_NET_CONFIGH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    API_NET_CONFIG_AIR_KISS_TYPE = 0,
    API_NET_CONFIG_WAVE_TYPE,
    API_NET_CONFIG_BT_TYPE
} NET_CONFIG_TYPE_T;

typedef enum {
    API_NET_CONFIG_ERROR_CONNECT_FINISH = 0, // device connect wifi ok
    API_NET_CONFIG_ERROR_START_CONNECT, // device start connect wifi
    API_NET_CONFIG_ERROR_START_SCAN, // device start net setting
    API_NET_CONFIG_ERROR_TIME_OUT, // device net setting time out
    API_NET_CONFIG_ERROR_LOCK_OK, // only for airkiss, device lock ok
    API_NET_CONFIG_ERROR_GET_SSID_PWD_OK, // get ssid and pwd ok
    API_NET_CONFIG_ERROR_ERROR, // error
    API_NET_CONFIG_ERROR_EXIT // device setting exit ok
} NET_CONFIG_MSG_TYPE_T;

typedef int (*net_config_callback)(NET_CONFIG_TYPE_T type, NET_CONFIG_MSG_TYPE_T error_type, char *ssid, char *pwd);

void api_net_config_start(NET_CONFIG_TYPE_T type);
void api_net_config_stop(NET_CONFIG_TYPE_T type);
void api_net_config_regist_callback(net_config_callback callback);
void api_net_config_voice_input(NET_CONFIG_TYPE_T type, void *audio, size_t size);

#ifdef __cplusplus
}
#endif

#endif

