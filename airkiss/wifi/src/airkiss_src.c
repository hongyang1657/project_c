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

#include "../capture/common.h"
#include "../capture/osdep.h"
#include "../utils/utils.h"
#include "../utils/wifi_scan.h"
#include "../include/airkiss.h"
#include "airkiss_src.h"

static net_config_callback net_callback = NULL;

static pthread_t g_thread;
static bool g_exit = false; 
static bool g_switch_exit = false; 
static unsigned int g_start_ms;

#define MAX_CHANNELS 14
#define conntcts_max 2

static airkiss_context_t *akcontex = NULL;
static const airkiss_config_t akconf = {
(airkiss_memset_fn)&memset,
(airkiss_memcpy_fn)&memcpy,
(airkiss_memcmp_fn)&memcmp,
(airkiss_printf_fn)&printf };

static airkiss_result_t ak_result;
static char *wifi_if = "wlan0";
static struct wif *wi = NULL;
static int g_channels[MAX_CHANNELS] = {0};
static int g_channel_index = 0;
static int g_channel_nums = 0;
static pthread_mutex_t lock;
static unsigned char g_random = 0;

static void add_channel(int chan) {
    int i;
    for(i=0; i<g_channel_nums; i++) {
        if(g_channels[i]==chan)
            break;
    }
    if(i==g_channel_nums) {
        g_channel_nums += 1;
        g_channels[i] = chan;
    }
}

static void init_channels(void)
{
    int i;
    for(i=1; i<MAX_CHANNELS; i++)
    {
        add_channel(i);
    }
}

static int operation_wifi_managed()
{
    //iw dev wlan0 info
    char s[64] = { 0 };
    //ifconfig wlan0 down
    snprintf(s, sizeof(s) - 1, "ifconfig %s down", wifi_if);
    if (system(s) != 0)
        goto error;
    //iw wlan0 set type managed
    snprintf(s, sizeof(s) - 1, "iw %s set type managed", wifi_if);
    if (system(s) != 0)
        goto error;
    //ifconfig wlan0 up
    snprintf(s, sizeof(s) - 1, "ifconfig %s up", wifi_if);
    if (system(s) != 0) {
        goto error;
    }
    return 0;
error:
    LOG_TRACE("cmd:\"%s\" failure.\n", s);
    return -1;
}

static wireless_scan_head operation_scan(void)
{
    wireless_scan_head head;
    wireless_scan *presult = NULL;
    printf("Scanning accesss point...\n");
    if(wifi_scan(wifi_if, &head) == 0)
    {
        printf("Scan success.\n");
        presult = head.result;
        while(presult != NULL) {
            unsigned int freq = get_freq_mhz(presult);
            int channel = getChannelFromFrequency(freq);
            printf("channel = %d\n",channel);
            add_channel(channel);
            presult = presult->next;
        }
    }
    else
    {
        printf("ERROR to scan AP, init with all %d channels\n", MAX_CHANNELS);
        init_channels();
    }

    return head;
}

static void *switch_channel_callback(void *arg)
{
    (void)arg;
    
    while(1)
    {
        usleep(1000*400);

        if((g_exit == true) || (g_switch_exit == true))
            break;

        pthread_mutex_lock(&lock);
        g_channel_index++;
        if(g_channel_index > g_channel_nums - 1)
        {
            g_channel_index = 0;
            printf("scan all channels\n");
        }
    
        int ret = wi_set_channel(wi, g_channels[g_channel_index]);
        if (ret) {
            printf("cannot set channel to %d\n", g_channels[g_channel_index]);
        }

        airkiss_change_channel(akcontex);
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

static int udp_broadcast(unsigned char random, int port)
{
    int fd;
    int enabled = 1;
    int err;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_BROADCAST;
    addr.sin_port = htons(port);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        LOG_TRACE("Error to create socket, reason: %s", strerror(errno));
        return 1;
    }

    err = setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char *) &enabled, sizeof(enabled));
    if(err == -1)
    {
        close(fd);
        return 1;
    }

    LOG_TRACE("Sending random to broadcast..");
    int i;
    useconds_t usecs = 1000*20;
    for(i=0;i<50;i++)
    {
        sendto(fd, (unsigned char *)&random, 1, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr));
        usleep(usecs);
    }

    close(fd);
    return 0;
}

static int process_airkiss(const unsigned char *packet, int size)
{
    pthread_mutex_lock(&lock);
    int ret;

    ret = airkiss_recv(akcontex, (void *)packet, size);
    if(ret == AIRKISS_STATUS_CONTINUE)
    {
    }
    else if(ret == AIRKISS_STATUS_CHANNEL_LOCKED)
    {
        g_switch_exit = true;
        printf("Lock channel in %d\n", g_channels[g_channel_index]);
        if(net_callback)
        {
            net_callback(API_NET_CONFIG_AIR_KISS_TYPE, API_NET_CONFIG_ERROR_LOCK_OK, NULL, NULL);
        }
    }
    else if(ret == AIRKISS_STATUS_COMPLETE)
    {
        printf("Airkiss completed.\n");
        airkiss_get_result(akcontex, &ak_result);
        printf("Result:\n ssid_crc:[%x]\n key_len:[%d]\n key:[%s]\n random:[0x%02x]\n ssid:[%s]\n",
            ak_result.reserved,
            ak_result.pwd_length,
            ak_result.pwd,
            ak_result.random,
            ak_result.ssid);

        if(net_callback)
        {
            net_callback(API_NET_CONFIG_AIR_KISS_TYPE, API_NET_CONFIG_ERROR_GET_SSID_PWD_OK, ak_result.ssid, ak_result.pwd);
        }

        g_random = ak_result.random;
    }
    pthread_mutex_unlock(&lock);

    return ret;
}

