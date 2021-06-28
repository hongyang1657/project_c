#ifndef AIRKISS_SRC_H
#define AIRKISS_SRC_H

#include "api_net_config.h"

void airkiss_start(void);
void airkiss_stop(void);
void airkiss_regist_callback(net_config_callback callback);

#endif
