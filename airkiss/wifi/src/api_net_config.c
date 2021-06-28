#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

#include "api_net_config.h"

//#define HAVE_AIRKISS
#ifdef HAVE_AIRKISS
#include "airkiss_src.h"
#endif
#ifdef HAVE_SOUNDWAVE
#include "wav_src.h"
#endif
#ifdef HAVE_BLE
#include "bt_src.h"
#endif

//#define APP_TEST

typedef struct DATA
{
    char ssid[128];
    char pwd[128];
    int used;
}DATA;

#define MAX_NUM 10
static DATA g_data[MAX_NUM];

static void dump(void)
{
    int i = 0;
    for(i = 0; i < MAX_NUM; i++)
    {
        printf("\n");
        printf("index = %d ssid = %s pwd = %s\n", i, g_data[i].ssid, g_data[i].pwd);
        printf("\n");
    }
}

static int g_flag = 0;
static net_config_callback g_net_callback = NULL;
static int net_callback(NET_CONFIG_TYPE_T type, NET_CONFIG_MSG_TYPE_T error_type, char *ssid, char *pwd)
{
    printf("type = %d, error_type = %d\n", type, error_type);

    if(error_type == API_NET_CONFIG_ERROR_GET_SSID_PWD_OK)
    {
        printf("get ssid pwd ok\n");
        printf("ssid = %s, pwd = %s\n", ssid, pwd);
        g_flag = 1;

        int i = 0;
        int flag = 0;
        for(i = 0; i < MAX_NUM; i++)
        {
            if(strcmp(g_data[i].ssid, ssid) == 0)
            {
                memcpy(g_data[i].pwd, pwd, strlen(pwd));
                g_data[i].pwd[strlen(pwd)] = '\0';
                flag = 1;
                break;
            }
        }

        if(flag == 0)
        {
            for(i = 0; i < MAX_NUM; i++)
            {
                if(g_data[i].used == 0)
                {
                    g_data[i].used = 1;
                    memcpy(g_data[i].ssid, ssid, strlen(ssid));
                    g_data[i].ssid[strlen(ssid)] = '\0';
                    memcpy(g_data[i].pwd, pwd, strlen(pwd));
                    g_data[i].pwd[strlen(pwd)] = '\0';
                    flag = 1;
                    break;
                }
            }
        }

        if(flag == 0)
        {
            g_data[0].used = 1;
            memcpy(g_data[0].ssid, ssid, strlen(ssid));
            g_data[0].ssid[strlen(ssid)] = '\0';
            memcpy(g_data[0].pwd, pwd, strlen(pwd));
            g_data[0].pwd[strlen(pwd)] = '\0';
        }

        if (access("/data/system/", F_OK) != 0)
        {
            if (mkdir("/data/system/", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
            {
                printf("mkdir /data/system/ failed\n");
            }
        }

        FILE *file_data = fopen("/data/system/data","w");
        if(file_data)
        {
            fwrite(g_data, sizeof(g_data), 1, file_data);
            fclose(file_data);
        }

        FILE *file_wpa = fopen("/data/system/wpa_supplicant.conf","w");
        if(file_wpa)
        {
            fprintf(file_wpa, "ctrl_interface=/var/run/wpa_supplicant\n");
            for(i = 0; i < MAX_NUM; i++)
            {
                if(g_data[i].used == 1)
                {
                    fprintf(file_wpa, "network={\n");
                    fprintf(file_wpa, "ssid=\"%s\"\n", g_data[i].ssid);
                    if (strlen(g_data[i].pwd) == 0){
                        fprintf(file_wpa, "key_mgmt=NONE\n");
                    } else {
                        fprintf(file_wpa, "psk=\"%s\"\n", g_data[i].pwd);
                    }
                    fprintf(file_wpa, "scan_ssid=1\n");
                    if (!strcmp(g_data[i].ssid, ssid)) {
                        fprintf(file_wpa, "priority=5\n");
                    } else {
                        fprintf(file_wpa, "priority=1\n");
                    }
                    fprintf(file_wpa, "}\n");
                }
            }

            fclose(file_wpa);
        }

        //system("/etc/wlan_force.sh");
    }
    else if(error_type == API_NET_CONFIG_ERROR_TIME_OUT)
    {
        g_flag = 1;
    }

    if(g_net_callback)
        g_net_callback(type, error_type, ssid, pwd);

    return 0;
}

#ifdef APP_TEST
static int app_net_callback(NET_CONFIG_TYPE_T type, NET_CONFIG_MSG_TYPE_T error_type, char *ssid, char *pwd)
{
    (void)type;
    (void)error_type;
    (void)ssid;
    (void)pwd;
    return 0;
}
#endif

/////////////////////////////////////////////////////////////////////

void api_net_config_start(NET_CONFIG_TYPE_T type)
{
    memset(g_data, 0, sizeof(g_data));
    FILE *file = fopen("/usr/share/airkiss/data","r");
    if(file)
    {
        fread(g_data, sizeof(g_data), 1, file);
        fclose(file);
    }

    dump();

    system("killall wpa_supplicant");

    if(type == API_NET_CONFIG_AIR_KISS_TYPE)
    {
#ifdef HAVE_AIRKISS
        printf("####airkiss_start###\n");
        airkiss_start();
#else
        printf("there are no integrating network by airkiss\n");
#endif
    }
    else if(type == API_NET_CONFIG_WAVE_TYPE)
    {
#ifdef HAVE_SOUNDWAVE
        wav_start();
#else
        printf("there are no integrating network by soundwave\n");
#endif
    }
    else if(type == API_NET_CONFIG_BT_TYPE)
    {
#ifdef HAVE_BLE
        bt_start();
#else
        printf("there are no integrating network by ble\n");
#endif
    }
}

void api_net_config_stop(NET_CONFIG_TYPE_T type)
{
    if(type == API_NET_CONFIG_AIR_KISS_TYPE)
    {
#ifdef HAVE_AIRKISS
        airkiss_stop();
#endif
    }
    else if(type == API_NET_CONFIG_WAVE_TYPE)
    {
#ifdef HAVE_SOUNDWAVE
        wav_stop();
#endif
    }
    else if(type == API_NET_CONFIG_BT_TYPE)
    {
#ifdef HAVE_BLE
        bt_stop();
#endif
    }
}

void api_net_config_regist_callback(net_config_callback callback)
{
    g_net_callback = callback;
#ifdef HAVE_AIRKISS
    airkiss_regist_callback(net_callback);
#endif
#ifdef HAVE_SOUNDWAVE
    wav_regist_callback(net_callback);
#endif
#ifdef HAVE_BLE
    bt_regist_callback(net_callback);
#endif
}

void api_net_config_voice_input(NET_CONFIG_TYPE_T type, void *audio, size_t size)
{
    if(type == API_NET_CONFIG_WAVE_TYPE)
    {
#ifdef HAVE_SOUNDWAVE
        wav_input(audio, size);
#else
        if((audio == NULL) && (size == 0))
            printf("invaild data\n");
#endif
    }
}

#ifdef APP_TEST
int main(void)
{
    api_net_config_regist_callback(app_net_callback);

    printf("please inpt 1-3 to test, 1: airkiss, 2:wav, 3:bt\n");
    char input = getchar();

#if 1 // airkiss test
    if(input == '1')
    {
        g_flag = 0;
        api_net_config_start(API_NET_CONFIG_AIR_KISS_TYPE);
        while(1)
        {
            sleep(1);
            if(g_flag == 1)
                break;
        }
        printf("airkiss test exit\n");
        api_net_config_stop(API_NET_CONFIG_AIR_KISS_TYPE);
    }
#endif

#if 1 // wav test
    if(input == '2')
    {
        FILE *fpp = fopen("/out.pcm", "rb");
        g_flag = 0;
        api_net_config_start(API_NET_CONFIG_WAVE_TYPE);
        while(1)
        {
            char buffer[512] = {0};
            int ret = fread(buffer, 1, 512, fpp);
            if(ret > 0)
            {
                api_net_config_voice_input(API_NET_CONFIG_WAVE_TYPE, buffer, ret);
            }
            usleep(1000*10);
            if(g_flag == 1)
                break;
        }
            printf("wav test exit\n");
            api_net_config_stop(API_NET_CONFIG_WAVE_TYPE);
        }
#endif

#if 1 // bt test
    if(input == '3')
    {
        g_flag = 0;
        api_net_config_start(API_NET_CONFIG_BT_TYPE);
        while(1)
        {
            usleep(1000*10);
            if(g_flag == 1)
                break;
        }
        printf("bt test exit\n");
        api_net_config_stop(API_NET_CONFIG_BT_TYPE);
    }

#endif

    sleep(5);
    return 0;
}
#endif
