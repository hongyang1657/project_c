#ifndef _WAV_SRC_h
#define _WAV_SRC_h

#include "api_net_config.h"

void wav_start(void);
void wav_stop(void);
void wav_input(void *audio, size_t size);
void wav_regist_callback(net_config_callback callback);

#endif

