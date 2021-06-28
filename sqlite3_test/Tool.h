//
//  Tool.h
//  generator
//
//  Created by Trilobite on 2020/7/9.
//  Copyright © 2020 Trilobite. All rights reserved.
//

#ifndef Tool_h
#define Tool_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <cstring>

//static std::vector<std::string>set_v;

//字符串分割
std::vector<std::string> split(const std::string& str, const std::string& pattern);
std::vector<std::string> splits(const std::string& str, const std::string& pattern);

//获取时间戳
long long getTimestamp();

std::vector <std::string> subdump(std::string str);

void Subset(std::vector<std::string> &set_v, std::string a, int n, int len);

std::string & replace_all(std::string &str,const std::string &old_value,const std::string &new_value);
void GetFileNames(std::string path, std::vector<std::string>& filenames);

#endif /* Tool_h */
