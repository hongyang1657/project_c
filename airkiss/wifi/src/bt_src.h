#ifndef _BT_SRC_H_
#define _BT_SRC_H_

#include "api_net_config.h"

void bt_start(void);
void bt_stop(void);
void bt_regist_callback(net_config_callback callback);

#endif