static int operation_airkiss(void)
{
    int ret = 0;
    /* Open the interface and set mode monitor */
    wi = wi_open(wifi_if);
    if (!wi) {
        LOG_ERROR("cannot init interface %s", wifi_if);
        return 1;
    }

    /* airkiss setup */
    int result;
    akcontex = (airkiss_context_t *)malloc(sizeof(airkiss_context_t));
    if (akcontex == NULL) {
        LOG_ERROR("malloc failed!!");
        ret = -1;
        goto MALLOC_ERR;
    }

    result = airkiss_init(akcontex, &akconf);
    if(result != 0)
    {
        LOG_ERROR("Airkiss init failed!!");
        ret = -1;
        goto INIT_ERR;
    }
    LOG_TRACE("Airkiss version: %s", airkiss_version());
    if(pthread_mutex_init(&lock, NULL) != 0)
    {
        LOG_ERROR("mutex init failed");
        ret = -1;
        goto MUTEX_ERR;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, switch_channel_callback, NULL);

    int read_size;
    unsigned char buf[RECV_BUFSIZE] = {0};
    for(;;)
    {
        read_size = wi_read(wi, buf, RECV_BUFSIZE, NULL);
        if ((read_size < 0) || (g_exit == true)) {
            g_exit = true;
            LOG_ERROR("recv failed, ret %d", read_size);
            break;
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        int end_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        if((end_ms - g_start_ms) > 1000*60)
        {
            printf("airkiss time out\n");
            g_exit = true;
            if(net_callback)
            {
                net_callback(API_NET_CONFIG_AIR_KISS_TYPE, API_NET_CONFIG_ERROR_TIME_OUT, NULL, NULL);
            }
            break;
        }
    
        if(AIRKISS_STATUS_COMPLETE == process_airkiss(buf, read_size))
            break;
    }

    pthread_join(thread, NULL);

MUTEX_ERR:
    pthread_mutex_destroy(&lock);

INIT_ERR:
    free(akcontex);

MALLOC_ERR:
    wi_close(wi);
    wi = NULL;
    return ret;
}

static int do_thing(void)
{
    if (0 != operation_wifi_managed()) 
    {
        return -1;
    }

    sleep(1);
    wireless_scan_head head;
    head = operation_scan();
    if (head.result == NULL)
        return -1;

    if (0 != operation_airkiss()) {
        return -1;
    }

    printf("airkiss conf is complete.\n");
    return 0;
}

static int do_loop(void)
{
    if (do_thing() == 0)
    {
        //(char)ak_result.random;
        return 0;
    }
    else
    {
        // error
        if(net_callback)
        {
            net_callback(API_NET_CONFIG_AIR_KISS_TYPE, API_NET_CONFIG_ERROR_ERROR, NULL, NULL);
        }
        return -1;
    }
}

static bool activity_of_netthread = false;
static void *net_thread(void *arg)
{
    (void)arg;
    activity_of_netthread = true;
    pthread_detach(pthread_self());
    do_loop();

    if(net_callback)
    {
        net_callback(API_NET_CONFIG_AIR_KISS_TYPE, API_NET_CONFIG_ERROR_EXIT, NULL, NULL);
    }
    activity_of_netthread = false;
    return NULL;
}

static void *airkiss_result_broadcast(void *arg)
{
    int i = 0;
    for(i=0;i<3;i++)
    {
        udp_broadcast(*(unsigned char *)arg, 10000);
        usleep(500 *1000);
    }
    return NULL;
}

static void deal_with_signal_usr1(int signal)
{
    if (signal == SIGUSR1) {
        if (g_random != 0) {
            pthread_t udp_thread;
            pthread_create(&udp_thread, NULL, airkiss_result_broadcast, (void *)&g_random);
        }
    }
}

static char g_signal_usr1 = 0;
static void register_signal_usr1()
{
    if (g_signal_usr1 == 0) {
        g_signal_usr1 = 1;
        signal(SIGUSR1, deal_with_signal_usr1);
    }

    char buf[64] = {0};
    sprintf(buf, "echo %d > /var/run/airkiss.pid", getpid());
    system(buf);
}

void airkiss_start(void)
{
    if (activity_of_netthread) {
        printf("last task of airkiss is unfinished\n");
        if (net_callback) {
            net_callback(API_NET_CONFIG_AIR_KISS_TYPE, API_NET_CONFIG_ERROR_ERROR, NULL, NULL);
        }
        return;
    }

    int i = 0;
    for(i = 0; i < MAX_CHANNELS; i++)
    {
        g_channels[i] = 0;
    }
    g_channel_index = 0;
    g_channel_nums = 0;
    
    g_exit = false;
    g_switch_exit = false;
    g_random = 0;
    register_signal_usr1();
    
    memset(&ak_result, 0, sizeof(airkiss_result_t));

    struct timeval tv;
    gettimeofday(&tv, NULL);
    g_start_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    if(net_callback)
    {
        net_callback(API_NET_CONFIG_AIR_KISS_TYPE, API_NET_CONFIG_ERROR_START_SCAN, NULL, NULL);
    }
    
    pthread_create(&g_thread, NULL, net_thread, NULL);
}

void airkiss_stop(void)
{
    g_exit = true;
    //pthread_join(g_thread, NULL);
    //g_exit = false;
}

void airkiss_regist_callback(net_config_callback callback)
{
    net_callback = callback;
}
