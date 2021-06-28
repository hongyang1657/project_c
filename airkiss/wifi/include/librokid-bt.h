#ifndef _LIBROKID_BT_H_
#define _LIBROKID_BT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bsa_hs_api.h"

typedef uint8_t BTAddr[6];
typedef struct
{
    uint8_t  valid; /* TRUE if this entry is valid */
    uint16_t vendor_id_source; /* Indicate if the vendor field is BT or USB */
    uint16_t vendor; /* Vendor Id of the peer device */
    uint16_t product; /* Product Id of the peer device */
    uint16_t version; /* Version of the peer device */
} BTDevPid;

typedef struct
{
    BTAddr bda_connected;  /* peer BDA*/
    uint8_t in_use;         /* TRUE if this connection block is in use */
    int index;              /* Index of this connection block in the array */

    uint8_t ccb_handle;       /* AVK (A2DP) handle */
    uint8_t is_open;        /* TRUE if AVK (signaling channel) is open */

    uint8_t rc_handle;        /* AVRCP handle */
    uint8_t is_rc_open;     /* TRUE if AVRCP is open */
    uint16_t peer_features;       /* peer AVRCP features mask */
    uint16_t peer_version;    /* Peer AVRCP version */

    uint8_t is_streaming_chl_open; /* TRUE is streaming channel is open */
    uint8_t is_started_streaming;     /* TRUE if streaming has started */
    uint8_t format;
    uint16_t sample_rate;
    uint8_t num_channel;
    uint8_t bit_per_sample;

    volatile uint8_t handle; // av handle
    uint8_t volChangeLabel;   /* label used for volume change registration. */
} BTConnection;

typedef struct BTDevice
{
    BTAddr bd_addr; /* BD address peer device. */
    uint8_t class_of_device[3]; /* Class of Device */
    char name[249]; /* Name of peer device. */
    int rssi; /* The rssi value */
    uint32_t services; /* Service discovery discovered */
    BTDevPid eir_vid_pid[1];
    uint8_t eir_data[240];  /* Full EIR data */
    uint8_t inq_result_type;
    uint8_t ble_addr_type;
    uint8_t device_type;
} BTDevice;

#define AVRC_PLAYSTATE_STOPPED                  0x00    /* Stopped */
#define AVRC_PLAYSTATE_PLAYING                  0x01    /* Playing */
#define AVRC_PLAYSTATE_PAUSED                   0x02    /* Paused  */

#define BLE_CHAR_ID1 0x2A06
#define BLE_CHAR_ID2 0x2A07

typedef enum
{
    BT_EVENT_A2DP_OPEN           = 1,     /* connection opened */             
    BT_EVENT_A2DP_CLOSE          = 2,     /* connection closed */             
    BT_EVENT_A2DP_START          = 3,     /* stream data transfer started */  
    BT_EVENT_A2DP_STOP           = 4,     /* stream data transfer stopped */  
    BT_EVENT_A2DP_RC_OPEN        = 5,     /* remote control channel open */   
    BT_EVENT_A2DP_RC_CLOSE       = 6,     /* remote control channel closed */ 
    BT_EVENT_A2DP_REMOTE_CMD     = 7,     /* remote control command */        
    BT_EVENT_A2DP_REMOTE_RSP     = 8,     /* remote control response */       

    BT_EVENT_AVK_OPEN            = 21,    /* connection opened */                  
    BT_EVENT_AVK_CLOSE           = 22,    /* connection closed */                  
    BT_EVENT_AVK_STR_OPEN        = 23,    /* stream data transfer started */       
    BT_EVENT_AVK_STR_CLOSE       = 24,    /* stream data transfer stopped */       
    BT_EVENT_AVK_START           = 25,    /* stream channel opened */              
    BT_EVENT_AVK_PAUSE           = 26,    /* stream channel suspended */              
    BT_EVENT_AVK_STOP            = 27,    /* stream channel closed */              
    BT_EVENT_AVK_RC_OPEN         = 28,    /* remote control channel open */        
    BT_EVENT_AVK_RC_PEER_OPEN    = 29,    /* remote control channel open by peer */
    BT_EVENT_AVK_RC_CLOSE        = 30,    /* remote control channel closed */
    BT_EVENT_AVK_SET_ABS_VOL     = 31,    /* remote control channel closed */
    BT_EVENT_AVK_GET_PLAY_STATUS = 32,    /* get play status */

    BT_EVENT_BLE_OPEN            = 41,
    BT_EVENT_BLE_CLOSE           = 42,
    BT_EVENT_BLE_WRITE           = 43,
    BT_EVENT_BLE_CON             = 44,

    BT_EVENT_MGR_CONNECT         = 100,
    BT_EVENT_MGR_DISCONNECT      = 101,
}rk_bt_event_t;

