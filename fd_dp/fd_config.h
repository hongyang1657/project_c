#ifndef FD_CONFIG_H
#define FD_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "cJSON.h"
#include "fd_log.h"

#define CONFIG_PATH "/data/found/data/config.conf"

class FdConfig
{
private:
    FdConfig();
    ~FdConfig();

    void set_value_config_path(std::string path);
    
public:
    static FdConfig *_GetInstance()
    {
        static FdConfig instance;
        return &instance;
    }

    bool _intent_model;
    bool _slot_model;
    bool _multi_round;
    bool _offline;
    bool _online;

};


#endif //FD_CONFIG_H