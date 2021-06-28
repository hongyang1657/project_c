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
#include "wav_src.h"
#include "adt.h"
#include "kfifo.h"

static pthread_mutex_t lock;
static pthread_t g_thread;
static void *decoder = NULL;
static struct fifo sw_fifo;
static char *sw_buffer;
static short *dec_buffer;
static bool _running = false;
static int bsize;

static net_config_callback net_callback = NULL;

#define SOUNDWAVE_TIMEOUT 60

/********** CRC16校验 **********/
static const unsigned char aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const unsigned short aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

static unsigned short usCRC16(unsigned char *pucFrame, int len)
{
    unsigned char ucCRCHi = 0xFF;
    unsigned char ucCRCLo = 0xFF;
    unsigned short iIndex;
    for (int i=0; i < len; i++)
    {
        iIndex = (unsigned short)(ucCRCLo ^ pucFrame[i]);
        ucCRCLo = (unsigned char)(ucCRCHi ^ aucCRCHi[iIndex]);
        ucCRCHi = (unsigned char)(aucCRCLo[iIndex]);
    }
    return (unsigned short)((ucCRCHi << 8) | ucCRCLo);
}
/****************************************/

static int VoiceOut(void *audio, size_t size)
{
    return kfifo_out(&sw_fifo, audio, size);
}

static void VoiceIn(void *audio, size_t size)
{
    pthread_mutex_lock(&lock);
    if(_running == true)
    {
        kfifo_in(&sw_fifo, audio, size);
    }
    pthread_mutex_unlock(&lock);
}

static int SoundWaveConfigure(char *conf)
{
#if 1
    //对接杭州服务器
    enum result_type{
        SW_SSID = 0,
        SW_PWD,
        SW_WECHATID,
        SW_CRC,
        SW_TOTAL
    };
    char result[SW_TOTAL][128] = {0};
    char crcpara[256] = {0};
    char calcrc[5] = {0};
    char *pstart = NULL;
    char *pend = NULL;
    int n = 0;

    pstart = strstr(conf, "\1");
    if (pstart) {
        pend = strstr(pstart, "\3");
    }
    if (NULL == pstart || NULL == pend) {
        return -1;
    }
    strncpy(crcpara, pstart, pend - pstart);

    pstart = crcpara;
    pend = NULL;
    while (1) {
        pend = strstr(pstart + 1, "\2");
        if (pend == NULL) {
            strcpy(result[n++], pstart + 1);
            break;
        }
        strncpy(result[n++], pstart + 1, pend - pstart - 1);
        pstart = pend;
    }
    if (n != SW_TOTAL)
        return -1;

    pstart = strstr(crcpara, result[SW_CRC]);
    if (pstart != crcpara) {
        memcpy(pstart - 1, "\3\0", 2);
    }

    sprintf(calcrc, "%x", usCRC16((unsigned char*)crcpara, strlen(crcpara)));
    printf("SOUNDWAVE_DEBUG_TAG ssid[%s] psk[%s] check[%s] calcheck[%s]\n",
            result[SW_SSID], result[SW_PWD], result[SW_CRC], calcrc);
    if (0 != memcmp(result[SW_CRC], calcrc, 4)) {
        return -1;
    }

    if(net_callback)
    {
        net_callback(API_NET_CONFIG_WAVE_TYPE, API_NET_CONFIG_ERROR_GET_SSID_PWD_OK, result[SW_SSID], result[SW_PWD]);
    }
#else
    //对接公众号:我要长大机器人
    char ssid[32] = {0};
    char psk[32] = {0};
    char *p1 = NULL;
    char *p2 = NULL;

    p1 = strstr(conf, "  ");
    if (p1) {
        p2 = strstr(p1 + 2, "  ");
    }

    if (NULL == p1 || NULL == p2)
    {
        return -1;
    }

    if (p2 - p1 - 2 > 0) {
        strncpy(ssid, p1 + 2, p2 - p1 - 2);
    }

    p1 = strstr(p2 + 2, "  ");
    if (p1 && p1 - p2 - 2 > 0) {
        strncpy(psk, p2 + 2, p1 - p2 - 2);
    }

    if (0 == strlen(ssid) || 0 == strlen(psk))
    {
        return -1;
    }

    if(net_callback)
    {
        net_callback(API_NET_CONFIG_WAVE_TYPE, API_NET_CONFIG_ERROR_GET_SSID_PWD_OK, ssid, psk);
    }
#endif
    return 0;
}

static void DeInit(void)
{
    pthread_mutex_lock(&lock);
    if (decoder)
    {
        decoder_destroy(decoder);
        decoder = NULL;
    }

    if (dec_buffer) {
        free(dec_buffer);
        dec_buffer = NULL;
    }

    if (sw_buffer) {
        free(sw_buffer);
        sw_buffer = NULL;
    }

    _running = false;
    pthread_mutex_unlock(&lock);

    //pthread_mutex_destroy(&lock);

    if(net_callback)
    {
        net_callback(API_NET_CONFIG_WAVE_TYPE, API_NET_CONFIG_ERROR_EXIT, NULL, NULL);
    }
}

