#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <linux/input.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include "../utils/utils.h"
#include "bt_src.h"
#include "librokid-bt.h"
#include <stdbool.h>

#define MAX_LINE            100
#define BT_TIMEOUT 60

static net_config_callback net_callback = NULL;
static RKBluetooth *bt_handle;
static pthread_t g_thread;
static bool _running = false;

static void discovery_callback(void *userdata, const char *bt_name, const char bd_addr[6], int is_completed)
{
    (void)userdata;
    (void)bd_addr;
    //char address[18];

    printf("[%s][%s][%d] ===== discovery_callback ===== \n", __FILE__, __func__, __LINE__);
    printf("get bt_name : %s\n", bt_name);
    //printf("get bd_addr : %s\n", format_address(bd_addr, address) ? address : "NULL");
    //printf("get bd_name : %s\n", address);
    printf("get is_completed = %d (%s)\n", is_completed, ((is_completed == 1) ? "true" : "false"));
}

static void event_listener(void *userdata, int what, int arg1, int arg2, void *data)
{
    (void)userdata;
    char msgBuf[MAX_LINE] = {0};

    printf("[%s][%s][%d] ===== event_listener =====\n", __FILE__, __func__, __LINE__);
    printf("get bt event what: %d\n", what);
    printf("get bt event arg1: %d\n", arg1);
    printf("get bt event arg2: %d\n", arg2);
    printf("get bt event data: %s\n", (char*)(data ? data : "NULL"));

    if(what == BT_EVENT_BLE_OPEN) //41
    {
        sprintf (msgBuf, "sl_ble_link::%s", "connected");

        if(net_callback)
        {
            net_callback(API_NET_CONFIG_BT_TYPE, API_NET_CONFIG_ERROR_LOCK_OK, NULL, NULL);
        }
    }
    else if(what == BT_EVENT_BLE_CLOSE) //42
    {
        sprintf (msgBuf, "sl_ble_link::%s", "disconnected");
    }
    else if(what == BT_EVENT_BLE_WRITE) //43
    {
        if(data != NULL)
            sprintf (msgBuf, "sl_ble_data::%s", (char*)data);
    }

    if(strlen(msgBuf) > 0)
    {
        char ssid[32] = {0};
        char psk[32] = {0};
        char wechat_id[32] = {0};
        printf("[%s][%s][%d] msgBuf: %s\n", __FILE__, __func__, __LINE__, msgBuf);

        char *pstart = NULL;
        char *pend = NULL;
	    pstart = strstr(msgBuf, "/");
	    if (pstart) 
        {
            pend = strstr(pstart+1, "&");
        }
	    if (NULL == pstart || NULL == pend) 
        {
            return;
        }
	    strncpy(ssid, pstart+1, pend - pstart - 1);

	    pstart = pend;
	    if (pstart) {
	        pend = strstr(pstart+1, "&");
	    }
        if (NULL == pstart || NULL == pend)
        {
            return;
        }
        strncpy(psk, pstart+1, pend - pstart - 1);

        pstart = pend;
	    if (pstart) 
        {
            pend = strstr(pstart+1, "/");
        }
	    if (NULL == pstart || NULL == pend) 
        {
            return;
        }
	    strncpy(wechat_id, pstart+1, pend - pstart - 1);

        if(net_callback)
        {
            net_callback(API_NET_CONFIG_BT_TYPE, API_NET_CONFIG_ERROR_GET_SSID_PWD_OK, ssid, psk);
            //sleep(3);
        }
        _running = false;
    }
}

static int close_bluetooth(void)
{
    if(bt_handle != NULL)
    {
        rokidbt_destroy(bt_handle);
        bt_handle = NULL;
    }

    return 0;
}

static int open_bluetooth(void)
{
    int res = -1;

    close_bluetooth();

    bt_handle = rokidbt_create();
    if(bt_handle)
    {
        rokidbt_set_discovery_cb(bt_handle, discovery_callback, NULL);
        rokidbt_set_event_listener(bt_handle, event_listener, NULL);
        res = rokidbt_init(bt_handle, NULL);
    }

    return res;
}

static int set_bluetooth_name(const char *name)
{
    if((name == NULL) || (strlen(name) == 0))
        return -1;

    rokidbt_set_name(bt_handle, name);
    return 0;
}

static int enable_ble(void)
{
    return rokidbt_ble_enable(bt_handle);
}

static int disable_ble(void)
{
    return rokidbt_ble_disable(bt_handle);
}

static void *bt_thread(void *arg)
{
    (void)arg;
    pthread_detach(pthread_self());

    open_bluetooth();
    set_bluetooth_name("Rokid-Me-000000");
    enable_ble();

    time_t first_time = time(NULL);

    while (_running) 
    {
        sleep(1);

        if (time(NULL) - first_time > BT_TIMEOUT)
        {
            printf ("timeout\n");
            if(net_callback)
            {
                net_callback(API_NET_CONFIG_BT_TYPE, API_NET_CONFIG_ERROR_TIME_OUT, NULL, NULL);
                _running = false;
                break;
            }
        }
    }

    disable_ble();
    close_bluetooth();

    if(net_callback)
    {
        net_callback(API_NET_CONFIG_BT_TYPE, API_NET_CONFIG_ERROR_EXIT, NULL, NULL);
    }

    return NULL;
}

void bt_start(void)
{
    _running = true;

    if(net_callback)
    {
        net_callback(API_NET_CONFIG_BT_TYPE, API_NET_CONFIG_ERROR_START_SCAN, NULL, NULL);
    }
    
    pthread_create(&g_thread, NULL, bt_thread, NULL);
}

void bt_stop(void)
{
    _running = false;
}

void bt_regist_callback(net_config_callback callback)
{
    net_callback = callback;
}
