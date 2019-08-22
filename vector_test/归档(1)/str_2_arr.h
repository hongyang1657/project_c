//
//  str_2_arr.hpp
//  cppDemo
//
//  Created by PC-wzj on 2019/2/18.
//  Copyright © 2019 PC-wzj. All rights reserved.
//

#ifndef str_2_arr_h
#define str_2_arr_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <fstream>

class Tool {
public:
    
    /**
     读取文件设置字典信息

     @param path 文件路径
     */
    void set_dictionay_for_file(std::string path);
    
    /**
     字典对照

     @param str 需对照原串
     @return 字典对照结果（下标数组）
     */
    int * str_2_arr(std::string str);
    
    /**
     字典对照

     @param char_str 需对照原串
     @param path 对照结果存储文件路径
     @return 字典对照结果(下标数组)
     */
    int * str_2_arr(std::string str, std::string path);
    
    /**
     单字符字典对照

     @param char_str 查询字符
     @return 对应字典下表
     */
    int char_2_index(std::string char_str);
    
    bool isLog = false;
private:
    //工具字典，通过set_dictionary设置，无初始化
    std::map<std::string, int> dic;
};

#endif /* str_2_arr_h */