static void *wav_thread(void *arg)
{
    (void)arg;
    int ret_dec;
    unsigned char out[257] = {0};
    size_t ret = 0;
    time_t first_time = time(NULL);

    pthread_detach(pthread_self());

    while (_running)
    {
        usleep(1000);

        if (time(NULL) - first_time > SOUNDWAVE_TIMEOUT)
        {
            printf ("timeout\n");

            if(net_callback)
            {
                net_callback(API_NET_CONFIG_WAVE_TYPE, API_NET_CONFIG_ERROR_TIME_OUT, NULL, NULL);
            }
            DeInit();
            return NULL;
        }

        pthread_mutex_lock(&lock);

        if (kfifo_len(&sw_fifo) < bsize * sizeof(short))
        {
            pthread_mutex_unlock(&lock);
            continue;
        }

        ret = VoiceOut((void *)dec_buffer, bsize * sizeof(short));
        if (ret != bsize * sizeof(short))
        {
            printf("read over\n");
            pthread_mutex_unlock(&lock);

            if(net_callback)
            {
                net_callback(API_NET_CONFIG_WAVE_TYPE, API_NET_CONFIG_ERROR_ERROR, NULL, NULL);
            }
            break;
        }
        //PRINT_SPEED("audio_out", dec_buffer, ret);

        ret_dec = decoder_fedpcm(decoder, dec_buffer);
        if (RET_DEC_ERROR == ret_dec)
        {
            printf("decoder error !\n");
            pthread_mutex_unlock(&lock);

            if(net_callback)
            {
                net_callback(API_NET_CONFIG_WAVE_TYPE, API_NET_CONFIG_ERROR_ERROR, NULL, NULL);
            }
            break;
        }
        else if (RET_DEC_NOTREADY == ret_dec)
        {
            printf("wait to decoder\n");
        }
        else if (RET_DEC_END == ret_dec)
        {
            printf("decoder end\n");
            memset(out, 0, sizeof(out));
            ret_dec = decoder_getstr(decoder, out);
            if (ret_dec == RET_DEC_NORMAL)
            {
                printf("wav out = %s\n", (char*)out);

                pthread_mutex_unlock(&lock);
                if (0 == SoundWaveConfigure((char *)out))
                {
                 // ok
                    DeInit();
                    return NULL;
                }
                else
                {
                    decoder_reset(decoder);
                }
                continue;
            }
            else
            {
                decoder_reset(decoder);
                printf("decoder output nothing! \n");
            }
        }
        else if (RET_DEC_NORMAL != ret_dec)
        {
            printf("ret is invalid\n");
        }

        pthread_mutex_unlock(&lock);
    }

    DeInit();
    return NULL;
}

static void Init(void)
{
    static int flag = 0;
    if(flag == 0)
    {
        if(pthread_mutex_init(&lock, NULL) != 0)
        {
            printf("mutex init failed\n");
            exit(0);
        }
        flag = 1;
    }

    pthread_mutex_lock(&lock);

    config_decoder_t config_decoder;
    config_decoder.max_strlen = 256;
    config_decoder.freq_type = FREQ_TYPE_LOW;
    config_decoder.sample_rate = 16000;
    config_decoder.error_correct = 1;
    config_decoder.error_correct_num = 2;
    config_decoder.group_symbol_num = 10;

    decoder = decoder_create(&config_decoder);
    if (decoder == NULL)
    {
        printf( "allocate handle error !\n");
        pthread_mutex_unlock(&lock);
        DeInit();
        return;
    }

    bsize = decoder_getbsize(decoder);
    printf("decoder size = %d\n", bsize);

    dec_buffer = (short *)malloc(sizeof(short) * bsize);
    if (dec_buffer == NULL) {
        printf("allocate buffer error !\n");
        pthread_mutex_unlock(&lock);
        DeInit();
        return;
    }

    sw_buffer = (char *)malloc(16000);
    if (sw_buffer == NULL) {
        printf("allocate buffer error !\n");
        pthread_mutex_unlock(&lock);
        DeInit();
        return;
    }

    _running = true;
    kfifo_init(&sw_fifo, sw_buffer, 16000);
    pthread_create(&g_thread, NULL, wav_thread, NULL);
    pthread_mutex_unlock(&lock);
}

void wav_start(void)
{
    if(net_callback)
    {
        net_callback(API_NET_CONFIG_WAVE_TYPE, API_NET_CONFIG_ERROR_START_SCAN, NULL, NULL);
    }

    Init();
}

void wav_stop(void)
{
    pthread_mutex_lock(&lock);
    _running = false;
    pthread_mutex_unlock(&lock);
    //pthread_join(g_thread, NULL);
    //DeInit();
}

void wav_input(void *audio, size_t size)
{
    VoiceIn(audio, size);
}

void wav_regist_callback(net_config_callback callback)
{
    net_callback = callback;
}

