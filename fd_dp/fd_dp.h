#ifndef FD_DP_H
#define FD_DP_H

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <string>
#include <vector>
#include "fd_intent_npu.h"
#include "fd_slot_npu.h"
#include "fd_resource.h"
#include "fd_config.h"

#define ROW_SIZE 200
#define COLUMN_SIZE 300



    FdResource *resource;
    FdConfig *config;
    FdIntentNpu *intentNpu;
    FdSlotNpu   *slotNpu;

    float query[ROW_SIZE][COLUMN_SIZE];
    int query_index[ROW_SIZE];
    std::string query_chars[ROW_SIZE];
    float query_slot[ROW_SIZE][COLUMN_SIZE];
    std::string query_str;

    void intentEmbeddingEventList(std::vector<std::string> event_list);
    void slotEmbeddingEventList(std::vector<std::string> event_list);

    std::vector<std::string> checkoutSlotNameWithIntent(std::string intent);
    int checkoutSlotWithNpu(int intent_index, std::string &slot_name, std::string &slot_value);
    
    void _RecvEventsList(const std::vector<std::string> &events_list);

    static void StartDP();




#endif //FD_DP_H