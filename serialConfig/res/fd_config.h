#ifndef _FD_CONFIG_H_
#define _FD_CONFIG_H_

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include "cJSON.h"

class FdConfig
{
private:
    FdConfig();
    ~FdConfig();

    cJSON* config_json; 
    void initConfigValue(std::string path);
    
public:
    static FdConfig *_GetInstance()
    {
        static FdConfig instance;
        return &instance;
    }

    /**
     * 获取参数类型为string的配置值
     * @param config_name 配置的模块名称
     * @param config_key  配置的键名
     * @return 返回配置的值,如果配置文件中找不到对应配置，将返回NULL
     * */
    std::string getConfigString(std::string config_name, 
                                std::string config_key);

    /**
     * 获取参数类型为string的配置值
     * @param config_name 配置的模块名称
     * @param config_key  配置的键名
     * @return 返回配置的值,如果配置文件中找不到对应配置，将返回false
     * */
    bool getConfigBool(std::string config_name, 
                       std::string config_key);

    /**
     * 获取参数类型为string的配置值
     * @param config_name 配置的模块名称
     * @param config_key  配置的键名
     * @return 返回配置的值,如果配置文件中找不到对应配置，将返回-1
     * */
    int getConfigInt(std::string config_name, 
                     std::string config_key);       

    std::map<std::string,int> pin_map;
    std::map<std::string,int> value_map;

};

#endif //FD_CONFIG_H