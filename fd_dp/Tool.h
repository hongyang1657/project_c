//
//  Tool.h
//  AM
//
//  Created by Trilobite on 2019/4/10.
//  Copyright © 2019 Trilobite. All rights reserved.
//

#ifndef Tool_h
#define Tool_h

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <sys/time.h>


/**
 判断是否为中文
 
 @param p 输入字符
 @return 判断结果 0不是汉字， 1是中文汉字
 */
int is_zh_ch(char p);

int includeChinese(std::string str);

/**
 字符串去掉收尾引号

 @param str 目标字符串
 @return 结果
 */
std::string clearHeadTailQuotes(std::string &str);

/**
 字符串分割

 @param str 原串
 @param pattern 分割工具字符串
 @return 分割结果列表
 */
std::vector<std::string> split(const std::string& str, const std::string& pattern);

/**
 字符分离（可分离中英文）

 @param str 目标字符串
 @return 结果列表
 */
std::vector <std::string> subdump(std::string str);

float softMax(float *values, int num);
float softMax(float *values, float value, int num);

/**
 获取时间戳

 @return 时间戳
 */
std::string getTime();

/**
 * 获取当前时间戳
 * @return 当前时间戳（精确到秒）
 **/
long getCurrentTime();

/**
 * 英文数字字符串转阿拉伯数字
 * 
 * @param numStr 英文数字字符串
 * @return 对应的阿拉伯数字
 **/
int EN_number(std::string numStr);

/**
 * 中文数字字符串转阿拉伯数字
 * 
 * @param numStr 中文数字字符串
 * @return 对应的阿拉伯数字
 **/
int ZH_number(std::string numStr);

/**
 * 打印当前时间
 * */
void getDateAndTime();

#endif /* Tool_h */