typedef enum
{
    BT_HS_STATUS_UNKNOWN          = 0,     /* UNKNOWN  STATUS.*/
    BT_HS_SERVICE_OFF           = 1,     /* implies no service. No Home/Roam network available.*/
    BT_HS_SERVICE_ON          = 2,     /* implies presence of service. Home/Roam network available */

    BT_HS_CALL_OFF          = 3,     /* means there are no calls in progress */
    BT_HS_CALL_ON           = 4,     /* means at least one call is in progress */

    BT_HS_CALLSETUP_DONE        = 5,     /* means not currently in call set up. */
    BT_HS_CALLSETUP_INCOMING       = 6,     /* means an incoming call process ongoing */
    BT_HS_CALLSETUP_OUTGOING     = 7,     /* means an outgoing call set up is ongoing */
    BT_HS_CALLSETUP_REMOTE_ALERTED     = 8,     /* means remote party being alerted in an outgoing call. */

    BT_HS_CALLHELD_NONE            = 9,    /* No calls held */
    BT_HS_CALLHELD_HOLD_WITH_ACTIVE= 10,    /* Call is placed on hold or active/held calls swapped (The AG has both an active AND a held call)*/
    BT_HS_CALLHELD_HOLD        = 11,    /* Call on hold, no active call */

    BT_HS_BATTCHG0       = 12,    /* battery info*/
    BT_HS_BATTCHG1           = 13,    /* stream channel opened */
    BT_HS_BATTCHG2           = 14,    /* stream channel suspended */
    BT_HS_BATTCHG3            = 15,    /* stream channel closed */
    BT_HS_BATTCHG4         = 16,    /* remote control channel open */
    BT_HS_BATTCHG5    = 17,    /* remote control channel open by peer */

    BT_HS_SIGNAL0        = 18,    /* signal info*/
    BT_HS_SIGNAL1     = 19,
    BT_HS_SIGNAL2     = 20,
    BT_HS_SIGNAL3            = 21,
    BT_HS_SIGNAL4           = 22,
    BT_HS_SIGNAL5           = 23,

    BT_HS_ROAM_OFF             = 24,/* roam off*/
    BT_HS_ROAM_ON         = 25,
}RK_BT_HF_CIEV_STATUS;

typedef enum {
    RK_BTHF_VOL_SPK = 0,
    RK_BTHF_VOL_MIC = 1
} RK_BTHF_VOLUME_TYPE_T;

typedef struct RKBluetooth RKBluetooth;

#define RKBSA_AV_RC_PLAY        0x44        /* play */
#define RKBSA_AV_RC_STOP        0x45        /* stop */
#define RKBSA_AV_RC_PAUSE       0x46        /* pause */
#define RKBSA_AV_RC_FORWARD     0x4B        /* forward */
#define RKBSA_AV_RC_BACKWARD    0x4C        /* backward */

typedef void (*BTEventListener) (void *userdata, int what, int arg1, int arg2, void *data);
typedef void (*discovery_cb_t) (void *userdata, const char *bt_name, const char bt_addr[6], int is_completed);

/* event callback for hs applications */
typedef void (*UserHsCallback)(void *userdata, tBSA_HS_EVT event, tBSA_HS_MSG *p_data, void *data);
typedef int (*feed_mic_data_cb) (void *userdata, unsigned char *mic_data, int mic_len);

RKBluetooth *rokidbt_create();
int  rokidbt_init(RKBluetooth *bt, const char *bt_name);
void rokidbt_destroy(RKBluetooth *bt);

void rokidbt_set_event_listener(RKBluetooth *bt, BTEventListener listener, void *userdata);

void rokidbt_set_discovery_cb(RKBluetooth *bt, discovery_cb_t cb, void *d);
int  rokidbt_discovery(RKBluetooth *bt);
void rokidbt_set_name(RKBluetooth *bt, const char *name);
int  rokidbt_get_disc_devices_count(RKBluetooth *bt);
int  rokidbt_get_disc_devices(RKBluetooth *bt, BTDevice *dev_array, int arr_len);
int  rokidbt_get_bonded_devices(RKBluetooth *bt, BTDevice *dev_array, int arr_len);
int  rokidbt_find_bonded_device(BTDevice *dev);
void rokidbt_set_visibility(int visible);
void rokidbt_set_visibility_full(int discoverable, int connectable);
void rokidbt_set_ble_visibility(int visible);
void rokidbt_set_ble_visibility_full(int discoverable, int connectable);

void rokidbt_discovery_cancel(RKBluetooth *bt);
int  rokidbt_a2dp_enable(RKBluetooth *bt);
int  rokidbt_a2dp_open(RKBluetooth *bt, BTDevice *dev);
int  rokidbt_a2dp_start(RKBluetooth *bt, BTDevice *dev);
int  rokidbt_a2dp_start_with_aptx(RKBluetooth *bt, BTDevice *dev);
int  rokidbt_a2dp_close(RKBluetooth *bt);
int  rokidbt_a2dp_close_device(RKBluetooth *bt, BTDevice *dev);
void rokidbt_a2dp_disable(RKBluetooth *bt);
void rokidbt_a2dp_send_inc_volume(RKBluetooth *bt);
void rokidbt_a2dp_send_dec_volume(RKBluetooth *bt);
int  rokidbt_a2dp_is_enabled(RKBluetooth *bt);
int  rokidbt_a2dp_is_opened(RKBluetooth *bt);
int  rokidbt_a2dp_get_connected_devices(RKBluetooth *bt, BTDevice *dev_array, int arr_len);

int  rokidbt_a2dp_sink_enable(RKBluetooth *bt);
int  rokidbt_a2dp_sink_close(RKBluetooth *bt);
int  rokidbt_a2dp_sink_open(RKBluetooth *bt, BTDevice *dev);
int  rokidbt_a2dp_sink_close_device(RKBluetooth *bt, BTDevice *dev);
void rokidbt_a2dp_sink_disable(RKBluetooth *bt);
int  rokidbt_a2dp_sink_get_connected_devices(RKBluetooth *bt, BTDevice *dev_array, int arr_len);

int rokidbt_a2dp_sink_getplay(RKBluetooth *bt);
void rokidbt_a2dp_sink_send_play(RKBluetooth *bt);
void rokidbt_a2dp_sink_send_stop(RKBluetooth *bt);
void rokidbt_a2dp_sink_send_pause(RKBluetooth *bt);
void rokidbt_a2dp_sink_send_forward(RKBluetooth *bt);
void rokidbt_a2dp_sink_send_backward(RKBluetooth *bt);
void rokidbt_a2dp_sink_get_element_attrs(RKBluetooth *bt);

int rokidbt_ble_send_buf(RKBluetooth *bt, uint16_t uuid, char *buf, int len);
int  rokidbt_ble_enable(RKBluetooth *bt);
int  rokidbt_ble_disable(RKBluetooth *bt);
int  rokidbt_ble_is_enabled(RKBluetooth *bt);

/*HFP HS*/
int rokidbt_hs_enable(RKBluetooth *bt, UserHsCallback s_pHsCallback, feed_mic_data_cb feed_mic, void *userdata);


void rokidbt_hs_disable(RKBluetooth *bt);


int rokidbt_hs_open(RKBluetooth *bt, BTDevice *dev);


int rokidbt_hs_close(RKBluetooth *bt);


int rokidbt_hs_ansercall(RKBluetooth *bt);


int rokidbt_hs_hangup(RKBluetooth *bt);

int rokidbt_hs_dial_num(RKBluetooth *bt, const char *num);

int rokidbt_hs_last_num_dial(RKBluetooth *bt);


int rokidbt_hs_send_dtmf(RKBluetooth *bt, char dtmf);

int rokidbt_hs_set_volume(RKBluetooth *bt, RK_BTHF_VOLUME_TYPE_T type, int volume);

int rokidbt_hs_mute_unmute_microphone(RKBluetooth *bt, unsigned char bMute);

#ifdef __cplusplus
}
#endif

#endif /* */
